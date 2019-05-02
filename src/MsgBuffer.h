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


#ifndef SEE_MSG_BUFFER_H
#define SEE_MSG_BUFFER_H

#include "SeeObject.h"
#include "Error.h"
#include "DynamicArray.h"

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * These enumerated values are used to indicate what kind of value is stored
 * in 1 SeeMsgBufferPart.
 */
enum msg_part_value_t {
    /**
     * \brief The SeeMsgBufferPart is currently not initialized.
     */
    SEE_MSG_PART_NOT_INIT,
    /**
     * \brief The part contains a 32 bit signed integer
     */
    SEE_MSG_PART_INT32_T,
    /**
     * \brief The part contains a 64 bit signed integer
     */
    SEE_MSG_PART_INT64_T,
    /**
     * \brief the part contains a 32 bit unsigned integer
     */
    SEE_MSG_PART_UINT32_T,
    /**
     * \brief the part contains a 64 bit unsigned integer
     */
    SEE_MSG_PART_UINT64_T,
    /**
     * \brief The part contains a string. A string is encoded inside of the
     * MessageBufferPart as a 32 bit unsigned integer that representes the number
     * of bytes that the string is long, the terminating null byte is not
     * encoded inside of the buffer.
     */
    SEE_MSG_PART_STRING_T,
    /**
     * \brief The part contains a floating point number. The number is encoded
     * as a string message part. So the string should be converted back to a
     * double/float.
     */
    SEE_MSG_PART_FLOAT_T,

    /**
     * \brief If a type larger or equal to this type is received we know the
     * package is invalid.
     */
    SEE_MSG_PART_TRAILER,
};

typedef struct _SeeMsgBufferPart SeeMsgBufferPart;
typedef struct _SeeMsgBufferPartClass SeeMsgBufferPartClass;

struct _SeeMsgBufferPart {

    /**
     * \brief The parent object
     * \private
     */
    SeeObject parent_obj;


    /**
     * \brief The value is put in this union.
     *
     * floating point number are transformed to string and subsequently stored
     * as a string.
     * Integers are stored in network order.
     * \private
     */
    union {
        int32_t     int32_val;
        uint32_t    uint32_val;
        int64_t     int64_val;
        uint64_t    uint64_val;
        char*       str_val;
    } value;

    /**
     * \brief The total length of the message part.
     *
     * The length is consists of the length of the header of the part
     * which is sizeof(length) + sizeof(value_type) + sizeof the payload.
     *
     * \private
     */
    uint32_t    length;

    /**
     * This value defines the values that this package contains.
     */
    uint32_t    value_type;
};

struct _SeeMsgBufferPartClass {

    SeeObjectClass parent_cls;

    int (*msg_buffer_part_init) (
        SeeMsgBufferPart*            msg_buffer_part,
        const SeeMsgBufferPartClass* msg_buffer_part_cls
        /* Put instance specific arguments here and remove this comment. */
        );

    int (*write_int32) (
        SeeMsgBufferPart*       msg_buf_p,
        int32_t                 value,
        SeeError**              error_out
        );

    int (*get_int32) (
        const SeeMsgBufferPart* msg_buf_p,
        int32_t*                value,
        SeeError**              error_out
        );

    int (*write_uint32) (
        SeeMsgBufferPart*       msg_buf_p,
        uint32_t                value,
        SeeError**              error_out
        );

    int (*get_uint32) (
        const SeeMsgBufferPart* msg_buf_p,
        uint32_t*               value_out,
        SeeError**              error_out
        );

    int (*write_int64) (
        SeeMsgBufferPart*       msg_buf_p,
        int64_t                 value,
        SeeError**              error_out
    );

    int (*get_int64) (
        const SeeMsgBufferPart* msg_buf_p,
        int64_t*                value,
        SeeError**              error_out
    );

    int (*write_uint64) (
        SeeMsgBufferPart*       msg_buf_p,
        uint64_t                value,
        SeeError**              error_out
    );

    int (*get_uint64) (
        const SeeMsgBufferPart* msg_buf_p,
        uint64_t*               value_out,
        SeeError**              error_out
    );

    int (*write_string) (
        SeeMsgBufferPart*       msg_buf_p,
        const char*             value,
        size_t                  length,
        SeeError**              error_out
        );

    int (*get_string) (
        SeeMsgBufferPart*       msg_buf_p,
        char**                  value_out,
        SeeError**              error_out
        );

    int (*write_float) (
        SeeMsgBufferPart*       msg_buf,
        double                  value,
        SeeError**              error_out
        );

    int (*get_float) (
        const SeeMsgBufferPart* msg_buf_p,
        double*                 value_out,
        SeeError**              error_out
        );

    int (*buffer_length) (
        const SeeMsgBufferPart* msg_buf_p,
        size_t*                 size_out,
        SeeError**              error_out
        );

};

/* **** function style macro casts **** */

/**
 * \brief cast a pointer from a SeeMsgBufferPart derived instance back to a
 *        pointer to SeeMsgBufferPart.
 */
#define SEE_MSG_BUFFER_PART(obj)                      \
    ((SeeMsgBufferPart*) obj)

/**
 * \brief cast a pointer to pointer from a SeeMsgBufferPart derived instance back to a
 *        reference to SeeMsgBufferPart*.
 */
