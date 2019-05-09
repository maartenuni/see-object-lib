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
 * in 1 SeeMsgPart.
 */
typedef enum msg_part_value_t {
    /**
     * \brief The SeeMsgPart is currently not initialized.
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
} see_msg_part_value_t;

typedef struct _SeeMsgPart SeeMsgPart;
typedef struct _SeeMsgPartClass SeeMsgPartClass;

struct _SeeMsgPart {

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
    uint16_t    length;

    /**
     * This value defines the values that this package contains.
     */
    uint16_t    value_type;
};

struct _SeeMsgPartClass {

    SeeObjectClass parent_cls;

    int (*msg_part_init) (
        SeeMsgPart*            msg_part,
        const SeeMsgPartClass* msg_part_cls
        /* Put instance specific arguments here and remove this comment. */
        );

    int (*length) (
        const SeeMsgPart* part,
        uint16_t*         length,
        SeeError**        error_out
        );

    int (*value_type) (
        const SeeMsgPart* part,
        uint16_t*         type
        );

    int (*write_int32) (
        SeeMsgPart*       msg_buf_p,
        int32_t           value,
        SeeError**        error_out
        );

    int (*get_int32) (
        const SeeMsgPart* msg_buf_p,
        int32_t*          value,
        SeeError**        error_out
        );

    int (*write_uint32) (
        SeeMsgPart*       msg_buf_p,
        uint32_t          value,
        SeeError**        error_out
        );

    int (*get_uint32) (
        const SeeMsgPart* msg_buf_p,
        uint32_t*         value_out,
        SeeError**        error_out
        );

    int (*write_int64) (
        SeeMsgPart*       msg_buf_p,
        int64_t           value,
        SeeError**        error_out
        );

    int (*get_int64) (
        const SeeMsgPart* msg_buf_p,
        int64_t*          value,
        SeeError**        error_out
        );

    int (*write_uint64) (
        SeeMsgPart*       msg_buf_p,
        uint64_t          value,
        SeeError**        error_out
        );

    int (*get_uint64) (
        const SeeMsgPart* msg_buf_p,
        uint64_t*         value_out,
        SeeError**        error_out
        );

    int (*write_string) (
        SeeMsgPart*       msg_buf_p,
        const char*       value,
        size_t            length,
        SeeError**        error_out
        );

    int (*get_string) (
        const SeeMsgPart* msg_buf_p,
        char**            value_out,
        SeeError**        error_out
        );

    int (*write_float) (
        SeeMsgPart*       msg_buf,
        double            value,
        SeeError**        error_out
        );

    int (*get_float) (
        const SeeMsgPart* msg_buf_p,
        double*           value_out,
        SeeError**        error_out
        );

    int (*buffer_length) (
        const SeeMsgPart* msg_buf_p,
        size_t*           size_out,
        SeeError**        error_out
        );

};

/* **** function style macro casts **** */

/**
 * \brief cast a pointer from a SeeMsgPart derived instance back to a
 *        pointer to SeeMsgPart.
 */
#define SEE_MSG_PART(obj)                      \
    ((SeeMsgPart*) obj)

/**
 * \brief cast a pointer to pointer from a SeeMsgPart derived instance back to a
 *        reference to SeeMsgPart*.
 */
#define SEE_MSG_PART_REF(ref)                      \
    ((SeeMsgPart**) ref)

/**
 * \brief cast a pointer to SeeMsgPartClass derived class back to a
 *        pointer to SeeMsgPartClass.
 */
#define SEE_MSG_PART_CLASS(cls)                      \
    ((const SeeMsgPartClass*) cls)

/**
 * \brief obtain a pointer to SeeMsgPartClass from a instance of
 *        derived from SeeMsgPart. This macro is preferably
 *        used when obtaining the class of a instance. When this
 *        macro is used. Calling methods on the class will enable
 *        polymorphism, because you'll get the derived class.
 */
#define SEE_MSG_PART_GET_CLASS(obj)                \
    (SEE_MSG_PART_CLASS(see_object_get_class(SEE_OBJECT(obj)) )  )

/* **** public functions **** */

/**
 * @brief Create a new message buffer part.
 *
 * A message buffer contains 0 or more of these parts.
 *
 * @param [out] part A pointer to a SeeMsgPart Pointer*
 * @param [out] error_out
 * @return SEE_SUCCESS, SEE_INVALID_ARGUMENT, SEE_ERROR_RUNTIME
 */
