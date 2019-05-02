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


#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>

#include "MetaClass.h"
#include "MsgBuffer.h"
#include "see_object_config.h"
#include "utilities.h"
#include "RuntimeError.h"

static size_t
msg_buffer_part_header_length()
{
    // sizeof(SeeMsgBufferPart.value_type) + sizeof(SeeMsgBufferPart.length);
    return sizeof(uint32_t) + sizeof(uint32_t);
}

/* **** functions that implement SeeMsgBufferPart or override SeeObject **** */

static int
msg_buffer_part_init(
    SeeMsgBufferPart* msg_buffer_part,
    const SeeMsgBufferPartClass* msg_buffer_part_cls
    /*Add your parameters here and make sure you obtain them in init below*/
)
{
    int ret = SEE_SUCCESS;
    const SeeObjectClass* parent_cls = SEE_OBJECT_GET_CLASS(
        msg_buffer_part
    );

    /* Because every class has its own members to initialize, you have
     * to check how your parent is initialized and pass through all relevant
     * parameters here. Typically, this should be parent_name_init, where
     * parent_name is the name of the parent and it should be the first function
     * that extends the parent above its parent.
     * Check how the parent is initialized and pass through the right parameters.
     */
    parent_cls->object_init(
        SEE_OBJECT(msg_buffer_part),
        SEE_OBJECT_CLASS(msg_buffer_part_cls)
        );

    /*
     * Check if the parent initialization was successful.
     * if not return a useful error value.
     */

    /*
     * Initialize whatever the parents initializer function didn't initialize.
     * Typically, SeeMsgBufferPart extends SeeObject with one or
     * a few new members. Those bytes should be 0, since the default
     * SeeObjectClass->new() uses calloc to allocate 1 instance.
     * However, this is the place to give them a sensible value.
     */

    return ret;
}

static int
part_init(const SeeObjectClass* cls, SeeObject* obj, va_list args)
{
    const SeeMsgBufferPartClass* msg_buffer_part_cls = SEE_MSG_BUFFER_PART_CLASS(cls);
    SeeMsgBufferPart* msg_buffer_part = SEE_MSG_BUFFER_PART(obj);

    /*Extract parameters here from va_list args here.*/
    (void) args;

    return msg_buffer_part_cls->msg_buffer_part_init(
        msg_buffer_part,
        msg_buffer_part_cls
        /*Add your extra parameters here.*/
    );

}

static int
msg_buffer_part_write_int32(
    SeeMsgBufferPart*   part,
    int32_t             value,
    SeeError**          error_out
    )
{
    (void) error_out;
    part->value_type        = SEE_MSG_PART_INT32_T;
    part->value.int32_val   = see_host_to_network32(value);

    part->length = msg_buffer_part_header_length() + sizeof(int32_t);

    return SEE_SUCCESS;
}

static int
msg_buffer_part_get_int32 (
    SeeMsgBufferPart*   part,
    int32_t*            value,
    SeeError**          error_out
    )
{
    if (part->value_type != SEE_MSG_PART_INT32_T) {
        int toto_create_a_error;
        return; // SEE_ERROR_MESSAGE;
    }

    int32_t host_val = see_network_to_host32(part->value.int32_val);
    *value = host_val;

    return SEE_SUCCESS;
}

static int
msg_buffer_part_write_uint32(
    SeeMsgBufferPart*   part,
    uint32_t            value,
    SeeError**          error_out
    )
{
    (void) error_out;
    part->value_type        = SEE_MSG_PART_UINT32_T;
    part->value.uint32_val  = see_host_to_network32(value);

    part->length = msg_buffer_part_header_length() + sizeof(uint32_t);

    return SEE_SUCCESS;
}

static int
msg_buffer_part_get_uint32 (
    SeeMsgBufferPart*   part,
    uint32_t*           value,
    SeeError**          error_out
)
{
    if (part->value_type != SEE_MSG_PART_UINT32_T) {
        int toto_create_a_error;
        return; // SEE_ERROR_MESSAGE;
    }

    uint32_t host_val = see_network_to_host32(part->value.uint32_val);
    *value = host_val;

    return SEE_SUCCESS;
}

static int
msg_buffer_part_write_int64(
    SeeMsgBufferPart*   part,
    int64_t             value,
    SeeError**          error_out
    )
{
    (void) error_out;
    part->value_type        = SEE_MSG_PART_INT64_T;
    part->value.int64_val   = see_host_to_network64(value);

    part->length = msg_buffer_part_header_length() + sizeof(int64_t);

    return SEE_SUCCESS;
}