#define SEE_MSG_BUFFER_PART_REF(ref)                      \
    ((SeeMsgBufferPart**) ref)

/**
 * \brief cast a pointer to SeeMsgBufferPartClass derived class back to a
 *        pointer to SeeMsgBufferPartClass.
 */
#define SEE_MSG_BUFFER_PART_CLASS(cls)                      \
    ((const SeeMsgBufferPartClass*) cls)

/**
 * \brief obtain a pointer to SeeMsgBufferPartClass from a instance of
 *        derived from SeeMsgBufferPart. This macro is preferably
 *        used when obtaining the class of a instance. When this
 *        macro is used. Calling methods on the class will enable
 *        polymorphism, because you'll get the derived class.
 */
#define SEE_MSG_BUFFER_PART_GET_CLASS(obj)                \
    (SEE_MSG_BUFFER_PART_CLASS(see_object_get_class(SEE_OBJECT(obj)) )  )

/* **** public functions **** */

/**
 * Gets the pointer to the SeeMsgBufferPartClass table.
 */
SEE_EXPORT const SeeMsgBufferPartClass*
see_msg_buffer_part_class();

/* Expand the class with public functions here, don't forget the SEE_EXPORT
 * macro, because otherwise you'll run into troubles when exporting function
 * in a windows dll.
 */

/* **** class initialization functions **** */

/**
 * Initialize SeeMsgBufferPart; make it ready for use.
 */
SEE_EXPORT
int see_msg_buffer_part_init();

/**
 * Deinitialize SeeMsgBufferPart, after SeeMsgBufferPart has been deinitialized,
 * all functions in this header shouldn't be used anymore.
 */
SEE_EXPORT
void see_msg_buffer_part_deinit();

/* **** Message Buffer **** */

typedef struct _SeeMsgBuffer SeeMsgBuffer;
typedef struct _SeeMsgBufferClass SeeMsgBufferClass;

struct _SeeMsgBuffer {

    SeeObject parent_obj;

    /*expand SeeMsgBuffer data here*/

    /**
     * \brief The parts stored in this message
     *
     * A message can consist of multiple parts, all the parts are put into
     * the buffer in such a way that the remote end is able to tell which
     * parts are embedded in the message.
     *
     * \private
     */
    SeeDynamicArray*    parts;

    /**
     * The total message linearized to a number of bytes.
     */
    void*               buffer;

    int                 msg_type;
};

struct _SeeMsgBufferClass {

    SeeObjectClass parent_cls;
    
    int (*msg_buffer_init)(
        SeeMsgBuffer*            msg_buffer,
        const SeeMsgBufferClass* msg_buffer_cls
        /* Put instance specific arguments here and remove this comment. */
        );
        
    /* expand SeeMsgBuffer class with extra functions here.*/

    int (*add_part) (
        SeeMsgBuffer*       msg,
        SeeMsgBufferPart*   buf,
        SeeError**          error
        );

    int (*get_part) (
        SeeMsgBuffer*       msg,
        SeeMsgBuffer**      part_out,
        SeeError**          error_out
        );
};

/* **** function style macro casts **** */

/**
 * \brief cast a pointer from a SeeMsgBuffer derived instance back to a
 *        pointer to SeeMsgBuffer.
 */
#define SEE_MSG_BUFFER(obj)                      \
    ((SeeMsgBuffer*) obj)

/**
 * \brief cast a pointer to pointer from a SeeMsgBuffer derived instance back to a
 *        reference to SeeMsgBuffer*.
 */
#define SEE_MSG_BUFFER_REF(ref)                      \
    ((SeeMsgBuffer**) ref)

/**
 * \brief cast a pointer to SeeMsgBufferClass derived class back to a
 *        pointer to SeeMsgBufferClass.
 */
#define SEE_MSG_BUFFER_CLASS(cls)                      \
    ((const SeeMsgBufferClass*) cls)

/**
 * \brief obtain a pointer to SeeMsgBufferClass from a instance of
 *        derived from SeeMsgBuffer. This macro is preferably
 *        used when obtaining the class of a instance. When this
 *        macro is used. Calling methods on the class will enable
 *        polymorphism, because you'll get the derived class.
 */
#define SEE_MSG_BUFFER_GET_CLASS(obj)                \
    (SEE_MSG_BUFFER_CLASS(see_object_get_class(SEE_OBJECT(obj)) )  )

/* **** public functions **** */

/**
 * Gets the pointer to the SeeMsgBufferClass table.
 */
SEE_EXPORT const SeeMsgBufferClass*
see_msg_buffer_class();

/* Expand the class with public functions here, don't forget the SEE_EXPORT
 * macro, because otherwise you'll run into troubles when exporting function
 * in a windows dll.
 */

/* **** class initialization functions **** */

/**
 * Initialize SeeMsgBuffer; make it ready for use.
 */
SEE_EXPORT
int see_msg_buffer_init();

/**
 * Deinitialize SeeMsgBuffer, after SeeMsgBuffer has been deinitialized,
 * all functions in this header shouldn't be used anymore.
 */
SEE_EXPORT
void see_msg_buffer_deinit();

#ifdef __cplusplus
}
#endif

#endif //ifndef SEE_MSG_BUFFER_H