SEE_EXPORT int
see_msg_part_new(SeeMsgPart** part, SeeError** error_out);

/**
 * @brief Obtain the length in byte of the message part
 *
 * @param [in]  part      The part whose length you would like to know.
 * @param [out] length    The length of the message part is returned here.
 * @param [out] error_out If an error occurs it is returned here.
 *
 * @return SEE_SUCCESS, SEE_INVALID_ARGUMENT or ...
 */
SEE_EXPORT int
see_msg_part_length(
    const SeeMsgPart* part,
    uint16_t*         length,
    SeeError**        error_out
    );

/**
 * @brief Obtain the type of a part, this can be used to see what information
 *        is included in the part.
 * @param [in]  part The part whose type you would like to know.
 * @param [out] val_type The value type is returned here.
 *
 * @return SEE_SUCCESS, SEE_INVALID_ARGUMENT
 */
SEE_EXPORT int
see_msg_part_value_type(
    const SeeMsgPart*   part,
    uint16_t*           val_type
    );

/**
 * @brief write a signed integer with 32 bits precisions to the part
 *
 * @param [in,out]  part        The part to which you would like to write the int.
 * @param [in]      value       The value to write
 * @param [out]     error_out   If an error occurs it is returned here.
 *
 * @return SEE_SUCCESS, SEE_INVALID_ARGUMENT
 */
SEE_EXPORT int
see_msg_part_write_int32(
    SeeMsgPart*       part,
    int32_t           value,
    SeeError**        error_out
    );

/**
 * @brief Obtain a signed integer with 32 bits precisions from the part
 *
 * @param [in,out]  part        The part to which you would like to obtain the int.
 * @param [out]     value       The value is returned here
 * @param [out]     error_out   If an error occurs it is returned here.
 *
 * @return SEE_SUCCESS, SEE_INVALID_ARGUMENT
 */
SEE_EXPORT int
see_msg_part_get_int32(
    const SeeMsgPart* part,
    int32_t*          value,
    SeeError**        error_out
    );

/**
 * @brief write a unsigned integer with 32 bits precisions to the part
 *
 * @param [in,out]  part        The part to which you would like to write the int.
 * @param [in]      value       The value to write
 * @param [out]     error_out   If an error occurs it is returned here.
 *
 * @return SEE_SUCCESS, SEE_INVALID_ARGUMENT
 */
SEE_EXPORT int
see_msg_part_write_uint32(
    SeeMsgPart*       part,
    uint32_t           value,
    SeeError**        error_out
    );

/**
 * @brief Obtain a unsigned integer with 32 bits precisions from the part
 *
 * @param [in,out]  part        The part to which you would like to obtain the int.
 * @param [out]     value       The value is returned here
 * @param [out]     error_out   If an error occurs it is returned here.
 *
 * @return SEE_SUCCESS, SEE_INVALID_ARGUMENT
 */
SEE_EXPORT int
see_msg_part_get_uint32(
    const SeeMsgPart* part,
    uint32_t*         value,
    SeeError**        error_out
    );

/**
 * @brief write a signed integer with 64 bits precisions to the part
 *
 * @param [in,out]  part        The part to which you would like to write the int.
 * @param [in]      value       The value to write
 * @param [out]     error_out   If an error occurs it is returned here.
 *
 * @return SEE_SUCCESS, SEE_INVALID_ARGUMENT
 */
SEE_EXPORT int
see_msg_part_write_int64(
    SeeMsgPart*       part,
    int64_t           value,
    SeeError**        error_out
    );

/**
 * @brief Obtain a signed integer with 64 bits precisions from the part
 *
 * @param [in,out]  part        The part to which you would like to obtain the int.
 * @param [out]     value       The value is returned here
 * @param [out]     error_out   If an error occurs it is returned here.
 *
 * @return SEE_SUCCESS, SEE_INVALID_ARGUMENT
 */
SEE_EXPORT int
see_msg_part_get_int64(
    const SeeMsgPart* part,
    int64_t*          value,
    SeeError**        error_out
    );