static int
msg_buffer_part_get_int64 (
    SeeMsgBufferPart*   part,
    int64_t*            value,
    SeeError**          error_out
    )
{
    if (part->value_type != SEE_MSG_PART_INT64_T) {
        int toto_create_a_error;
        return; // SEE_ERROR_MESSAGE;
    }

    int64_t host_val = see_network_to_host64(part->value.int64_val);
    *value = host_val;

    return SEE_SUCCESS;
}

static int
msg_buffer_part_write_uint64(
    SeeMsgBufferPart*   part,
    uint64_t            value,
    SeeError**          error_out
    )
{
    (void) error_out;
    part->value_type        = SEE_MSG_PART_UINT64_T;
    part->value.uint64_val  = see_host_to_network64(value);

    part->length = msg_buffer_part_header_length() + sizeof(uint64_t);

    return SEE_SUCCESS;
}

static int
msg_buffer_part_get_uint64 (
    SeeMsgBufferPart*   part,
    uint64_t*           value,
    SeeError**          error_out
    )
{
    if (part->value_type != SEE_MSG_PART_UINT64_T) {
        int toto_create_a_error;
        return; // SEE_ERROR_MESSAGE;
    }

    int64_t host_val = see_network_to_host64(part->value.int64_val);
    *value = host_val;

    return SEE_SUCCESS;
}

static int
msg_buffer_part_write_string(
    SeeMsgBufferPart*   part,
    const char*         value,
    size_t              length,
    SeeError**          error_out
    )
{
    char* duplicate = malloc(length);
    if (!duplicate) {
        see_runtime_error_create(error_out, errno);
        return SEE_ERROR_RUNTIME;
    }

    memcpy(duplicate, value, length);

    part->length = msg_buffer_part_header_length() + length;
}

static int
msg_buffer_part_get_string(
    SeeMsgBufferPart*   part,
    char**              value_out,
    SeeError**          error_out
    )
{
    // floats are also encoded as string
    if ( part->value_type != SEE_MSG_PART_STRING_T &&
         part->value_type != SEE_MSG_PART_FLOAT_T ) {
        return SEE_ERROR_RUNTIME;
    }

    size_t length = part->length - msg_buffer_part_header_length();
    *value_out = malloc(length + 1);
    if (!*value_out) {
        see_runtime_error_create(error_out, errno);
        return SEE_ERROR_RUNTIME;
    }
    memcpy(*value_out, part->value.str_val, length);
    *value_out[length] = 0; //terminate the string.

    return SEE_SUCCESS;
}

static int
msg_buffer_part_write_float(
    SeeMsgBufferPart*   part,
    double              value,
    SeeError**          error_out
    )
{
    const SeeMsgBufferPartClass* cls = SEE_MSG_BUFFER_GET_CLASS(part);

    char buffer[64];
    snprintf(buffer, sizeof(buffer), "%g", value);
    size_t length = strlen(buffer);
    int ret = cls->write_string(part, buffer, length, error_out);
    if(ret)
        return ret;
    part->value_type = SEE_MSG_PART_FLOAT_T;
}

static int
msg_buffer_part_get_float(
    SeeMsgBufferPart*   part,
    double*             value_out,
    SeeError**          error_out
    )
{
    const SeeMsgBufferPartClass* cls = SEE_MSG_BUFFER_GET_CLASS(part);
    char* text_double = NULL;
    double output;

    int ret = cls->get_string(part, &text_double, error_out);
    if (ret)
        return ret;

    assert(text_double != NULL);
    int nmatched = sscanf(text_double, "%lg", &output);
    if (nmatched == 0) {
        int create_error;
        return SEE_ERROR_RUNTIME;
    }
    *value_out = output;

    return SEE_SUCCESS;
}

static int
msg_buffer_part_buffer_length(
    const SeeMsgBufferPart* part,
    size_t*                 size,
    SeeError**              error_out
    )
{
    if (part->value_type == SEE_MSG_PART_NOT_INIT ||
        part->value_type >= SEE_MSG_PART_TRAILER) {

        SeeError* error = NULL;
        return SEE_ERROR_RUNTIME;
    }

    *size = part->length - msg_buffer_part_header_length();

    return SEE_SUCCESS;
}

