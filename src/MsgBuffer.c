/*
 * This file is part of see-object.
 *
 * see-object is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * see-object is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with see-object.  If not, see <http://www.gnu.org/licenses/>.
 */

/**
 * \file MsgBuffer.c
 * \brief This file implements The SeeMsgBuffer instance and class.
 *
 * This file implements a MessageBuffer. The message buffer consist of a
 * header of it own, clients can deduce from the message header which msg
 * parts should be embedded in the entire message. All the message parts
 * also contain a header.
 * Every integer is written as to a buffer in network byte order, that is
 * big endian. The buffer can also contain strings or double precision
 * floating point numbers.
 *
 * \private
 */

#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>
#include <stdbool.h>

#include "MetaClass.h"
#include "MsgBuffer.h"
#include "see_object_config.h"
#include "utilities.h"
#include "RuntimeError.h"
#include "IncomparableError.h"


/**
 * The start of each message. This can be used to syncronise with a bytestream.
 */
const char* g_see_msg_start = "SMSG";


/**
 * \brief If the message part needs to allocate resources they are freed here
 *
 * After this function finishes, a SeeMessageBufferPart should be a fresh
 * uninitalized message buffer.
 *
 * @param [in,out] mbp The SeeMsgPart whose resource should be freed.
 * \private
 */
static void
msg_part_destroy_content(SeeMsgPart* mbp)
{
    if (mbp->value_type == SEE_MSG_PART_NOT_INIT)
        return;

    switch(mbp->value_type) {
        case SEE_MSG_PART_INT32_T:
        case SEE_MSG_PART_INT64_T:
        case SEE_MSG_PART_UINT32_T:
        case SEE_MSG_PART_UINT64_T:
        case SEE_MSG_PART_FLOAT_T:
        case SEE_MSG_PART_DOUBLE_T:
            break; // nothing to free.
        case SEE_MSG_PART_STRING_T:
            free(mbp->value.str_val);
            mbp->value.str_val = NULL;
            break;
        default:
            assert(0 == 1);
    }
    mbp->value_type = SEE_MSG_PART_NOT_INIT;
    mbp->length = 0;
}

/* **** functions that implement SeeMsgPart or override SeeObject **** */

static int
msg_part_init(
    SeeMsgPart* msg_part,
    const SeeMsgPartClass* msg_part_cls
    )
{
    int ret = SEE_SUCCESS;
    const SeeObjectClass* parent_cls = SEE_OBJECT_GET_CLASS(
        msg_part
    );

    parent_cls->object_init(
        SEE_OBJECT(msg_part),
        SEE_OBJECT_CLASS(msg_part_cls)
        );

    msg_part->value_type = SEE_MSG_PART_NOT_INIT;

    return ret;
}

static void
msg_part_destroy(SeeObject* obj)
{
    msg_part_destroy_content(SEE_MSG_PART(obj));
    see_object_class()->destroy(obj);
}

static int
part_init(const SeeObjectClass* cls, SeeObject* obj, va_list args)
{
    const SeeMsgPartClass* msg_part_cls = SEE_MSG_PART_CLASS(cls);
    SeeMsgPart* msg_part = SEE_MSG_PART(obj);

    /*Extract parameters here from va_list args here.*/
    (void) args;

    return msg_part_cls->msg_part_init(
        msg_part,
        msg_part_cls
        );

}

static int msg_part_equal(
    const SeeObject*  so_part,
    const SeeObject*  so_other,
    int*              result,
    SeeError**        error
    )
{
    const SeeMsgPart* part, *other;
    int ret, isinstance;
    if (so_part == so_other) {
        *result = 1;
        return SEE_SUCCESS;
    }

#if !defined(NDEBUG)
    see_object_is_instance_of(
        so_part,
        SEE_OBJECT_CLASS(see_msg_part_class()),
        &isinstance
        );
    assert(isinstance);
#endif

    ret = see_object_is_instance_of(
        so_other,
        SEE_OBJECT_CLASS(see_msg_part_class()),
        &isinstance
        );

    assert(ret == SEE_SUCCESS);
    if (!isinstance) {
        see_incomparable_error_create(
            error,
            SEE_OBJECT_CLASS(so_part),
            SEE_OBJECT_CLASS(so_other)
            );
        return SEE_ERROR_INCOMPARABLE;
    }
    part = (const SeeMsgPart*) so_part;
    other= (const SeeMsgPart*) so_other;

    if (part->value_type != other->value_type) {
        *result = 0;
        return SEE_SUCCESS;
    }
    if (part->length != other->length) {
        *result = 0;
    }
    switch (part->value_type)
    {
        case SEE_MSG_PART_INT32_T:
            *result = part->value.int32_val == other->value.int32_val;
            break;
        case SEE_MSG_PART_UINT32_T:
            *result = part->value.uint32_val == other->value.uint32_val;
            break;
        case SEE_MSG_PART_INT64_T:
            *result = part->value.int64_val == other->value.int64_val;
            break;
        case SEE_MSG_PART_UINT64_T:
            *result = part->value.uint64_val == other->value.uint64_val;
            break;
        case SEE_MSG_PART_STRING_T:
            *result = strcmp(part->value.str_val, other->value.str_val) == 0;
            break;
        case SEE_MSG_PART_FLOAT_T:
            *result = part->value.float_val == other->value.float_val;
            break;
        case SEE_MSG_PART_DOUBLE_T:
            *result = part->value.double_val == other->value.double_val;
            break;
        case SEE_MSG_PART_NOT_INIT:
            *result = 1; // content is irrelevant.
            break;
        default:
            assert(0 == 1);
            return SEE_INVALID_ARGUMENT;
    }
    return SEE_SUCCESS;
}

static int
msg_part_not_equal(
    const SeeObject*    part,
    const SeeObject*    other,
    int*                result,
    SeeError**          error
    )
{
    int ret, tres;
    ret = msg_part_equal(part, other, &tres, error);
    *result = !result;
    return ret;
}

static int
msg_part_length(
    const SeeMsgPart*   part,
    uint32_t*           size,
    SeeError**          error
    )
{
    if (part->value_type == SEE_MSG_PART_NOT_INIT ||
        part->value_type >= SEE_MSG_PART_TRAILER) {
        errno = EINVAL;
        see_runtime_error_create(error, errno);
        return SEE_ERROR_RUNTIME;
    }

    assert(sizeof(uint32_t) == sizeof(float));
    assert(sizeof(uint64_t) == sizeof(double));
    switch (part->value_type) {
        case SEE_MSG_PART_INT32_T:
        case SEE_MSG_PART_UINT32_T:
        case SEE_MSG_PART_FLOAT_T:
            *size = sizeof(int32_t);
            break;
        case SEE_MSG_PART_INT64_T:
        case SEE_MSG_PART_UINT64_T:
        case SEE_MSG_PART_DOUBLE_T:
            *size = sizeof(int64_t);
            break;
        case SEE_MSG_PART_STRING_T:
            *size = part->length - (
                sizeof(part->value_type) + sizeof(part->length)
                );
            break;
        default:
            assert(0 == 1);
            return SEE_ERROR_INTERNAL;
    }

    return SEE_SUCCESS;
}

static int msg_part_value_type(
    const SeeMsgPart* part,
    uint8_t*          value_type
    )
{
    *value_type = part->value_type;
    return SEE_SUCCESS;
}

static int
msg_part_write_int32 (
    SeeMsgPart*   part,
    int32_t       value,
    SeeError**    error_out
    )
{
    (void) error_out;
    msg_part_destroy_content(part);

    part->value_type        = SEE_MSG_PART_INT32_T;
    part->value.int32_val   = value;

    part->length = sizeof(part->value_type) + sizeof(part->value.int32_val);

    return SEE_SUCCESS;
}