/**
 * @brief write a unsigned integer with 64 bits precisions to the part
 *
 * @param [in,out]  part        The part to which you would like to write the int.
 * @param [in]      value       The value to write
 * @param [out]     error_out   If an error occurs it is returned here.
 *
 * @return SEE_SUCCESS, SEE_INVALID_ARGUMENT
 */
SEE_EXPORT int
see_msg_part_write_uint64(
    SeeMsgPart*       part,
    uint64_t           value,
    SeeError**        error_out
    );

/**
 * @brief Obtain a unsigned integer with 64 bits precisions from the part
 *
 * @param [in,out]  part        The part to which you would like to obtain the int.
 * @param [out]     value       The value is returned here
 * @param [out]     error_out   If an error occurs it is returned here.
 *
 * @return SEE_SUCCESS, SEE_INVALID_ARGUMENT
 */
SEE_EXPORT int
see_msg_part_get_uint64(
    const SeeMsgPart* part,
    uint64_t*         value,
    SeeError**        error_out
    );

/**
 * @brief write a string to the part
 *
 * @param [in,out]  part        The part to which you would like to write the int.
 * @param [in]      value       The string to write.
 * @param [in]      size        The length in bytes of the string.
 * @param [out]     error_out   If an error occurs it is returned here.
 *
 * @return SEE_SUCCESS, SEE_INVALID_ARGUMENT
 */
SEE_EXPORT int
see_msg_part_write_string(
    SeeMsgPart*     part,
    const char*     value,
    size_t          size,
    SeeError**      error_out
    );

/**
 * @brief Obtain a string from the message part.
 * @param [in] part      The part from which to obtain a message
 * @param [out]value     The value is returned here, free it after use.
 * @param [out]error_out If an error occurs info about is returned here.
 * @return SEE_SUCCESS, SEE_INVALID_ARGUMENT, SEE_ERROR_RUNTIME
 */
SEE_EXPORT int
see_msg_part_get_string(
    const SeeMsgPart*   part,
    char**              value,
    SeeError**          error_out
    );

/**
 * @brief write a floating point number to the part.
 *
 * @param [in,out]  part        The part to which you would like to write the int.
 * @param [in]      value       The value to write
 * @param [out]     error_out   If an error occurs it is returned here.
 *
 * @return SEE_SUCCESS, SEE_INVALID_ARGUMENT
 */
SEE_EXPORT int
see_msg_part_write_float(
    SeeMsgPart*         part,
    double              value,
    SeeError**          error_out
    );

/**
 * @brief Obtain a floating point number from the part
 *
 * @param [in,out]  part        The part to which you would like to obtain the int.
 * @param [out]     value       The value is returned here
 * @param [out]     error_out   If an error occurs it is returned here.
 *
 * @return SEE_SUCCESS, SEE_INVALID_ARGUMENT
 */
SEE_EXPORT int
see_msg_part_get_float(
    const SeeMsgPart*   part,
    double*             value,
    SeeError**          error_out
    );


/**
 * Gets the pointer to the SeeMsgPartClass table.
 */
SEE_EXPORT const SeeMsgPartClass*
see_msg_part_class();

/* **** class initialization functions **** */

/**
 * Initialize SeeMsgPart; make it ready for use.
 */
SEE_EXPORT
int see_msg_part_init();

/**
 * Deinitialize SeeMsgPart, after SeeMsgPart has been deinitialized,
 * all functions in this header shouldn't be used anymore.
 */
SEE_EXPORT
void see_msg_part_deinit();


/* ************************ */
/* **** Message Buffer **** */
/* ************************ */

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

    uint16_t            msg_type;
};

struct _SeeMsgBufferClass {

    SeeObjectClass parent_cls;
    
    int (*msg_buffer_init)(
        SeeMsgBuffer*            msg_buffer,
        const SeeMsgBufferClass* msg_buffer_cls,
        SeeError**               error_out
        );

    int (*set_msg_type) (
        SeeMsgBuffer*       msg,
        uint16_t            msg_type
        );

    int (*get_msg_type) (
        const SeeMsgBuffer* msg,
        uint16_t*           msg_type
        );

    int (*add_part) (
        SeeMsgBuffer*       msg,
        SeeMsgPart*   buf,
        SeeError**          error
        );

    int (*get_part) (
        SeeMsgBuffer*       msg,
        size_t              size,
        SeeMsgPart**  part_out,
        SeeError**          error_out
        );

