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

    part->length = sizeof(int32_t) + sizeof(uint8_t);

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
            SEE_MSG_PART_TYPE_ERROR_REF(error_out),
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

    part->length = sizeof(uint32_t) + sizeof(uint8_t);

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
            SEE_MSG_PART_TYPE_ERROR_REF(error_out),
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

    part->length = sizeof(uint8_t) + sizeof(int64_t);

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
            SEE_MSG_PART_TYPE_ERROR_REF(error_out),
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

    part->length = sizeof(uint8_t) + sizeof(uint64_t);

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
            SEE_MSG_PART_TYPE_ERROR_REF(error_out),
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

    part->length        = sizeof(uint8_t) + sizeof(uint32_t) + length;
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
            SEE_MSG_PART_TYPE_ERROR_REF(error_out),
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
            SEE_MSG_PART_TYPE_ERROR_REF(error_out),
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
    part->value.double_val   = value;
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
            SEE_MSG_PART_TYPE_ERROR_REF(error_out),
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
    uint16_t*               length,
    SeeError**              error_out
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
    uint16_t*               val_type
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
    size_t*                 value,
    SeeError**              error_out
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

/* **** initialization of the class **** */

SeeMsgPartClass* g_SeeMsgPartClass = NULL;

static int see_msg_part_class_init(SeeObjectClass* new_cls)
{
    int ret = SEE_SUCCESS;

    /* Override the functions on the parent here */
    new_cls->init   = part_init;
    new_cls->name   = "SeeMsgPart";
    new_cls->destroy= msg_part_destroy;

    /* Set the function pointers of the own class here */
    SeeMsgPartClass* cls = (SeeMsgPartClass*) new_cls;
    cls->msg_part_init   = msg_part_init;

    cls->write_int32            = msg_part_write_int32;
    cls->get_int32              = msg_part_get_int32;
    cls->write_uint32           = msg_part_write_uint32;
    cls->get_uint32             = msg_part_get_uint32;

    cls->write_int64            = msg_part_write_int64;
    cls->get_int64              = msg_part_get_int64;
    cls->write_uint64           = msg_part_write_uint64;
    cls->get_uint64             = msg_part_get_uint64;

    cls->write_string           = msg_part_write_string;
    cls->get_string             = msg_part_get_string;

    cls->write_float            = msg_part_write_float;
    cls->get_float              = msg_part_get_float;
    cls->write_double           = msg_part_write_double;
    cls->get_double             = msg_part_get_double;

    cls->buffer_length          = msg_part_buffer_length;

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

/**
 * \brief Destroys the buffer kept in a msg and marks it as destroyed
 * @param [in,out] msg The SeeMsgBuffer whose buffer must be destroyed.
 * \private
 */
static void
see_msg_buffer_destroy_buffer(SeeMsgBuffer* msg)
{
    if (!msg->buffer)
        return;
    free(msg->buffer);
    msg->buffer = NULL;
}


static int
msg_buffer_init(
    SeeMsgBuffer*               msg_buffer,
    const SeeMsgBufferClass*    msg_buffer_cls,
    SeeError**                  error_out
    )
{
    int ret = SEE_SUCCESS;
    const SeeObjectClass* parent_cls = SEE_OBJECT_GET_CLASS(
        msg_buffer
        );
        
    /* Because every class has its own members to initialize, you have
     * to check how your parent is initialized and pass through all relevant
     * parameters here. Typically, this should be parent_name_init, where
     * parent_name is the name of the parent and it should be the first function
     * that extends the parent above its parent.
     * Check how the parent is initialized and pass through the right parameters.
     */
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

    return ret;
}

static int
init(const SeeObjectClass* cls, SeeObject* obj, va_list args)
{
    const SeeMsgBufferClass* msg_buffer_cls = SEE_MSG_BUFFER_CLASS(cls);
    SeeMsgBuffer* msg_buffer = SEE_MSG_BUFFER(obj);
    
    /*Extract parameters here from va_list args here.*/
    SeeError** error = va_arg(args, SeeError**);
    
    return msg_buffer_cls->msg_buffer_init(
        msg_buffer,
        msg_buffer_cls,
        error
        );
}

static void
msg_buffer_destroy(SeeObject* obj)
{
    const SeeObjectClass* cls = see_object_class();
    SeeMsgBuffer* msg = SEE_MSG_BUFFER(obj);

    see_msg_buffer_destroy_buffer(msg);
    see_object_decref(SEE_OBJECT(msg->parts));
    cls->destroy(obj);
}

static int
msg_buffer_set_type(
    SeeMsgBuffer*       mbuf,
    uint16_t            mtype
    )
{
    mbuf->msg_type = mtype;
    return SEE_SUCCESS;
}

static int
msg_buffer_get_type(
    const SeeMsgBuffer* mbuf,
    uint16_t*           type
    )
{
    *type = mbuf->msg_type;
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

/* **** implementation of the public API **** */

int see_msg_buffer_new(SeeMsgBuffer** buf, SeeError** error_out)
{
    const SeeObjectClass* cls = SEE_OBJECT_CLASS(see_msg_buffer_class());

    if (!cls)
        return SEE_NOT_INITIALIZED;

    if (!buf || *buf || !error_out || *error_out)
        return SEE_INVALID_ARGUMENT;

    return cls->new_obj(cls, 0, SEE_OBJECT_REF(buf), error_out);
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
    cls->set_msg_type       = msg_buffer_set_type;
    cls->get_msg_type       = msg_buffer_get_type;
    cls->add_part           = msg_buffer_add_part;
    cls->get_part           = msg_buffer_get_part;
    cls->num_parts          = msg_buffer_num_parts;

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
    SeeMsgPartTypeError** error,
    see_msg_part_value_t  expected,
    see_msg_part_value_t  asked
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