static int
msg_part_get_int32 (
    const SeeMsgPart* part,
    int32_t*          value,
    SeeError**        error_out
    )
{
    if (part->value_type != SEE_MSG_PART_INT32_T) {
        see_msg_part_type_error_new(
            error_out,
            SEE_MSG_PART_INT32_T,
            part->value_type
            );
        return SEE_ERROR_MSG_PART_TYPE;
    }

    int32_t host_val = part->value.int32_val;
    *value = host_val;

    return SEE_SUCCESS;
}

static int
msg_part_write_uint32(
    SeeMsgPart*   part,
    uint32_t      value,
    SeeError**    error_out
    )
{
    (void) error_out;

    msg_part_destroy_content(part);

    part->value_type        = SEE_MSG_PART_UINT32_T;
    part->value.uint32_val  = value;

    part->length = sizeof(part->value_type) + sizeof(part->value.uint32_val);

    return SEE_SUCCESS;
}

static int
msg_part_get_uint32 (
    const SeeMsgPart* part,
    uint32_t*         value,
    SeeError**        error_out
    )
{
    if (part->value_type != SEE_MSG_PART_UINT32_T) {
        see_msg_part_type_error_new(
            error_out,
            part->value_type,
            SEE_MSG_PART_UINT32_T
        );
        return SEE_ERROR_MSG_PART_TYPE;
    }

    uint32_t host_val = part->value.uint32_val;
    *value = host_val;

    return SEE_SUCCESS;
}

static int
msg_part_write_int64(
    SeeMsgPart*   part,
    int64_t       value,
    SeeError**    error_out
    )
{
    (void) error_out;

    msg_part_destroy_content(part);

    part->value_type        = SEE_MSG_PART_INT64_T;
    part->value.int64_val   = value;

    part->length = sizeof(part->value_type) + sizeof(part->value.int64_val);

    return SEE_SUCCESS;
}

static int
msg_part_get_int64 (
    const SeeMsgPart* part,
    int64_t*          value,
    SeeError**        error_out
    )
{
    if (part->value_type != SEE_MSG_PART_INT64_T) {
        see_msg_part_type_error_new(
            error_out,
            part->value_type,
            SEE_MSG_PART_INT64_T
        );
        return SEE_ERROR_MSG_PART_TYPE;
    }

    int64_t host_val = part->value.int64_val;
    *value = host_val;

    return SEE_SUCCESS;
}

static int
msg_part_write_uint64(
    SeeMsgPart*   part,
    uint64_t      value,
    SeeError**    error_out
    )
{
    (void) error_out;

    msg_part_destroy_content(part);

    part->value_type        = SEE_MSG_PART_UINT64_T;
    part->value.uint64_val  = value;

    part->length = sizeof(part->value_type) + sizeof(part->value.uint64_val);

    return SEE_SUCCESS;
}

static int
msg_part_get_uint64 (
    const SeeMsgPart* part,
    uint64_t*         value,
    SeeError**        error_out
    )
{
    if (part->value_type != SEE_MSG_PART_UINT64_T) {
        see_msg_part_type_error_new(
            error_out,
            part->value_type,
            SEE_MSG_PART_UINT64_T
            );
        return SEE_ERROR_MSG_PART_TYPE;
    }

    int64_t host_val = part->value.int64_val;
    *value = host_val;

    return SEE_SUCCESS;
}

static int
msg_part_write_string(
    SeeMsgPart*   part,
    const char*   value,
    size_t        length,
    SeeError**    error_out
    )
{
    char* duplicate = strdup(value);
    if (!duplicate) {
        see_runtime_error_create(error_out, errno);
        return SEE_ERROR_RUNTIME;
    }

    msg_part_destroy_content(part);

    part->length        = sizeof(part->value_type) + sizeof(part->length) + length;
    part->value_type    = SEE_MSG_PART_STRING_T;
    part->value.str_val = duplicate;

    return SEE_SUCCESS;
}

static int
msg_part_get_string(
    const SeeMsgPart* part,
    char**            value_out,
    SeeError**        error_out
    )
{
    char* out = NULL;

    if (part->value_type != SEE_MSG_PART_STRING_T) {
        see_msg_part_type_error_new(
            error_out,
            part->value_type,
            SEE_MSG_PART_STRING_T
        );
        return SEE_ERROR_MSG_PART_TYPE;
    }

    out = strdup(part->value.str_val);
    if (!out) {
        see_runtime_error_create(error_out, errno);
        return SEE_ERROR_RUNTIME;
    }
    *value_out = out;

    return SEE_SUCCESS;
}

static int
msg_part_write_float(
    SeeMsgPart* part,
    float       value,
    SeeError**  error_out
    )
{
    (void) error_out;

    part->value.float_val   = value;
    part->value_type        = SEE_MSG_PART_FLOAT_T;

    part->length = sizeof(part->value_type) + sizeof(part->value.float_val);

    return SEE_SUCCESS;
}

static int
msg_part_get_float(
    const SeeMsgPart*   part,
    float*              value_out,
    SeeError**          error_out
    )
{
    if (part->value_type != SEE_MSG_PART_FLOAT_T) {
        see_msg_part_type_error_new(
            error_out,
            part->value_type,
            SEE_MSG_PART_FLOAT_T
        );
        return SEE_ERROR_MSG_PART_TYPE;
    }


    *value_out = part->value.float_val;

    return SEE_SUCCESS;
}

static int
msg_part_write_double(
    SeeMsgPart* part,
    double      value,
    SeeError**  error_out
    )
{
    (void) error_out;

    part->value.double_val  = value;
    part->value_type        = SEE_MSG_PART_DOUBLE_T;

    part->length = sizeof(part->value_type) + sizeof(part->value.double_val);

    return SEE_SUCCESS;
}

static int
msg_part_get_double(
    const SeeMsgPart*   part,
    double*             value_out,
    SeeError**          error_out
    )
{
    if (part->value_type != SEE_MSG_PART_DOUBLE_T) {
        see_msg_part_type_error_new(
            error_out,
            part->value_type,
            SEE_MSG_PART_DOUBLE_T
        );
        return SEE_ERROR_MSG_PART_TYPE;
    }

    *value_out = part->value.double_val;

    return SEE_SUCCESS;
}

static int
msg_part_buffer_length(
    const SeeMsgPart* part,
    size_t*           size,
    SeeError**        error_out
    )
{
    if ( part->value_type == SEE_MSG_PART_NOT_INIT ||
         part->value_type >= SEE_MSG_PART_TRAILER ) {
        errno = EINVAL;
        see_runtime_error_create(error_out, errno);
        return SEE_ERROR_RUNTIME;
    }

    *size = part->length;

    return SEE_SUCCESS;
}