    int (*num_parts) (
        const SeeMsgBuffer* msg,
        size_t*             size
        );

//    int (*get_buffer) (
//        SeeMsgBuffer*   msg,
//        void**          buffer_out,
//        SeeError**      error_out
//        );
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
 * @brief Create a new message buffer.
 *
 * @param [out] buf_out   If successful a new buffer is returned here.
 * @param [out] error_out If unsuccessful a error might be returned here.
 *
 * @return SEE_SUCCESS, SEE_NOT_INITIALISED, SEE_INVALID_ARGUMENT
 */
SEE_EXPORT int
see_msg_buffer_new(
    SeeMsgBuffer**  buf_out,
    SeeError**      error_out
    );

/**
 * Gets the pointer to the SeeMsgBufferClass table.
 */
SEE_EXPORT const SeeMsgBufferClass*
see_msg_buffer_class();

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


/* ********************************* */
/* **** Message Part Type Error **** */
/* ********************************* */

typedef struct _SeeMsgPartTypeError SeeMsgPartTypeError;
typedef struct _SeeMsgPartTypeErrorClass SeeMsgPartTypeErrorClass;

struct _SeeMsgPartTypeError {
    SeeError parent_obj;
    /*expand SeeMsgPartTypeError data here*/

};

struct _SeeMsgPartTypeErrorClass {
    SeeErrorClass parent_cls;

    int (*msg_part_type_error_init)(
        SeeMsgPartTypeError*            msg_part_type_error,
        const SeeMsgPartTypeErrorClass* msg_part_type_error_cls,
        see_msg_part_value_t            expected,
        see_msg_part_value_t            asked
        );

};

/* **** function style macro casts **** */

/**
 * \brief cast a pointer from a SeeMsgPartTypeError derived instance back to a
 *        pointer to SeeMsgPartTypeError.
 */
#define SEE_MSG_PART_TYPE_ERROR(obj)                      \
    ((SeeMsgPartTypeError*) obj)

/**
 * \brief cast a pointer to pointer from a SeeMsgPartTypeError derived instance back to a
 *        reference to SeeMsgPartTypeError*.
 */
#define SEE_MSG_PART_TYPE_ERROR_REF(ref)                      \
    ((SeeMsgPartTypeError**) ref)

/**
 * \brief cast a pointer to SeeMsgPartTypeErrorClass derived class back to a
 *        pointer to SeeMsgPartTypeErrorClass.
 */
#define SEE_MSG_PART_TYPE_ERROR_CLASS(cls)                      \
    ((const SeeMsgPartTypeErrorClass*) cls)

/**
 * \brief obtain a pointer to SeeMsgPartTypeErrorClass from a instance of
 *        derived from SeeMsgPartTypeError. This macro is preferably
 *        used when obtaining the class of a instance. When this
 *        macro is used. Calling methods on the class will enable
 *        polymorphism, because you'll get the derived class.
 */
#define SEE_MSG_PART_TYPE_ERROR_GET_CLASS(obj)                \
    (SEE_MSG_PART_TYPE_ERROR_CLASS(see_object_get_class(SEE_OBJECT(obj)) )  )

/* **** public functions **** */

SEE_EXPORT int
see_msg_part_type_error_new(
    SeeMsgPartTypeError**   error,
    see_msg_part_value_t    expected,
    see_msg_part_value_t    asked
    );

/**
 * Gets the pointer to the SeeMsgPartTypeErrorClass table.
 */
SEE_EXPORT const SeeMsgPartTypeErrorClass*
see_msg_part_type_error_class();

/* Expand the class with public functions here, don't forget the SEE_EXPORT
 * macro, because otherwise you'll run into troubles when exporting function
 * in a windows dll.
 */

/* **** class initialization functions **** */

/**
 * Initialize SeeMsgPartTypeError; make it ready for use.
 */
SEE_EXPORT
int see_msg_part_type_error_init();

/**
 * Deinitialize SeeMsgPartTypeError, after SeeMsgPartTypeError has been deinitialized,
 * all functions in this header shouldn't be used anymore.
 */
SEE_EXPORT
void see_msg_part_type_error_deinit();

#ifdef __cplusplus
}
#endif

#endif //ifndef SEE_MSG_BUFFER_H
