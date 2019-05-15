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
 * floating point numbers encoded as a string, hence endianness doesn't
 * apply to the floats, but they are more expensive to send/receive.
 *
 * \private
 */

#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>

#include "MetaClass.h"
#include "MsgBuffer.h"
#include "see_object_config.h"
#include "utilities.h"
#include "RuntimeError.h"

///**
// * \brief Returns the length of the header of one SeeMsgPart
// * @return The length of the header
// * \private
// */
//static size_t
//msg_part_header_length()
//{
//    // sizeof(SeeMsgPart.value_type) + sizeof(SeeMsgPart.length);
//    return sizeof(uint16_t) + sizeof(uint16_t);
//}

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

static int
msg_part_write_int32(
    SeeMsgPart*   part,
    int32_t             value,
    SeeError**          error_out
    )
{
    (void) error_out;
    msg_part_destroy_content(part);

    part->value_type        = SEE_MSG_PART_INT32_T;
    part->value.int32_val   = see_host_to_network32(value);

    part->length = sizeof(part->value_type) + sizeof(part->value.int32_val);

    return SEE_SUCCESS;
}

static int
msg_part_get_int32 (
    const SeeMsgPart* part,
    int32_t*                value,
    SeeError**              error_out
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

    int32_t host_val = see_network_to_host32(part->value.int32_val);
    *value = host_val;

    return SEE_SUCCESS;
}

static int
msg_part_write_uint32(
    SeeMsgPart*   part,
    uint32_t            value,
    SeeError**          error_out
    )
{
    (void) error_out;

    msg_part_destroy_content(part);

    part->value_type        = SEE_MSG_PART_UINT32_T;
    part->value.uint32_val  = see_host_to_network32(value);

    part->length = sizeof(part->value_type) + sizeof(part->value.uint32_val);

    return SEE_SUCCESS;
}

static int
msg_part_get_uint32 (
    const SeeMsgPart* part,
    uint32_t*               value,
    SeeError**              error_out
    )
{
    if (part->value_type != SEE_MSG_PART_UINT32_T) {
        see_msg_part_type_error_new(
            error_out,
            SEE_MSG_PART_UINT32_T,
            part->value_type
        );
        return SEE_ERROR_MSG_PART_TYPE;
    }

    uint32_t host_val = see_network_to_host32(part->value.uint32_val);
    *value = host_val;

    return SEE_SUCCESS;
}

static int
msg_part_write_int64(
    SeeMsgPart*   part,
    int64_t             value,
    SeeError**          error_out
    )
{
    (void) error_out;

    msg_part_destroy_content(part);

    part->value_type        = SEE_MSG_PART_INT64_T;
    part->value.int64_val   = see_host_to_network64(value);

    part->length = sizeof(part->value_type) + sizeof(part->value.int64_val);

    return SEE_SUCCESS;
}

static int
msg_part_get_int64 (
    const SeeMsgPart* part,
    int64_t*                value,
    SeeError**              error_out
    )
{
    if (part->value_type != SEE_MSG_PART_INT64_T) {
        see_msg_part_type_error_new(
            error_out,
            SEE_MSG_PART_INT64_T,
            part->value_type
        );
        return SEE_ERROR_MSG_PART_TYPE;
    }

    int64_t host_val = see_network_to_host64(part->value.int64_val);
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
    part->value.uint64_val  = see_host_to_network64(value);

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
            SEE_MSG_PART_UINT64_T,
            part->value_type
            );
        return SEE_ERROR_MSG_PART_TYPE;
    }

    int64_t host_val = see_network_to_host64(part->value.int64_val);
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

    char* duplicate = malloc(length);
    if (!duplicate) {
        see_runtime_error_create(error_out, errno);
        return SEE_ERROR_RUNTIME;
    }

    msg_part_destroy_content(part);

    memcpy(duplicate, value, length);

    part->length        = sizeof(part->value_type) + sizeof(part->length) + length;
    part->value_type    = SEE_MSG_PART_STRING_T;
    part->value.str_val = duplicate;

    return SEE_SUCCESS;
}

static int
msg_part_retrieve_string(
    const SeeMsgPart* part,
    char**            value_out,
    SeeError**        error_out
    )
{
    size_t length = part->length - (sizeof(uint8_t)  + sizeof(uint32_t));
    char*out = malloc(length + 1);
    if (!out) {
        see_runtime_error_create(error_out, errno);
        return SEE_ERROR_RUNTIME;
    }
    memcpy(out, part->value.str_val, length);
    out[length] = 0; //terminate the string.
    *value_out = out;

    return SEE_SUCCESS;
}