static int
msg_part_write(
    const SeeMsgPart*   part,
    void*               buffer,
    SeeError**          error_out
    )
{
    char*       bytes    = buffer;
    size_t      nwritten = 0;

    // These variables are to contain the representation of a value in
    // network byte order.
    union {
        int32_t     i32;
        uint32_t    ui32;
        int64_t     i64;
        uint64_t    ui64;
        float       flt;
        double      dbl;
    }net_order;

    memcpy(&bytes[nwritten], &part->value_type, sizeof(part->value_type));
    nwritten += sizeof(part->value_type);

    switch (part->value_type) {
        case SEE_MSG_PART_INT32_T:
            net_order.i32 = see_host_to_network32(part->value.int32_val);
            memcpy(&bytes[nwritten], &net_order.i32, sizeof(net_order.i32));
            nwritten += sizeof(net_order.i32);
            break;
        case SEE_MSG_PART_INT64_T:
            net_order.i64 = see_host_to_network64(part->value.int64_val);
            memcpy(&bytes[nwritten], &net_order.i64, sizeof(net_order.i64));
            nwritten += sizeof(net_order.i64);
            break;
        case SEE_MSG_PART_UINT32_T:
            net_order.ui32 = see_host_to_network32(part->value.uint32_val);
            memcpy(&bytes[nwritten], &net_order.ui32, sizeof(net_order.ui32));
            nwritten += sizeof(net_order.ui32);
            break;
        case SEE_MSG_PART_UINT64_T:
            net_order.ui64 = see_host_to_network64(part->value.uint64_val);
            memcpy(&bytes[nwritten], &net_order.ui64, sizeof(net_order.ui64));
            nwritten += sizeof(net_order.ui64);
            break;
        case SEE_MSG_PART_STRING_T:
            {
                // write the length of the part to be able to deduce the length of
                // the part in network byte order.
                uint32_t net_length = see_host_to_network32(part->length);
                memcpy(&bytes[nwritten], &net_length, sizeof(net_length));
                nwritten += sizeof(part->length);
                //write the string itself.
                size_t header_len = sizeof(part->value_type) + sizeof(part->length);
                memcpy(
                    &bytes[nwritten],
                    part->value.str_val,
                    part->length - header_len
                );
                nwritten += part->length - header_len;
            }
            break;

        case SEE_MSG_PART_FLOAT_T:
            assert(sizeof(net_order.flt) == sizeof(part->value.int32_val));
            net_order.i32 = see_host_to_network32(part->value.int32_val);
            memcpy(&bytes[nwritten], &net_order.flt, sizeof(net_order.flt));
            nwritten += sizeof(float);
            break;
        case SEE_MSG_PART_DOUBLE_T:
            assert(sizeof(net_order.dbl) == sizeof(part->value.uint64_val));
            net_order.ui64 = see_host_to_network64(part->value.uint64_val);
            memcpy(&bytes[nwritten], &net_order.dbl, sizeof(net_order.dbl));
            nwritten += sizeof(net_order.dbl);
            break;
        default:
            errno = EINVAL;
            see_runtime_error_create(error_out, errno);
            return SEE_ERROR_RUNTIME;
    }

    return SEE_SUCCESS;
}


static int
msg_part_read(
    SeeMsgPart**    part_out,
    const void*     buffer,
    size_t          bufsiz,
    size_t*         size_out,
    SeeError**      error_out
    )
{
    const char* bytes       = buffer;
    size_t      nread       = 0;
    uint8_t     type;
    uint32_t    length      = 0;
    uint32_t    strlength   = 0;
    int         ret         = SEE_SUCCESS;
    char*       str         = NULL;
    SeeMsgPart* new_part    = NULL;

    // The next variables are going to contain the variables in network order.
    union {
        int32_t     i32;
        uint32_t    u32;
        int64_t     i64;
        uint64_t    u64;
        float       flt;
        double      dbl;
    }net_order;

    ret = see_msg_part_new(&new_part, error_out);
    if (ret)
        return ret;

    if (nread + 1 > bufsiz) {
        ret = see_msg_invalid_error_new(error_out);
        goto fail;
    }


    memcpy(&type, &bytes[nread], sizeof(type));
    nread += sizeof(type);
    new_part->value_type = type;

    switch (type) {
        case SEE_MSG_PART_INT32_T:
            length = sizeof(new_part->value.int32_val) + sizeof(new_part->value_type);
            if (length > bufsiz) {
                ret = SEE_ERROR_MSG_INVALID;
                see_msg_invalid_error_new(error_out);
                goto fail;
            }
            memcpy(&net_order.i32,
                &bytes[nread],
                sizeof(new_part->value.int32_val)
                );
            new_part->value.int32_val = see_network_to_host32(net_order.i32);

            nread += sizeof(new_part->value.int32_val);
            new_part->length = length;

            break;
        case SEE_MSG_PART_INT64_T:
            length = sizeof(new_part->value.int64_val) + sizeof(new_part->value_type);
            if (length > bufsiz) {
                ret = SEE_ERROR_MSG_INVALID;
                see_msg_invalid_error_new(error_out);
                goto fail;
            }

            memcpy(&net_order.i64,
                   &bytes[nread],
                   sizeof(new_part->value.int64_val)
                    );
            new_part->value.int64_val = see_network_to_host64(net_order.i64);

            nread += sizeof(new_part->value.int64_val);
            new_part->length = length;

            break;
        case SEE_MSG_PART_UINT32_T:
            length = sizeof(new_part->value.uint32_val) + sizeof(new_part->value_type);
            if (length > bufsiz) {
                ret = SEE_ERROR_MSG_INVALID;
                see_msg_invalid_error_new(error_out);
                goto fail;
            }

            memcpy(&net_order.u32,
                   &bytes[nread],
                   sizeof(new_part->value.uint32_val)
                   );
            new_part->value.uint32_val = see_network_to_host32(net_order.u32);

            nread += sizeof(new_part->value.int32_val);
            new_part->length = length;

            break;
        case SEE_MSG_PART_UINT64_T:
            length = sizeof(new_part->value.uint64_val) + sizeof(new_part->value_type);
            if (length > bufsiz) {
                ret = SEE_ERROR_MSG_INVALID;
                see_msg_invalid_error_new(error_out);
                goto fail;
            }

            memcpy(&net_order.u64,
                   &bytes[nread],
                   sizeof(new_part->value.uint64_val)
                   );
            new_part->value.uint64_val = see_network_to_host64(net_order.u64);

            nread += sizeof(new_part->value.int64_val);
            new_part->length = length;
            break;
        case SEE_MSG_PART_STRING_T:
            memcpy(&length, &bytes[nread], sizeof(new_part->length));
            nread += sizeof(new_part->length);
            length = see_network_to_host32(length);
            if (length > bufsiz) {
                ret = SEE_ERROR_MSG_INVALID;
                see_msg_invalid_error_new(error_out);
                goto fail;
            }

            if (length < (sizeof(new_part->value_type) + sizeof(new_part->length)) ||
                length > bufsiz)
            {
                see_msg_invalid_error_new(error_out);
                ret = SEE_ERROR_MSG_INVALID;
                goto fail;
            }
            strlength = length -
                (sizeof(new_part->value_type) + sizeof(new_part->length));

            str = malloc(strlength + 1 );
            if (!str)
            {
                see_runtime_error_create(error_out, errno);
                goto fail;
            }
            memcpy(str, &bytes[nread], strlength);
            str[strlength] = '\0';
            nread += strlength;
            new_part->value.str_val = str;
            new_part->length = length;

            break;
        case SEE_MSG_PART_FLOAT_T:
            length = sizeof(new_part->value.float_val) + sizeof(new_part->value_type);
            if (length > bufsiz) {
                ret = SEE_ERROR_MSG_INVALID;
                see_msg_invalid_error_new(error_out);
                goto fail;
            }

            assert(sizeof(net_order.u32) == sizeof(new_part->value.float_val));
            memcpy(&net_order.flt,
                   &bytes[nread],
                   sizeof(new_part->value.float_val)
                   );
            net_order.u32 = see_network_to_host32(net_order.u32);
            new_part->value.float_val = net_order.flt;

            nread += sizeof(new_part->value.float_val);
            new_part->length = length;
            break;
        case SEE_MSG_PART_DOUBLE_T:
            length = sizeof(new_part->value.double_val) + sizeof(new_part->value_type);
            if (length > bufsiz) {
                ret = SEE_ERROR_MSG_INVALID;
                see_msg_invalid_error_new(error_out);
                goto fail;
            }

            assert(sizeof(net_order.u64) == sizeof(new_part->value.double_val));
            memcpy(&net_order.dbl,
                   &bytes[nread],
                   sizeof(new_part->value.double_val)
                   );
            net_order.u64 = see_network_to_host64(net_order.u64);
            new_part->value.double_val = net_order.dbl;

            nread += sizeof(new_part->value.double_val);
            new_part->length = length;
            break;
        default:
            ret = SEE_ERROR_MSG_INVALID;
            see_msg_invalid_error_new(error_out);
            goto fail;
    }

    *part_out = new_part;
    *size_out = nread;

    return ret;

fail:

    see_object_decref(SEE_OBJECT(new_part));
    free(str);

    return ret;
}