/* **** implementation of the public API **** */

/* **** initialization of the class **** */

SeeMsgBufferPartClass* g_SeeMsgBufferPartClass = NULL;

static int see_msg_buffer_part_class_init(SeeObjectClass* new_cls)
{
    int ret = SEE_SUCCESS;

    /* Override the functions on the parent here */
    new_cls->init = part_init;
    new_cls->name = "SeeMsgBufferPart";

    /* Set the function pointers of the own class here */
    SeeMsgBufferPartClass* cls = (SeeMsgBufferPartClass*) new_cls;
    cls->msg_buffer_part_init   = msg_buffer_part_init;

    cls->write_int32            = msg_buffer_part_write_int32;
    cls->get_int32              = msg_buffer_part_get_int32;
    cls->write_uint32           = msg_buffer_part_write_uint32;
    cls->get_uint32             = msg_buffer_part_get_uint32;

    cls->write_int64            = msg_buffer_part_write_int64;
    cls->get_int64              = msg_buffer_part_get_int64;
    cls->write_uint64           = msg_buffer_part_write_uint64;
    cls->get_uint64             = msg_buffer_part_get_uint64;

    cls->write_string           = msg_buffer_part_write_string;
    cls->get_string             = msg_buffer_part_get_string;

    cls->write_float            = msg_buffer_part_write_float;
    cls->get_float              = msg_buffer_part_get_float;

    cls->buffer_length          = msg_buffer_part_buffer_length;

    return ret;
}

/**
 * \private
 * \brief this class initializes SeeMsgBufferPart(Class).
 *
 * You might want to call this from the library initialization func.
 */
int
see_msg_buffer_part_init()
{
    int ret;
    const SeeMetaClass* meta = see_meta_class_class();

    ret = see_meta_class_new_class(
        meta,
        (SeeObjectClass**) &g_SeeMsgBufferPartClass,
        sizeof(SeeMsgBufferPartClass),
        sizeof(SeeMsgBufferPart),
        SEE_OBJECT_CLASS(see_object_class()),
        sizeof(SeeObjectClass),
        see_msg_buffer_part_class_init
    );

    return ret;
}

void
see_msg_buffer_part_deinit()
{
    if(!g_SeeMsgBufferPartClass)
        return;

    see_object_decref(SEE_OBJECT(g_SeeMsgBufferPartClass));
    g_SeeMsgBufferPartClass = NULL;
}

const SeeMsgBufferPartClass*
see_msg_buffer_part_class()
{
    return g_SeeMsgBufferPartClass;
}

/* ********************************************************************* */
/* **** functions that implement SeeMsgBuffer or override SeeObject **** */
/* ********************************************************************* */

static int
msg_buffer_init(
    SeeMsgBuffer* msg_buffer,
    const SeeMsgBufferClass* msg_buffer_cls
    /*Add your parameters here and make sure you obtain them in init below*/
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
    
     /*
     * Check if the parent initialization was successful.
     * if not return a useful error value.
     */
     
    /*
     * Initialize whatever the parents initializer function didn't initialize.
     * Typically, SeeMsgBuffer extends SeeObject with one or 
     * a few new members. Those bytes should be 0, since the default 
     * SeeObjectClass->new() uses calloc to allocate 1 instance.
     * However, this is the place to give them a sensible value.
     */
    
    return ret;
}

static int
init(const SeeObjectClass* cls, SeeObject* obj, va_list args)
{
    const SeeMsgBufferClass* msg_buffer_cls = SEE_MSG_BUFFER_CLASS(cls);
    SeeMsgBuffer* msg_buffer = SEE_MSG_BUFFER(obj);
    
    /*Extract parameters here from va_list args here.*/
    
    return msg_buffer_cls->msg_buffer_init(
        msg_buffer,
        msg_buffer_cls
        /*Add your extra parameters here.*/
        );
}

/* **** implementation of the public API **** */

/* **** initialization of the class **** */

SeeMsgBufferClass* g_SeeMsgBufferClass = NULL;

static int see_msg_buffer_class_init(SeeObjectClass* new_cls)
{
    int ret = SEE_SUCCESS;
    
    /* Override the functions on the parent here */
    new_cls->init = init;
    
    /* Set the function pointers of the own class here */
    SeeMsgBufferClass* cls = (SeeMsgBufferClass*) new_cls;
    
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