static int
msg_part_get_string(
    const SeeMsgPart* part,
    char**            value_out,
    SeeError**        error_out
    )
{
    // floats are also encoded as string
    if (part->value_type != SEE_MSG_PART_STRING_T) {
        see_msg_part_type_error_new(
            error_out,
            SEE_MSG_PART_STRING_T,
            part->value_type
        );
        return SEE_ERROR_MSG_PART_TYPE;
    }

    return msg_part_retrieve_string(part, value_out, error_out);
}

static int
msg_part_write_float(
    SeeMsgPart* part,
    float       value,
    SeeError**  error_out
    )
{
    (void) error_out;
    assert(sizeof(float) == sizeof(uint32_t));
    part->length            = sizeof(part->value_type) + sizeof(float);
    part->value.float_val   = value;
    part->value.uint32_val  = see_swap_endianess32(part->value.uint32_val);
    part->value_type        = SEE_MSG_PART_FLOAT_T;
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
            SEE_MSG_PART_STRING_T,
            part->value_type
        );
        return SEE_ERROR_MSG_PART_TYPE;
    }

    union {
        float       flt_val;
        uint32_t    int_val;
    } temp;

    temp.int_val = see_swap_endianess32(part->value.uint32_val);

    *value_out = temp.flt_val;

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
    assert(sizeof (double) == sizeof(uint64_t));
    part->length            = sizeof(part->value_type) + sizeof(double);
    part->value.double_val  = value;
    part->value.uint64_val  = see_swap_endianess64(part->value.uint64_val);
    part->value_type        = SEE_MSG_PART_DOUBLE_T;
    return SEE_SUCCESS;
}