/* **** implementation of the public API **** */

int
see_msg_part_new(SeeMsgPart** mbp, SeeError** error_out)
{
    const SeeObjectClass* cls = SEE_OBJECT_CLASS(see_msg_part_class());

    if (!cls)
        return SEE_NOT_INITIALIZED;

    if (!mbp || *mbp)
        return SEE_INVALID_ARGUMENT;

    if (!error_out || *error_out)
        return SEE_INVALID_ARGUMENT;

    return cls->new_obj(cls, 0, SEE_OBJECT_REF(mbp));
}

int
see_msg_part_length(
    const SeeMsgPart* part,
    uint32_t*         length,
    SeeError**        error_out
    )
{
    const SeeMsgPartClass* cls;
    if (!part || !length)
        return SEE_INVALID_ARGUMENT;

    if (!error_out || *error_out)
        return SEE_INVALID_ARGUMENT;

    cls = SEE_MSG_PART_GET_CLASS(part);

    return cls->length(part, length, error_out);
}

int
see_msg_part_value_type(
    const SeeMsgPart* part,
    uint8_t*          val_type
    )
{
    const SeeMsgPartClass* cls;
    if (!part || !val_type)
        return SEE_INVALID_ARGUMENT;

    cls = SEE_MSG_PART_GET_CLASS(part);

    return cls->value_type(part, val_type);
}

int
see_msg_part_write_int32(
    SeeMsgPart*       part,
    int32_t           value,
    SeeError**        error_out
    )
{
    const SeeMsgPartClass* cls;
    if (!part)
        return SEE_INVALID_ARGUMENT;

    if (!error_out || *error_out)
        return SEE_INVALID_ARGUMENT;

    cls = SEE_MSG_PART_GET_CLASS(part);

    return cls->write_int32(part, value, error_out);
}

int
see_msg_part_get_int32(
    const SeeMsgPart* part,
    int32_t*          value,
    SeeError**        error_out
    )
{
    const SeeMsgPartClass* cls;
    if (!part || !value)
        return SEE_INVALID_ARGUMENT;

    if (!error_out || *error_out)
        return SEE_INVALID_ARGUMENT;

    cls = SEE_MSG_PART_GET_CLASS(part);

    return cls->get_int32(part, value, error_out);
}

int
see_msg_part_write_uint32(
    SeeMsgPart*       part,
    uint32_t          value,
    SeeError**        error_out
    )
{
    const SeeMsgPartClass* cls;
    if (!part)
        return SEE_INVALID_ARGUMENT;

    if (!error_out || *error_out)
        return SEE_INVALID_ARGUMENT;

    cls = SEE_MSG_PART_GET_CLASS(part);

    return cls->write_uint32(part, value, error_out);
}

int
see_msg_part_get_uint32(
    const SeeMsgPart* part,
    uint32_t*         value,
    SeeError**        error_out
    )
{
    const SeeMsgPartClass* cls;
    if (!part || !value)
        return SEE_INVALID_ARGUMENT;

    if (!error_out || *error_out)
        return SEE_INVALID_ARGUMENT;

    cls = SEE_MSG_PART_GET_CLASS(part);

    return cls->get_uint32(part, value, error_out);
}

int
see_msg_part_write_int64(
    SeeMsgPart*       part,
    int64_t           value,
    SeeError**        error_out
)
{
    const SeeMsgPartClass* cls;
    if (!part)
        return SEE_INVALID_ARGUMENT;

    if (!error_out || *error_out)
        return SEE_INVALID_ARGUMENT;

    cls = SEE_MSG_PART_GET_CLASS(part);

    return cls->write_int64(part, value, error_out);
}

int
see_msg_part_get_int64(
    const SeeMsgPart* part,
    int64_t*          value,
    SeeError**        error_out
)
{
    const SeeMsgPartClass* cls;
    if (!part || !value)
        return SEE_INVALID_ARGUMENT;

    if (!error_out || *error_out)
        return SEE_INVALID_ARGUMENT;

    cls = SEE_MSG_PART_GET_CLASS(part);

    return cls->get_int64(part, value, error_out);
}

int
see_msg_part_write_uint64(
    SeeMsgPart*       part,
    uint64_t          value,
    SeeError**        error_out
)
{
    const SeeMsgPartClass* cls;
    if (!part)
        return SEE_INVALID_ARGUMENT;

    if (!error_out || *error_out)
        return SEE_INVALID_ARGUMENT;

    cls = SEE_MSG_PART_GET_CLASS(part);

    return cls->write_uint64(part, value, error_out);
}

int
see_msg_part_get_uint64(
    const SeeMsgPart* part,
    uint64_t*         value,
    SeeError**        error_out
)
{
    const SeeMsgPartClass* cls;
    if (!part || !value)
        return SEE_INVALID_ARGUMENT;

    if (!error_out || *error_out)
        return SEE_INVALID_ARGUMENT;

    cls = SEE_MSG_PART_GET_CLASS(part);

    return cls->get_uint64(part, value, error_out);
}

int
see_msg_part_write_string(
    SeeMsgPart*       part,
    const char*       value,
    size_t            length,
    SeeError**        error_out
    )
{
    const SeeMsgPartClass* cls;
    if (!part || !value)
        return SEE_INVALID_ARGUMENT;

    if (!error_out || *error_out)
        return SEE_INVALID_ARGUMENT;

    cls = SEE_MSG_PART_GET_CLASS(part);

    return cls->write_string(part, value, length, error_out);
}

int
see_msg_part_get_string(
    const SeeMsgPart*   part,
    char**              value,
    SeeError**          error_out
    )
{
    const SeeMsgPartClass* cls;
    if (!part || !value)
        return SEE_INVALID_ARGUMENT;

    if (!error_out || *error_out)
        return SEE_INVALID_ARGUMENT;

    cls = SEE_MSG_PART_GET_CLASS(part);

    return cls->get_string(part, value, error_out);
}

int
see_msg_part_write_float(
    SeeMsgPart*       part,
    float             value,
    SeeError**        error_out
    )
{
    const SeeMsgPartClass* cls;
    if (!part)
        return SEE_INVALID_ARGUMENT;

    if (!error_out || *error_out)
        return SEE_INVALID_ARGUMENT;

    cls = SEE_MSG_PART_GET_CLASS(part);

    return cls->write_float(part, value, error_out);
}

int
see_msg_part_get_float(
    const SeeMsgPart* part,
    float*            value,
    SeeError**        error_out
    )
{
    const SeeMsgPartClass* cls;
    if (!part || !value)
        return SEE_INVALID_ARGUMENT;

    if (!error_out || *error_out)
        return SEE_INVALID_ARGUMENT;

    cls = SEE_MSG_PART_GET_CLASS(part);

    return cls->get_float(part, value, error_out);
}

int
see_msg_part_write_double(
    SeeMsgPart*       part,
    double            value,
    SeeError**        error_out
    )
{
    const SeeMsgPartClass* cls;
    if (!part)
        return SEE_INVALID_ARGUMENT;

    if (!error_out || *error_out)
        return SEE_INVALID_ARGUMENT;

    cls = SEE_MSG_PART_GET_CLASS(part);

    return cls->write_double(part, value, error_out);
}

int
see_msg_part_get_double(
    const SeeMsgPart* part,
    double*           value,
    SeeError**        error_out
    )
{
    const SeeMsgPartClass* cls;
    if (!part || !value)
        return SEE_INVALID_ARGUMENT;

    if (!error_out || *error_out)
        return SEE_INVALID_ARGUMENT;

    cls = SEE_MSG_PART_GET_CLASS(part);

    return cls->get_double(part, value, error_out);
}

int
see_msg_part_buffer_length(
    const SeeMsgPart* part,
    size_t*           value,
    SeeError**        error_out
    )
{
    const SeeMsgPartClass* cls;
    if (!part || !value)
        return SEE_INVALID_ARGUMENT;

    if (!error_out || *error_out)
        return SEE_INVALID_ARGUMENT;

    cls = SEE_MSG_PART_GET_CLASS(part);

    return cls->buffer_length(part, value, error_out);
}


int see_msg_part_write(
    const SeeMsgPart*   part,
    void *              buffer,
    SeeError**          error_out
    )
{
    if (!part || !buffer)
        return SEE_INVALID_ARGUMENT;

    if (!error_out || *error_out)
        return SEE_INVALID_ARGUMENT;

    const SeeMsgPartClass* cls = SEE_MSG_PART_GET_CLASS(part);

    return cls->write(part, buffer, error_out);
}

int see_msg_part_read(
    SeeMsgPart**    part_out,
    const void*     buffer,
    size_t          bufsiz,
    size_t*         size_out,
    SeeError**      error
    )
{
    if (!part_out || !buffer || !size_out) {
        assert(part_out && buffer && size_out);
        return SEE_INVALID_ARGUMENT;
    }

    if (!error || *error) {
        assert(error && !*error);
        return SEE_INVALID_ARGUMENT;
    }

    const SeeMsgPartClass* cls = see_msg_part_class();
    if (!cls)
        return SEE_NOT_INITIALIZED;

    return cls->read(part_out, buffer, bufsiz, size_out, error);
}


/* **** initialization of the class **** */

/**
 * \brief A pointer to the global SeeMsgPartClass.
 *
 * \private
 */
SeeMsgPartClass* g_SeeMsgPartClass = NULL;

static int see_msg_part_class_init(SeeObjectClass* new_cls)
{
    int ret = SEE_SUCCESS;

    /* Override the functions on the parent here */
    new_cls->init   = part_init;
    new_cls->name   = "SeeMsgPartClass";
    new_cls->destroy= msg_part_destroy;
    new_cls->equal  = msg_part_equal;
    new_cls->not_equal = msg_part_not_equal;

    /* Set the function pointers of the own class here */
    SeeMsgPartClass* cls = (SeeMsgPartClass*) new_cls;

    cls->msg_part_init  = msg_part_init;
    cls->length         = msg_part_length;

    cls->write_int32    = msg_part_write_int32;
    cls->get_int32      = msg_part_get_int32;
    cls->write_uint32   = msg_part_write_uint32;
    cls->get_uint32     = msg_part_get_uint32;

    cls->write_int64    = msg_part_write_int64;
    cls->get_int64      = msg_part_get_int64;
    cls->write_uint64   = msg_part_write_uint64;
    cls->get_uint64     = msg_part_get_uint64;

    cls->write_string   = msg_part_write_string;
    cls->get_string     = msg_part_get_string;

    cls->write_float    = msg_part_write_float;
    cls->get_float      = msg_part_get_float;
    cls->write_double   = msg_part_write_double;
    cls->get_double     = msg_part_get_double;

    cls->buffer_length  = msg_part_buffer_length;

    cls->write          = msg_part_write;
    cls->read           = msg_part_read;

    cls->value_type     = msg_part_value_type;

    return ret;
}

/**
 * \private
 * \brief this class initializes SeeMsgPart(Class).
 *
 * You might want to call this from the library initialization func.
 */
int
see_msg_part_init()
{
    int ret;
    const SeeMetaClass* meta = see_meta_class_class();

    ret = see_meta_class_new_class(
        meta,
        (SeeObjectClass**) &g_SeeMsgPartClass,
        sizeof(SeeMsgPartClass),
        sizeof(SeeMsgPart),
        SEE_OBJECT_CLASS(see_object_class()),
        sizeof(SeeObjectClass),
        see_msg_part_class_init
    );

    return ret;
}

void
see_msg_part_deinit()
{
    if(!g_SeeMsgPartClass)
        return;

    see_object_decref(SEE_OBJECT(g_SeeMsgPartClass));
    g_SeeMsgPartClass = NULL;
}

const SeeMsgPartClass*
see_msg_part_class()
{
    return g_SeeMsgPartClass;
}

/* ********************************************************************* */
/* **** functions that implement SeeMsgBuffer or override SeeObject **** */
/* ********************************************************************* */

static int
msg_buffer_init(
    SeeMsgBuffer*               msg_buffer,
    const SeeMsgBufferClass*    msg_buffer_cls,
    uint16_t                    id,
    SeeError**                  error_out
    )
{
    int ret = SEE_SUCCESS;
    const SeeObjectClass* parent_cls = SEE_OBJECT_GET_CLASS(
        msg_buffer
        );
        
    parent_cls->object_init(
        SEE_OBJECT(msg_buffer),
        SEE_OBJECT_CLASS(msg_buffer_cls)
        );
    
    ret = see_dynamic_array_new(
        &msg_buffer->parts,
        sizeof(SeeMsgPart*),
        see_copy_by_ref,
        see_init_memset,
        see_free_see_object,
        error_out
        );
    if (ret)
        return ret;

    msg_buffer->id = id;
    msg_buffer->length = strlen(msg_buffer_cls->msg_start) +
                         sizeof(msg_buffer->id)            +
                         sizeof(msg_buffer->length);

    return ret;
}

static int
init(const SeeObjectClass* cls, SeeObject* obj, va_list args)
{
    const SeeMsgBufferClass* msg_buffer_cls = SEE_MSG_BUFFER_CLASS(cls);
    SeeMsgBuffer* msg_buffer = SEE_MSG_BUFFER(obj);
    
    /*Extract parameters here from va_list args here.*/
    int id           = va_arg(args, int);
    SeeError** error = va_arg(args, SeeError**);

    // uint16_t are promoted to int but id should be in the range [0, 65535]
    assert(id >= 0 && id <= 65535);

    uint16_t short_id = (uint16_t) id;

    return msg_buffer_cls->msg_buffer_init(
        msg_buffer,
        msg_buffer_cls,
        short_id,
        error
        );
}

static void
msg_buffer_destroy(SeeObject* obj)
{
    const SeeObjectClass* cls = see_object_class();
    SeeMsgBuffer* msg = SEE_MSG_BUFFER(obj);

    see_object_decref(SEE_OBJECT(msg->parts));
    cls->destroy(obj);
}