static int
msg_part_get_double(
    const SeeMsgPart*   part,
    double*              value_out,
    SeeError**          error_out
    )
{
    if (part->value_type != SEE_MSG_PART_DOUBLE_T) {
        see_msg_part_type_error_new(
            error_out,
            SEE_MSG_PART_STRING_T,
            part->value_type
        );
        return SEE_ERROR_MSG_PART_TYPE;
    }

    union {
        double      flt_val;
        uint64_t    int_val;
    } temp;

    temp.int_val = see_swap_endianess64(part->value.uint64_val);

    *value_out = temp.flt_val;

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
    char*   bytes    = buffer;
    size_t  nwritten = 0;

    switch (part->value_type) {
        case SEE_MSG_PART_INT32_T:
            bytes[nwritten] = SEE_MSG_PART_INT32_T;
            nwritten += sizeof(part->value_type);
            memcpy(&bytes[nwritten], &part->value_type, sizeof(int32_t));
            nwritten += sizeof(int32_t);
            break;
        case SEE_MSG_PART_INT64_T:
            bytes[nwritten] = SEE_MSG_PART_INT64_T;
            nwritten += sizeof(part->value_type);
            memcpy(&bytes[nwritten], &part->value_type, sizeof(int32_t));
            nwritten += sizeof(int64_t);
            break;
        case SEE_MSG_PART_UINT32_T:
            bytes[nwritten] = SEE_MSG_PART_UINT32_T;
            nwritten += sizeof(part->value_type);
            memcpy(&bytes[nwritten], &part->value_type, sizeof(int32_t));
            nwritten += sizeof(uint32_t);
            break;
        case SEE_MSG_PART_UINT64_T:
            bytes[nwritten] = SEE_MSG_PART_UINT64_T;
            nwritten += sizeof(part->value_type);
            memcpy(&bytes[nwritten], &part->value_type, sizeof(int32_t));
            nwritten += sizeof(uint64_t);
            break;
        case SEE_MSG_PART_STRING_T:
            bytes[nwritten] = SEE_MSG_PART_STRING_T;
            nwritten += sizeof(part->value_type);
            memcpy(&bytes[nwritten], &part->length, sizeof(part->length));
            nwritten += sizeof(part->length);
            size_t header_len =sizeof(part->value_type) + sizeof(part->length);
            memcpy(
                &bytes[nwritten],
                part->value.str_val,
                part->length - header_len
                );
            nwritten += part->length - header_len;
            break;
        case SEE_MSG_PART_FLOAT_T:
            bytes[nwritten] = sizeof(SEE_MSG_PART_FLOAT_T);
            nwritten += sizeof(part->value_type);
            memcpy(&bytes[nwritten], &part->value_type, sizeof(int32_t));
            nwritten += sizeof(float);
            break;
        case SEE_MSG_PART_DOUBLE_T:
            bytes[nwritten] = sizeof(SEE_MSG_PART_DOUBLE_T);
            nwritten += sizeof(part->value_type);
            memcpy(&bytes[nwritten], &part->value_type, sizeof(int32_t));
            nwritten += sizeof(double);
            break;
        default:
            errno = EINVAL;
            see_runtime_error_create(error_out, errno);
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

    ret = see_msg_part_new(&new_part, error_out);
    if (ret)
        return ret;

    if (nread + 1 > bufsiz) {
        ret = see_msg_invalid_error_new(error_out);
        goto fail;
    }

    memcpy(&type, &bytes[0], sizeof(type));
    nread += sizeof(type);

    switch (type) {
        case SEE_MSG_PART_INT32_T:
            length = sizeof(new_part->value.int32_val) + sizeof(new_part->value_type);
            if (length > bufsiz) {
                ret = SEE_ERROR_MSG_INVALID;
                see_msg_invalid_error_new(error_out);
                goto fail;
            }

            memcpy(&new_part->value.int32_val,
                &bytes[nread],
                sizeof(new_part->value.int32_val)
                );
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

            memcpy(&new_part->value.int64_val,
                   &bytes[nread],
                   sizeof(new_part->value.int64_val)
                    );
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

            memcpy(&new_part->value.uint32_val,
                   &bytes[nread],
                   sizeof(new_part->value.uint32_val)
                   );
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

            memcpy(&new_part->value.uint64_val,
                   &bytes[nread],
                   sizeof(new_part->value.uint64_val)
                   );
            nread += sizeof(new_part->value.int64_val);
            new_part->length = length;
            break;
        case SEE_MSG_PART_STRING_T:
            memcpy(&length, &bytes[nread], sizeof(new_part->length));
            nread += length;
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

            break;
        case SEE_MSG_PART_FLOAT_T:
            length = sizeof(new_part->value.float_val) + sizeof(new_part->value_type);
            if (length > bufsiz) {
                ret = SEE_ERROR_MSG_INVALID;
                see_msg_invalid_error_new(error_out);
                goto fail;
            }

            memcpy(&new_part->value.float_val,
                   &bytes[nread],
                   sizeof(new_part->value.float_val)
            );
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

            memcpy(&new_part->value.double_val,
                   &bytes[nread],
                   sizeof(new_part->value.double_val)
                   );
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
    if (!part || !value)
        return SEE_INVALID_ARGUMENT;

    if (!error_out || *error_out)
        return SEE_INVALID_ARGUMENT;

    cls = SEE_MSG_PART_GET_CLASS(part);

    return cls->write_int32(part, value, error_out);
}

int
see_msg_part_get_int32(
    const SeeMsgPart* part,
    int32_t*                value,
    SeeError**              error_out
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
    uint32_t                value,
    SeeError**              error_out
    )
{
    const SeeMsgPartClass* cls;
    if (!part || !value)
        return SEE_INVALID_ARGUMENT;

    if (!error_out || *error_out)
        return SEE_INVALID_ARGUMENT;

    cls = SEE_MSG_PART_GET_CLASS(part);

    return cls->write_uint32(part, value, error_out);
}

int
see_msg_part_get_uint32(
    const SeeMsgPart* part,
    uint32_t*               value,
    SeeError**              error_out
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
    int64_t                 value,
    SeeError**              error_out
)
{
    const SeeMsgPartClass* cls;
    if (!part || !value)
        return SEE_INVALID_ARGUMENT;

    if (!error_out || *error_out)
        return SEE_INVALID_ARGUMENT;

    cls = SEE_MSG_PART_GET_CLASS(part);

    return cls->write_int64(part, value, error_out);
}

int
see_msg_part_get_int64(
    const SeeMsgPart* part,
    int64_t*                value,
    SeeError**              error_out
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
    uint64_t                value,
    SeeError**              error_out
)
{
    const SeeMsgPartClass* cls;
    if (!part || !value)
        return SEE_INVALID_ARGUMENT;

    if (!error_out || *error_out)
        return SEE_INVALID_ARGUMENT;

    cls = SEE_MSG_PART_GET_CLASS(part);

    return cls->write_uint64(part, value, error_out);
}

int
see_msg_part_get_uint64(
    const SeeMsgPart* part,
    uint64_t*               value,
    SeeError**              error_out
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
    const char*             value,
    size_t                  length,
    SeeError**              error_out
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
    if (!part || !value)
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
    if (!part || !value)
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
    uint32_t *        value,
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

SeeMsgPartClass* g_SeeMsgPartClass = NULL;

static int see_msg_part_class_init(SeeObjectClass* new_cls)
{
    int ret = SEE_SUCCESS;

    /* Override the functions on the parent here */
    new_cls->init   = part_init;
    new_cls->name   = "SeeMsgPartClass";
    new_cls->destroy= msg_part_destroy;

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
msg_buffer_add_part(
    SeeMsgBuffer* mbuf,
    SeeMsgPart*   mpart,
    SeeError**    error_out
    )
{
    return see_dynamic_array_add(mbuf->parts, &mpart, error_out);
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
    part = see_dynamic_array_get(mbuf->parts, n, error_out);

    if (!part)
        return SEE_ERROR_INDEX;

    if (*mbpart)
        see_object_decref(SEE_OBJECT(mbpart));

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
    const SeeMsgBuffer* msg,
    void**              buffer_out,
    size_t*             bufsize_out,
    SeeError**          error_out
    )
{
    int     ret;
    char*   bytes = NULL;

    uint32_t length;
    size_t n, nwritten = 0;
    const SeeMsgBufferClass* cls = SEE_MSG_BUFFER_GET_CLASS(msg);

    ret = cls->length(msg, &length);
    if (ret)
        return ret;

    cls->num_parts(msg, &n);

    bytes = malloc(length);
    if (!bytes) {
        see_runtime_error_create(error_out, errno);
        return SEE_ERROR_RUNTIME;
    }

    // write the header.
    uint16_t id = msg->id;
    memcpy(&bytes[0], &msg->id, sizeof(id));
    nwritten += sizeof(id);
    memcpy(&bytes[sizeof(id)], &length, sizeof(length));
    nwritten += sizeof(length);

    for (size_t i = 0; i < n; ++i) {

        size_t partsize;
        const SeeMsgPart** partptr = see_dynamic_array_get(msg->parts, n, error_out);

        if (!partptr) {
            ret = SEE_ERROR_INDEX;
            goto fail;
        }

        ret = see_msg_part_write(*partptr, &bytes[nwritten], error_out);
        if (ret)
            goto fail;

        ret = see_msg_part_buffer_length(*partptr, &partsize, error_out);
        if (ret)
            goto fail;

        ret = see_msg_part_length(*partptr, &length, error_out);
        if (ret)
            goto fail;

        nwritten += partsize;
    }

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
    uint16_t        id;
    uint32_t        length;
    size_t          nread = 0;
    int             ret;
    SeeMsgBuffer*   msg = NULL;
    const char* bytes = buffer;
    if (bufsiz < (sizeof(id) + sizeof(length))) {
        errno = EINVAL;
        see_runtime_error_create(error_out, errno);
        return SEE_ERROR_RUNTIME;
    }

    memcpy(&id, &bytes[0], sizeof(id));
    nread += sizeof(id);
    memcpy(&length, &bytes[nread], sizeof(length));
    nread += sizeof(length);

    if (length >= bufsiz) {
        errno = EINVAL;
        see_runtime_error_create(error_out, errno);
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
    }

    *new_buf_out = msg;

    return ret;

fail:

    see_object_decref(SEE_OBJECT(msg));
    return ret;
}

/* **** implementation of the public API **** */

int see_msg_buffer_new(SeeMsgBuffer** buf, uint16_t id, SeeError** error_out)
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
see_msg_buffer_add_part(
    SeeMsgBuffer*   msg,
    SeeMsgPart*     part,
    SeeError**      error_out
    )
{
    const SeeMsgBufferClass* cls;
    if (!msg || !part)
        return SEE_INVALID_ARGUMENT;
    if(!error_out || !error_out)
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
    const SeeMsgBuffer* msg,
    void**              buf_out,
    size_t*             size_out,
    SeeError**          error_out
    )
{
    if (!msg || !buf_out || size_out)
        return SEE_INVALID_ARGUMENT;

    if (!error_out || *error_out)
        return SEE_INVALID_ARGUMENT;

    const SeeMsgBufferClass* cls = SEE_MSG_BUFFER_GET_CLASS(msg);

    return cls->get_buffer(msg, buf_out, size_out, error_out);
}

int
see_msg_from_buffer(
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

SeeMsgBufferClass* g_SeeMsgBufferClass = NULL;

static int see_msg_buffer_class_init(SeeObjectClass* new_cls)
{
    int ret = SEE_SUCCESS;
    
    /* Override the functions on the parent here */
    new_cls->init       = init;
    new_cls->name       = "SeeMsgBufferClass";
    new_cls->destroy    = msg_buffer_destroy;
    
    /* Set the function pointers of the own class here */
    SeeMsgBufferClass* cls  = (SeeMsgBufferClass*) new_cls;
    cls->msg_buffer_init    = msg_buffer_init;
    cls->set_id             = msg_buffer_set_id;
    cls->get_id             = msg_buffer_get_id;
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