static int
msg_buffer_equal(
    const SeeObject*    so_self,
    const SeeObject*    so_other,
    int*                result,
    SeeError**          error
    )
{
    const SeeMsgBuffer* self, *other;
    int ret, isinstance;
    size_t npart, npartother;

    if (so_self == so_other) {
        *result = true;
        return SEE_SUCCESS;
    }

#if !defined(NDEBUG)
    ret = see_object_is_instance_of(
        so_self,
        SEE_OBJECT_CLASS(see_msg_buffer_class()),
        &isinstance
        );
    assert(ret == SEE_SUCCESS && isinstance);
#endif

    ret = see_object_is_instance_of(
        so_other,
        SEE_OBJECT_CLASS(see_msg_buffer_class()),
        &isinstance
        );
    assert(ret == SEE_SUCCESS);
    if (!isinstance) {
        see_incomparable_error_create(
            error,
            see_object_get_class(so_self),
            see_object_get_class(so_other)
            );
        return SEE_ERROR_INCOMPARABLE;
    }

    self = (const SeeMsgBuffer*) so_self;
    other= (const SeeMsgBuffer*) so_other;

    if (self->id != other->id) {
        *result = false;
        return SEE_SUCCESS;
    }

    if (self->length != other->length) {
        *result = false;
        return SEE_SUCCESS;
    }

    ret = see_msg_buffer_num_parts(self, &npart);
    assert(ret == SEE_SUCCESS);
    ret = see_msg_buffer_num_parts(other,&npartother);
    assert(ret == SEE_SUCCESS);
    if (npart != npartother) {
        *result = true;
        return SEE_SUCCESS;
    }

    const SeeObject** self_parts = NULL;
    const SeeObject** other_parts= NULL;

    self_parts = see_dynamic_array_get(self->parts, 0, error);
    if (!self_parts)
        return ret;
    other_parts = see_dynamic_array_get(self->parts, 0, error);
    if (!other_parts)
        return ret;

    for (size_t i = 0; i < npart; i++) {
        int res;
        ret = see_object_equal(self_parts[i], other_parts[i], &res, error);
        if (ret != SEE_SUCCESS)
            return ret;
        if (!res) {
            *result = false;
            return SEE_SUCCESS;
        }
    }

    *result = true;
    return SEE_SUCCESS;
}

static int
msg_buffer_not_equal(
    const SeeObject*    so_self,
    const SeeObject*    so_other,
    int*                result,
    SeeError**          error
    )
{
    int ret, tres;
    ret = msg_buffer_equal(so_self, so_other, &tres, error);
    if (ret == SEE_SUCCESS)
        *result = !tres;
    return ret;
}

static int
msg_buffer_set_id(
    SeeMsgBuffer*       mbuf,
    uint16_t            id
    )
{
    mbuf->id = id;
    return SEE_SUCCESS;
}

static int
msg_buffer_get_id(
    const SeeMsgBuffer* mbuf,
    uint16_t*           id
    )
{
    *id = mbuf->id;
    return SEE_SUCCESS;
}

static int
msg_buffer_calc_length(
    SeeMsgBuffer*       msg,
    uint32_t*           length,
    SeeError**          error
    )
{
    size_t size = 0;
    size_t num_parts;
    uint32_t max = 0;
    max -= 1;
    const SeeMsgBufferClass* cls = SEE_MSG_BUFFER_GET_CLASS(msg);

    size += strlen(cls->msg_start);
    size += sizeof(msg->id);
    size += sizeof(uint32_t); // The total length of the message

    int ret = see_msg_buffer_num_parts(msg, &num_parts);
    assert(ret == SEE_SUCCESS);

    for (size_t i = 0; i < num_parts; ++i)
    {
        SeeMsgPart* part = NULL;
        size_t part_length;
        ret = cls->get_part(msg, i, &part, error);
        if (ret)
            return ret;

        ret = see_msg_part_buffer_length(part, &part_length, error);
        if (ret)
            return ret;

        see_object_decref(SEE_OBJECT(part));

        size += part_length;
    }

    if (size > max) {
        errno = EOVERFLOW;
        see_runtime_error_create(error, errno);
        return SEE_ERROR_RUNTIME;
    }

    *length = (uint32_t) size;
    return SEE_SUCCESS;
}

static int
msg_buffer_add_part(
    SeeMsgBuffer* mbuf,
    SeeMsgPart*   mpart,
    SeeError**    error_out
    )
{
    const SeeMsgBufferClass* cls = SEE_MSG_BUFFER_GET_CLASS(mbuf);

    int ret = see_dynamic_array_add(mbuf->parts, &mpart, error_out);
    size_t size;
    if (ret)
        return ret;

    ret = see_msg_part_buffer_length(mpart, &size, error_out);
    if (ret)
        return ret;

    mbuf->length += size;

#if !defined(NDEBUG)
    uint32_t length;
    ret = cls->calc_length(mbuf, &length, error_out);
    assert(length == mbuf->length);
#endif

    return ret;
}

static int
msg_buffer_get_part(
    SeeMsgBuffer*   mbuf,
    size_t          n,
    SeeMsgPart**    mbpart,
    SeeError**      error_out
    )
{
    SeeMsgPart* part = NULL;
    SeeMsgPart** part_array = NULL;
    part_array = see_dynamic_array_get(mbuf->parts, n, error_out);
    if (!part_array)
        return SEE_ERROR_INDEX;
    part = part_array[0];

    if (*mbpart)
        see_object_decref(SEE_OBJECT(*mbpart));

    see_object_ref(SEE_OBJECT(part));
    *mbpart = part;

    return SEE_SUCCESS;
}

static int
msg_buffer_num_parts(
    const SeeMsgBuffer* msgbuf,
    size_t*             size
    )
{
    *size = see_dynamic_array_size(msgbuf->parts);
    return SEE_SUCCESS;
}

static int
msg_buffer_get_buffer(
    SeeMsgBuffer*       msg,
    void**              buffer_out,
    size_t*             bufsize_out,
    SeeError**          error_out
    )
{
    int     ret;
    char*   bytes = NULL;

    uint32_t length, length_network;
    size_t n, nwritten = 0;
    const SeeMsgBufferClass* cls = SEE_MSG_BUFFER_GET_CLASS(msg);

    length = msg->length;
    length_network = see_host_to_network32(length);

    cls->num_parts(msg, &n);

    bytes = malloc(length);
    if (!bytes) {
        see_runtime_error_create(error_out, errno);
        return SEE_ERROR_RUNTIME;
    }

    // write the header.
    const char* header = cls->msg_start;
    size_t start_length = strlen(header);
    memcpy(&bytes[nwritten], header, start_length);
    nwritten += start_length;

    uint16_t id = see_host_to_network16(msg->id);
    memcpy(&bytes[nwritten], &id, sizeof(id));
    nwritten += sizeof(id);

    memcpy(&bytes[nwritten], &length_network, sizeof(length_network));
    nwritten += sizeof(length_network);

    if (n > 0) {
        const SeeMsgPart **partarray = see_dynamic_array_get(msg->parts, 0, error_out);
        for (size_t i = 0; i < n; ++i) {

            size_t partsize;
            const SeeMsgPart *part = partarray[i];

            ret = see_msg_part_write(part, &bytes[nwritten], error_out);
            if (ret)
                goto fail;

            ret = see_msg_part_buffer_length(part, &partsize, error_out);
            if (ret)
                goto fail;

            nwritten += partsize;
        }
    }

    assert(nwritten == length);

    *bufsize_out= nwritten;
    *buffer_out = bytes;
    return SEE_SUCCESS;

fail:

    free(bytes);
    return ret;
}

static int
msg_buffer_from_buffer(
    SeeMsgBuffer**  new_buf_out,
    const void*     buffer,
    size_t          bufsiz,
    SeeError**      error_out
    )
{
    uint16_t        id, id_network;
    uint32_t        length, length_network;
    size_t          nread = 0;
    int             ret;
    SeeMsgBuffer*   msg = NULL;
    const char*     bytes = buffer;
    const char*     msg_start = g_see_msg_start;
    size_t          start_length = strlen(msg_start);

    char start[4] = {0};

    if (bufsiz < (strlen(g_see_msg_start) + sizeof(id) + sizeof(length))) {
        see_msg_invalid_error_new(error_out);
        return SEE_ERROR_MSG_INVALID;
    }

    memcpy(&start[0], &bytes[nread], start_length);
    if (memcmp(msg_start, start, start_length) != 0) {
        see_msg_invalid_error_new(error_out);
        return SEE_ERROR_MSG_INVALID;
    }
    nread += start_length;

    memcpy(&id_network, &bytes[nread], sizeof(id_network));
    id = see_network_to_host16(id_network);
    nread += sizeof(id_network);

    memcpy(&length_network, &bytes[nread], sizeof(length_network));
    length = see_network_to_host32(length_network);
    nread += sizeof(length);

    if (length > bufsiz) {
        see_msg_invalid_error_new(error_out);
        return SEE_ERROR_MSG_INVALID;
    }

    ret = see_msg_buffer_new(&msg, id, error_out);
    if (ret)
        return ret;

    while(nread < length) {

        SeeMsgPart* part        = NULL;
        size_t      partsize    = 0;

        ret = see_msg_part_read(
            &part,
            &bytes[nread],
            length - nread,
            &partsize,
            error_out
            );
        if (ret)
            goto fail;

        nread += partsize;

        ret = see_msg_buffer_add_part(msg, part, error_out);
        if (ret)
            goto fail;
        see_object_decref(SEE_OBJECT(part));
    }

    *new_buf_out = msg;

    return ret;

fail:

    see_object_decref(SEE_OBJECT(msg));
    return ret;
}

/* **** implementation of the public API **** */

int
see_msg_buffer_new(SeeMsgBuffer** buf, uint16_t id, SeeError** error_out)
{
    const SeeObjectClass* cls = SEE_OBJECT_CLASS(see_msg_buffer_class());

    // uint16_t is promoted to int when used in a function taking variable
    // length aruguments (in "...") so lets promote it more explicitly.
    int iid = id;

    if (!cls)
        return SEE_NOT_INITIALIZED;

    if (!buf || *buf || !error_out || *error_out)
        return SEE_INVALID_ARGUMENT;

    return cls->new_obj(cls, 0, SEE_OBJECT_REF(buf), iid, error_out);
}

int
see_msg_buffer_set_id(
    SeeMsgBuffer*   msg,
    uint16_t        id
    )
{
    if (!msg)
        return SEE_INVALID_ARGUMENT;

    const SeeMsgBufferClass* cls = SEE_MSG_BUFFER_GET_CLASS(msg);

    return cls->set_id(msg, id);
}

int
see_msg_buffer_get_id(
    const SeeMsgBuffer* msg,
    uint16_t*           id
    )
{
    if (!msg || !id)
        return SEE_INVALID_ARGUMENT;

    const SeeMsgBufferClass* cls = SEE_MSG_BUFFER_GET_CLASS(msg);

    return cls->get_id(msg, id);
}

int
see_msg_buffer_length(
    const SeeMsgBuffer* buffer,
    size_t*             length
    )
{
    if (!buffer || !length)
        return SEE_INVALID_ARGUMENT;

    *length = buffer->length;

    return SEE_SUCCESS;
}

int
see_msg_buffer_add_part(
    SeeMsgBuffer*   msg,
    SeeMsgPart*     part,
    SeeError**      error_out
    )
{
    const SeeMsgBufferClass* cls;
    if (!msg || !part)
        return SEE_INVALID_ARGUMENT;
    if(!error_out || *error_out)
        return SEE_INVALID_ARGUMENT;

    cls = SEE_MSG_BUFFER_GET_CLASS(msg);

    return cls->add_part(msg, part, error_out);
}

int
see_msg_buffer_get_part(
    SeeMsgBuffer*   msg,
    size_t          index,
    SeeMsgPart**    part,
    SeeError**      error
    )
{
    if (!msg || !part)
        return SEE_INVALID_ARGUMENT;
    if (!error || *error)
        return SEE_INVALID_ARGUMENT;

    const SeeMsgBufferClass* cls = SEE_MSG_BUFFER_GET_CLASS(msg);
    return cls->get_part(msg, index, part, error);
}

int
see_msg_buffer_num_parts(
    const SeeMsgBuffer* msg,
    size_t*             size_out
    )
{
    if (!msg || !size_out)
        return SEE_INVALID_ARGUMENT;

    const SeeMsgBufferClass* cls = SEE_MSG_BUFFER_GET_CLASS(msg);

    return cls->num_parts(msg, size_out);
}

int
see_msg_buffer_get_buffer(
    SeeMsgBuffer*   msg,
    void**          buf_out,
    size_t*         size_out,
    SeeError**      error_out
    )
{
    if (!msg || !buf_out || !size_out)
        return SEE_INVALID_ARGUMENT;

    if (!error_out || *error_out)
        return SEE_INVALID_ARGUMENT;

    const SeeMsgBufferClass* cls = SEE_MSG_BUFFER_GET_CLASS(msg);

    return cls->get_buffer(msg, buf_out, size_out, error_out);
}

int
see_msg_buffer_from_buffer(
    SeeMsgBuffer**  msg_out,
    const void*     buffer,
    size_t          size,
    SeeError**      error_out
    )
{
    if (!msg_out || !buffer)
        return SEE_INVALID_ARGUMENT;

    if (!error_out || *error_out)
        return SEE_INVALID_ARGUMENT;

    const SeeMsgBufferClass* cls = see_msg_buffer_class();

    return cls->from_buffer(msg_out, buffer, size, error_out);
}


/* **** initialization of the class **** */

/**
 * \brief A pointer to the global SeeMsgBufferClass.
 * \private.
 */
SeeMsgBufferClass* g_SeeMsgBufferClass = NULL;

static int see_msg_buffer_class_init(SeeObjectClass* new_cls)
{
    int ret = SEE_SUCCESS;
    
    /* Override the functions on the parent here */
    new_cls->init       = init;
    new_cls->name       = "SeeMsgBufferClass";
    new_cls->destroy    = msg_buffer_destroy;
    new_cls->equal      = msg_buffer_equal;
    new_cls->not_equal  = msg_buffer_not_equal;
    
    /* Set the function pointers of the own class here */
    SeeMsgBufferClass* cls  = (SeeMsgBufferClass*) new_cls;

    cls->msg_start          = g_see_msg_start;

    cls->msg_buffer_init    = msg_buffer_init;
    cls->set_id             = msg_buffer_set_id;
    cls->get_id             = msg_buffer_get_id;
    cls->calc_length        = msg_buffer_calc_length;
    cls->add_part           = msg_buffer_add_part;
    cls->get_part           = msg_buffer_get_part;
    cls->num_parts          = msg_buffer_num_parts;
    cls->get_buffer         = msg_buffer_get_buffer;
    cls->from_buffer        = msg_buffer_from_buffer;

    return ret;
}

/**
 * \private
 * \brief this class initializes SeeMsgBuffer(Class).
 *
 * You might want to call this from the library initialization func.
 */
int
see_msg_buffer_init()
{
    int ret;
    const SeeMetaClass* meta = see_meta_class_class();

    ret = see_meta_class_new_class(
        meta,
        (SeeObjectClass**) &g_SeeMsgBufferClass,
        sizeof(SeeMsgBufferClass),
        sizeof(SeeMsgBuffer),
        SEE_OBJECT_CLASS(see_object_class()),
        sizeof(SeeObjectClass),
        see_msg_buffer_class_init
        );

    return ret;
}

void
see_msg_buffer_deinit()
{
    if(!g_SeeMsgBufferClass)
        return;

    see_object_decref(SEE_OBJECT(g_SeeMsgBufferClass));
    g_SeeMsgBufferClass = NULL;
}

const SeeMsgBufferClass*
see_msg_buffer_class()
{
    return g_SeeMsgBufferClass;
}

/* ********************************* */
/* **** Message Part Type Error **** */
/* ********************************* */

static const char*
msg_part_value_type_to_string(see_msg_part_value_t value)
{
    switch(value) {
        case SEE_MSG_PART_NOT_INIT:
            return "Uninitialized";
        case SEE_MSG_PART_INT32_T:
            return "int32_t";
        case SEE_MSG_PART_INT64_T:
            return "int64_t";
        case SEE_MSG_PART_UINT32_T:
            return "uint32_t";
        case SEE_MSG_PART_UINT64_T:
            return "uint64_t";
        case SEE_MSG_PART_STRING_T:
            return "string";
        case SEE_MSG_PART_FLOAT_T:
            return "float";
        case SEE_MSG_PART_DOUBLE_T:
            return "double";
        default:
            assert(0 == 1);
            return NULL;
    }
}

static int
msg_part_type_error_init(
    SeeMsgPartTypeError*            msg_part_type_error,
    const SeeMsgPartTypeErrorClass* msg_part_type_error_cls,
    see_msg_part_value_t            expected,
    see_msg_part_value_t            asked
    )
{
    int ret = SEE_SUCCESS;
    const SeeErrorClass* parent_cls = SEE_ERROR_GET_CLASS(
        msg_part_type_error
        );

    char msg[1024];

    snprintf(msg, sizeof(msg), "MessagePart is a %s, but it is used as an %s",
        msg_part_value_type_to_string(expected),
        msg_part_value_type_to_string(asked)
        );

    parent_cls->error_init(
        SEE_ERROR(msg_part_type_error),
        SEE_ERROR_CLASS(msg_part_type_error_cls),
        msg
        );

    return ret;
}

static int
type_error_init(const SeeObjectClass* cls, SeeObject* obj, va_list args)
{
    const SeeMsgPartTypeErrorClass* msg_part_type_error_cls = SEE_MSG_PART_TYPE_ERROR_CLASS(cls);
    SeeMsgPartTypeError* msg_part_type_error = SEE_MSG_PART_TYPE_ERROR(obj);

    /*Extract parameters here from va_list args here.*/

    see_msg_part_value_t expected = va_arg(args, see_msg_part_value_t);
    see_msg_part_value_t asked    = va_arg(args, see_msg_part_value_t);

    return msg_part_type_error_cls->msg_part_type_error_init(
        msg_part_type_error,
        msg_part_type_error_cls,
        expected,
        asked
        );
}

/* **** implementation of the public API **** */

int
see_msg_part_type_error_new(
    SeeError**              error,
    see_msg_part_value_t    expected,
    see_msg_part_value_t    asked
    )
{
    const SeeObjectClass* cls = SEE_OBJECT_CLASS(
        see_msg_part_type_error_class()
        );

    if (!cls) {
        assert(cls != NULL);
        return SEE_NOT_INITIALIZED;
    }

    if (!error || *error)
        return SEE_INVALID_ARGUMENT;

    return cls->new_obj(cls, 0, SEE_OBJECT_REF(error), expected, asked);
}

/* **** initialization of the class **** */

/**
 * \brief A pointer to the SeeMsgPartTypeErrorClass.
 * \private
 */
SeeMsgPartTypeErrorClass* g_SeeMsgPartTypeErrorClass = NULL;

static int see_msg_part_type_error_class_init(SeeObjectClass* new_cls)
{
    int ret = SEE_SUCCESS;

    /* Override the functions on the parent here */
    new_cls->init = type_error_init;
    new_cls->name = "SeeMsgPartTypeErrorClass";

    /* Set the function pointers of the own class here */
    SeeMsgPartTypeErrorClass* cls = (SeeMsgPartTypeErrorClass*) new_cls;
    cls->msg_part_type_error_init = msg_part_type_error_init;

    return ret;
}

/**
 * \private
 * \brief this class initializes SeeMsgPartTypeError(Class).
 *
 * You might want to call this from the library initialization func.
 */
int
see_msg_part_type_error_init()
{
    int ret;
    const SeeMetaClass* meta = see_meta_class_class();

    ret = see_meta_class_new_class(
        meta,
        (SeeObjectClass**) &g_SeeMsgPartTypeErrorClass,
        sizeof(SeeMsgPartTypeErrorClass),
        sizeof(SeeMsgPartTypeError),
        SEE_OBJECT_CLASS(see_error_class()),
        sizeof(SeeErrorClass),
        see_msg_part_type_error_class_init
        );

    return ret;
}

void
see_msg_part_type_error_deinit()
{
    if(!g_SeeMsgPartTypeErrorClass)
        return;

    see_object_decref(SEE_OBJECT(g_SeeMsgPartTypeErrorClass));
    g_SeeMsgPartTypeErrorClass = NULL;
}

const SeeMsgPartTypeErrorClass*
see_msg_part_type_error_class()
{
    return g_SeeMsgPartTypeErrorClass;
}

/* ********************************* */
/* **** Message Invalid Error **** */
/* ********************************* */

static int
msg_invalid_error_init(
    SeeMsgInvalidError*            msg_invalid_error,
    const SeeMsgInvalidErrorClass* msg_invalid_error_cls
    )
{
    int ret = SEE_SUCCESS;
    const SeeErrorClass* parent_cls = SEE_ERROR_GET_CLASS(
        msg_invalid_error
        );

    parent_cls->error_init(
        SEE_ERROR(msg_invalid_error),
        SEE_ERROR_CLASS(msg_invalid_error_cls),
        "Invalid Message"
        );

    return ret;
}

static int
invalid_error_init(const SeeObjectClass* cls, SeeObject* obj, va_list args)
{
    const SeeMsgInvalidErrorClass* msg_invalid_error_cls = SEE_MSG_INVALID_ERROR_CLASS(cls);
    SeeMsgInvalidError* msg_invalid_error = SEE_MSG_INVALID_ERROR(obj);

    /*Extract parameters here from va_list args here.*/

    /*Currently none extra parameters needed*/
    (void) args;

    return msg_invalid_error_cls->msg_invalid_error_init(
        msg_invalid_error,
        msg_invalid_error_cls
        );
}

/* **** implementation of the public API **** */
int
see_msg_invalid_error_new(SeeError** error)
{
    const SeeObjectClass* cls = SEE_OBJECT_CLASS(
        see_msg_invalid_error_class()
        );

    if (!cls) {
        assert(cls != NULL);
        return SEE_NOT_INITIALIZED;
    }

    if (!error || *error)
        return SEE_INVALID_ARGUMENT;

    return cls->new_obj(cls, 0, SEE_OBJECT_REF(error));
}

/* **** initialization of the class **** */

/**
 * \brief   A pointer to the SeeMsgInvalidErrorClass
 * \private
 */
SeeMsgInvalidErrorClass* g_SeeMsgInvalidErrorClass = NULL;

static int see_msg_invalid_error_class_init(SeeObjectClass* new_cls)
{
    int ret = SEE_SUCCESS;

    /* Override the functions on the parent here */
    new_cls->init = invalid_error_init;
    new_cls->name = "SeeMsgInvalidErrorClass";

    /* Set the function pointers of the own class here */
    SeeMsgInvalidErrorClass* cls = (SeeMsgInvalidErrorClass*) new_cls;
    cls->msg_invalid_error_init = msg_invalid_error_init;

    return ret;
}

/**
 * \private
 * \brief this class initializes SeeMsgInvalidError(Class).
 *
 * You might want to call this from the library initialization func.
 */
int
see_msg_invalid_error_init()
{
    int ret;
    const SeeMetaClass* meta = see_meta_class_class();

    ret = see_meta_class_new_class(
        meta,
        (SeeObjectClass**) &g_SeeMsgInvalidErrorClass,
        sizeof(SeeMsgInvalidErrorClass),
        sizeof(SeeMsgInvalidError),
        SEE_OBJECT_CLASS(see_error_class()),
        sizeof(SeeErrorClass),
        see_msg_invalid_error_class_init
    );

    return ret;
}

void
see_msg_invalid_error_deinit()
{
    if(!g_SeeMsgInvalidErrorClass)
        return;

    see_object_decref(SEE_OBJECT(g_SeeMsgInvalidErrorClass));
    g_SeeMsgInvalidErrorClass = NULL;
}

const SeeMsgInvalidErrorClass*
see_msg_invalid_error_class()
{
    return g_SeeMsgInvalidErrorClass;
}

