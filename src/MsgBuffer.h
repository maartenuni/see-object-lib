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
 * \file MsgBuffer.h
 * \brief Exports the API a protocol for sending message across a wire.
 *
 * This file exports the SeeMsgBuffer and SeeMsgPart types. The idea of this
 * design is that a user can send a MsgBuffer over a wire or something like that.
 * The MsgBuffer contains a id, the id can be used across the wire to see
 * what type of message is send. The user is free to use id's that one likes,
 * then the other side should be able to recognise the id's. This last part
 * makes the "protocol" flexible, and the user defines how a message looks like.
 *
 * If a msg contains no parts it can be used as a flag. If a buffer does contain
 * parts it can send additional information across the wire. Say you want to
 * set the clock of a connected device. You could send a set_clock message,
 * with a string that signals which clock and a timestamp to set the clock.
 *
 * @code
 *
 * uint64_t micros = 0;
 * const char* clk = "synced_clk"
 *
 * const int set_clock_id = 1;
 * SeeError*     error = NULL;
 * SeeMsgBuffer* msg = NULL;
 * SeeMsgPart*   clock_id = NULL;
 * SeeMsgPart*   time = NULL;
 * SeeSerial*    serial;
 *
 * // error checking omitted
 *
 * int ret;
 * ret = see_serial_new_dev(&serial, "/dev/ttyAMC0", &error)
 * ret = see_msg_buffer_new(&msg, clock_id, &error);
 * ret = see_msg_part_new(&clock_id, &error);
 * ret = see_msg_part_new(&time, &error);
 * ret = see_msg_part_write_string(clock_id, clk, &error);
 * ret = see_msg_part_write_uint64(time, micros, &error);
 * ret = see_msg_buffer_add_part(msg, clock_id, &error);
 * ret = see_msg_buffer_add_part(msg, time, &error);
 *
 * ret = see_serial_send_msg(serial, msg, &error);
 *
 * @endcode
 *
 * So the message above can be transmitted as a message that looks like
 *
 * "SMSG" + id + length + part_clock_id + part_time
 *
 * the id is 2 bytes, the length = 4 bytes and then the two parts follow
 *
 * A part is stored in a buffer as a:
 *
 * see_msg_part_value_t [ + length] + payload.
 *
 * The length of the package is dependent on the type of the part, so the length
 * is only used (may-2019) when the part contains a string, otherwise the
 * length is dependent on the number of bytes of the payload + the see_msg_value_t
 * which takes one byte.
 *
 * The receiving end should examine the message_id and then retrieve the parts
 * from the message. So this layer add a protocol that implements a message stream
 * the user can define his/her own messages an put part in it. Perhaps in the
 * future a packetiser should be added that breaks these buffers in fixed size
 * packages, this would allow to optimize the stream for a given protocol.
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
 * \brief These enumerated values are used to indicate what kind of value is
 * stored in 1 SeeMsgPart.
 *
 * One shouldn't use these values directly, they are used internally to indicate
 * what type of content the packets contain.
 *
 * \private
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
     * \brief The part contains a floating point number.
     */
    SEE_MSG_PART_FLOAT_T,
    /**
     * \brief The part contains a floating point number.
     */
    SEE_MSG_PART_DOUBLE_T,

    /**
     * \brief If a type larger or equal to this type is received we know the
     * package is invalid.
     */
    SEE_MSG_PART_TRAILER,
} see_msg_part_value_t;

typedef struct _SeeMsgPart SeeMsgPart;
typedef struct _SeeMsgPartClass SeeMsgPartClass;

/**
 * \brief a structure to send one specific element over a wire.
 * @private
 */
struct _SeeMsgPart {

    /**
     * \brief The parent object
     * \private
     */
    SeeObject parent_obj;


    /**
     * \brief The value is put in this union.
     *
     * The integers are put in network order into this union.
     * The same goes for the floating point numbers. The string is a pointer
     * to a string. The 0 byte is not send over the connection, hence the length
     * of the SeeMsgPart should be used to deduce the length.
     * \private
     */
    union {
        int32_t     int32_val;
        uint32_t    uint32_val;
        int64_t     int64_val;
        uint64_t    uint64_val;
        char*       str_val;
        double      double_val;
        float       float_val;
    } value;

    /**
     * \brief The total length of the message part.
     *
     * The length is consists of the length of the header of the part
     * which is sizeof(length) + sizeof(value_type) + sizeof the payload.
     *
     * The length of the package is only used to determine the length of the
     * string. In all other cases the number of bytes that the c type uses
     * defines the length of the part. Then the length of one part is:
     * length = sizeof(value_type) + sizeof(c-type)
     * length = 1 + the number of bytes for a int32_t or double etc.
     *
     * \private
     */
    uint32_t    length;

    /**
     * \brief This value defines the value that this package contains.
     *
     * For non string types this also determines the length of the part.
     */
    uint8_t    value_type;
};

/**
 * \brief The class that contains the operation that can be done on a
 *        SeeMsgPart.
 *
 * @private
 */
struct _SeeMsgPartClass {

    SeeObjectClass parent_cls;

    /**
     * \brief initialize a newly allocated SeeMsgPart.
     *
     * @param [in,out] msg_part      the newly allocated part
     * @param [in]     msg_part_cls  the class of the SeeMsgPart.
     *
     * @return SEE_SUCCESS
     */
    int (*msg_part_init) (
        SeeMsgPart*            msg_part,
        const SeeMsgPartClass* msg_part_cls
        /* Put instance specific arguments here and remove this comment. */
        );

    /**
     * \brief Obtain the number of bytes this part requires in order to write it
     *        to a bytestream.
     *
     * @param [in]  part        A pointer to a valid SeeMsgPart
     * @param [out] length      The length of the part will be returned here.
     * @param [out] error_out   The returned error.
     *
     * @return SEE_SUCCESS or SEE_ERROR_RUNTIME with EINVAL when the mesgpart
     *         isn't properly initialized.
     * @private
     */
    int (*length) (
        const SeeMsgPart* part,
        uint32_t*         length,
        SeeError**        error_out
        );

    /**
     * \brief Obtain what payload this message contains, or what data type
     *        this can be used to extract the value the message contains.
     * @param part
     * @param type
     * @return
     */
    int (*value_type) (
        const SeeMsgPart* part,
        uint8_t*          type
        );

    /**
     * \brief Initialize this message part with a int32_t.
     *
     * @param [in,out] msg_part A pointer to a SeeMsgPart
     * @param [in]     value    The value to store within the message part.
     * @param [out]    error_out An error will be returned here when something
     *                           goes wrong
     * @return SEE_SUCCESS
     * @private
     */
    int (*write_int32) (
        SeeMsgPart*       msg_buf_p,
        int32_t           value,
        SeeError**        error_out
        );

    /**
     * \brief Obtain a uint32_t value from this part.
     *
     * @param [in]  msg_buf_p A pointer to SeeMsgPart that contains a uint32_t
     * @param [out] value_out The value will be returned here.
     * @param [out] error_out A SeeError will be returned here when something
     *                        goes wrong.
     *
     * @return SEE_SUCCESS, SEE_ERROR_MSG_PART_TYPE (when it doesn't contain a uint32_t)
     * @private
     */
    int (*get_int32) (
        const SeeMsgPart* msg_buf_p,
        int32_t*          value,
        SeeError**        error_out
        );

    /**
     * \brief Initialize this message part with a uint32_t.
     *
     * @param [in,out] msg_part A pointer to a SeeMsgPart
     * @param [in]     value    The value to store within the message part.
     * @param [out]    error_out An error will be returned here when something
     *                           goes wrong
     * @return SEE_SUCCESS
     * @private
     */
    int (*write_uint32) (
        SeeMsgPart*       msg_buf_p,
        uint32_t          value,
        SeeError**        error_out
        );

    /**
     * \brief Obtain a uint32_t value from this part.
     *
     * @param [in]  msg_buf_p A pointer to SeeMsgPart that contains a uint32_t
     * @param [out] value_out The value will be returned here.
     * @param [out] error_out A SeeError will be returned here when something
     *                        goes wrong.
     *
     * @return SEE_SUCCESS, SEE_ERROR_MSG_PART_TYPE (when it doesn't contain a uint32_t)
     * @private
     */
    int (*get_uint32) (
        const SeeMsgPart* msg_buf_p,
        uint32_t*         value_out,
        SeeError**        error_out
        );

    /**
     * \brief Initialize this message part with a int64_t.
     *
     * @param [in,out] msg_part A pointer to a SeeMsgPart
     * @param [in]     value    The value to store within the message part.
     * @param [out]    error_out An error will be returned here when something
     *                           goes wrong
     * @return SEE_SUCCESS
     * @private
     */
    int (*write_int64) (
        SeeMsgPart*       msg_buf_p,
        int64_t           value,
        SeeError**        error_out
        );

    /**
     * \brief Obtain a int64_t value from this part.
     *
     * @param [in]  msg_buf_p A pointer to SeeMsgPart that contains a int64_t
     * @param [out] value_out The value will be returned here.
     * @param [out] error_out A SeeError will be returned here when something
     *                        goes wrong.
     *
     * @return SEE_SUCCESS, SEE_ERROR_MSG_PART_TYPE (when it doesn't contain a int64_t)
     * @private
     */
    int (*get_int64) (
        const SeeMsgPart* msg_buf_p,
        int64_t*          value,
        SeeError**        error_out
        );

    /**
     * \brief Initialize this message part with a uint64_t.
     *
     * @param [in,out] msg_part A pointer to a SeeMsgPart
     * @param [in]     value    The value to store within the message part.
     * @param [out]    error_out An error will be returned here when something
     *                           goes wrong
     * @return SEE_SUCCESS
     * @private
     */
    int (*write_uint64) (
        SeeMsgPart*       msg_buf_p,
        uint64_t          value,
        SeeError**        error_out
        );

    /**
     * \brief Obtain a uint64_t value from this part.
     *
     * @param [in]  msg_buf_p A pointer to SeeMsgPart that contains a uint64_t
     * @param [out] value_out The value will be returned here.
     * @param [out] error_out A SeeError will be returned here when something
     *                        goes wrong.
     *
     * @return SEE_SUCCESS, SEE_ERROR_MSG_PART_TYPE (when it doesn't contain a uint64_t)
     * @private
     */
    int (*get_uint64) (
        const SeeMsgPart* msg_buf_p,
        uint64_t*         value_out,
        SeeError**        error_out
        );

    /**
     * \brief Initialize this message part with a string.
     *
     * @param [in,out] msg_part A pointer to a SeeMsgPart
     * @param [in]     value    The value to store within the message part.
     * @param [out]    error_out An error will be returned here when something
     *                           goes wrong
     * @return SEE_SUCCESS
     * @private
     */
    int (*write_string) (
        SeeMsgPart*       msg_buf_p,
        const char*       value,
        size_t            length,
        SeeError**        error_out
        );

    /**
     * \brief Obtain a string value from this part.
     *
     * @param [in]  msg_buf_p A pointer to SeeMsgPart that contains a string
     * @param [out] value_out The value will be returned here.
     * @param [out] error_out A SeeError will be returned here when something
     *                        goes wrong.
     *
     * @return SEE_SUCCESS, SEE_ERROR_MSG_PART_TYPE (when it doesn't contain a string)
     * @private
     */
    int (*get_string) (
        const SeeMsgPart* msg_buf_p,
        char**            value_out,
        SeeError**        error_out
        );

    /**
     * \brief Initialize this message part with a float.
     *
     * @param [in,out] msg_part A pointer to a SeeMsgPart
     * @param [in]     value    The value to store within the message part.
     * @param [out]    error_out An error will be returned here when something
     *                           goes wrong
     * @return SEE_SUCCESS
     * @private
     */
    int (*write_float) (
        SeeMsgPart*       msg_buf,
        float             value,
        SeeError**        error_out
        );

    /**
     * \brief Obtain a float value from this part.
     *
     * @param [in]  msg_buf_p A pointer to SeeMsgPart that contains a float
     * @param [out] value_out The value will be returned here.
     * @param [out] error_out A SeeError will be returned here when something
     *                        goes wrong.
     *
     * @return SEE_SUCCESS, SEE_ERROR_MSG_PART_TYPE (when it doesn't contain a float)
     * @private
     */
    int (*get_float) (
        const SeeMsgPart* msg_buf_p,
        float*            value_out,
        SeeError**        error_out
        );

    /**
     * \brief Initialize this message part with a double.
     *
     * @param [in,out] msg_part A pointer to a SeeMsgPart
     * @param [in]     value    The value to store within the message part.
     * @param [out]    error_out An error will be returned here when something
     *                           goes wrong
     * @return SEE_SUCCESS
     */
    int (*write_double) (
        SeeMsgPart*       msg_part,
        double            value,
        SeeError**        error_out
        );

    /**
     * \brief Obtain a double value from this part.
     *
     * @param [in]  msg_buf_p A pointer to SeeMsgPart that contains a double
     * @param [out] value_out The value will be returned here.
     * @param [out] error_out A SeeError will be returned here when something
     *                        goes wrong.
     *
     * @return SEE_SUCCESS, SEE_ERROR_MSG_PART_TYPE (when it doesn't contain a double)
     * @private
     */
    int (*get_double) (
        const SeeMsgPart* msg_buf_p,
        double*           value_out,
        SeeError**        error_out
        );

    /**
     * \brief Return the number of bytes required to transform this part to
     *        a bytestream.
     *
     * The SeeMsgPart should have a valid intialized msg type, otherwise
     * a runtime error with EINVAL will be returned.
     *
     * @param [in]  msg_buf_p A pointer to a valid msg_part
     * @param [out] size_out  The required size will be returned here.
     * @param [out] error_out A SeeError can be returned here when something
     *                        goes wrong
     * @return SEE_SUCCESS, SEE_ERROR_RUNTIME
     * @private
     */
    int (*buffer_length) (
        const SeeMsgPart* msg_buf_p,
        size_t*           size_out,
        SeeError**        error_out
        );

    /**
     * \brief Write a part to a bytestream.
     *
     * This method will transform a part to its bytestream representation.
     *
     * @param [in]      part    The part that must be transformed to its
     *                          bytestream representation.
     * @param [in, out] buffer  The buffer is initialized with bytes that
     *                          represent the part.
     * @param [out]     error_out If an error occurs a hopefully useful error
     *                            msg is returned here.
     * @return
     * @private
     */
    int (*write) (
        const SeeMsgPart* part,
        void*             buffer,
        SeeError**        error_out
        );

    /**
     * @brief Parse a new message from a bytestream
     *
     * Like SeeMsgBufferClass->read() is the inverse of SeeMsgPartClass->read()
     * is the inverse of the SeeMsgPartClass->write().
     *
     * @param [out] part_out        A newly created SeeMsgPart will be returned
     *                              here if this method is successful.
     * @param [in]  buffer          The part will be created from this buffer/
     *                              bytestream.
     * @param [in]  bufsiz          The size of the buffer, this must be large
     *                              enough to contain the entire SeeMsgPart
     * @param [out] part_size_out   The number of bytes used to construct the
     *                              SeeMsgPart
     * @param [out] error_out       If an error occurs during parsing a hopefully
     *                              useful error will be returned here.
     *
     * @return SEE_SUCCESS, SEE_ERROR_MSG_INVALID, SEE_ERROR_RUNTIME
     * @private
     */
    int (*read) (
        SeeMsgPart**      part_out,
        const void*       buffer,
        size_t            bufsiz,
        size_t*           part_size_out,
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
 * @brief Obtain the length in bytes of the message part
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
    uint32_t*         length,
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
    uint8_t*            val_type
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
    uint32_t          value,
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
 *
 * @param [in]  part      The part from which to obtain a message
 * @param [out] value     The value is returned here, free it after use.
 * @param [out] error_out If an error occurs info about is returned here.
 *
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
    float               value,
    SeeError**          error_out
    );

/**
 * @brief Obtain a floating point number from the part
 *
 * @param [in,out]  part        The part to which you would like to obtain the float.
 * @param [out]     value       The value is returned here
 * @param [out]     error_out   If an error occurs it is returned here.
 *
 * @return SEE_SUCCESS, SEE_INVALID_ARGUMENT
 */
SEE_EXPORT int
see_msg_part_get_float(
    const SeeMsgPart*   part,
    float*              value,
    SeeError**          error_out
    );

/**
 * @brief write a double precision floating point number to the part.
 *
 * @param [in,out]  part        The part to which you would like to write the double.
 * @param [in]      value       The value to write
 * @param [out]     error_out   If an error occurs it is returned here.
 *
 * @return SEE_SUCCESS, SEE_INVALID_ARGUMENT
 */
SEE_EXPORT int
see_msg_part_write_double(
    SeeMsgPart*         part,
    double              value,
    SeeError**          error_out
    );

/**
 * @brief Obtain a double precision floating point number from the part
 *
 * @param [in,out]  part        The part to which you would like to obtain the int.
 * @param [out]     value       The value is returned here
 * @param [out]     error_out   If an error occurs it is returned here.
 *
 * @return SEE_SUCCESS, SEE_INVALID_ARGUMENT
 */
SEE_EXPORT int
see_msg_part_get_double(
    const SeeMsgPart*   part,
    double*             value,
    SeeError**          error_out
    );

/**
 * @brief Determines the length that the SeeMsgPart requires for a data buffer
 *
 * This function can be used to compute the required size of a data buffer.
 *
 * @param [in]  part         The part whose necessary buffer size you would like
 *                           to know.
 * @param [out] length_out   The required size for the buffer is returned here
 *                           (the number of bytes).
 * @param [out] error_out    If an error occurs it is returned here.
 *
 * @return SEE_SUCCESS, SEE_INVALID_ARGUMENT, SEE_ERROR_RUNTIME
 */
SEE_EXPORT int
see_msg_part_buffer_length(
    const SeeMsgPart*   part,
    size_t*             length_out,
    SeeError**          error_out
    );

/**
 * @brief Write a SeeMsgPart to a memory buffer
 *
 * @param [in]  part      The part that you want to put in a buffer
 * @param [out] buffer    The buffer wherein you want to put the SeeMsgPart,
 *                        it is up to the caller to make sure the buffer is
 *                        large enough.
 * @param [out] error_out Error info is returned here on error.
 *
 * @return SEE_SUCCESS, SEE_INVALID_ARGUMENT, SEE_ERROR_RUNTIME
 */
SEE_EXPORT int
see_msg_part_write (
    const SeeMsgPart*   part,
    void*               buffer,
    SeeError**          error_out
    );

/**
 * @brief Read a SeeMsgPart from a memory buffer.
 *
 * This function can be seen as a static class function that initializes/creates
 * a new SeeMsgPart instance. In principle, this is the inverse way to obtain
 * a message part from see_msg_part_new(). In the latter function you define
 * a message part via its type and its data. The msg part can be send via a
 * bus in a binaray serialized fashion. This method reads the buffer across the
 * bus and recreate the SeeMsgPart as it was created at the sender side of the
 * bus.
 *
 * @param [out] part_out        The newly created part will be returned here.
 * @param [in]  buffer          The buffer that describes the part.
 * @param [in]  bufsiz          The maximum size of the buffer
 *                              if more bytes are neccessary to create the
 *                              SeeMsgPart SEE_ERROR_MSG_INVALID will be
 *                              returned
 * @param [out] part_size_out   Returns the number of bytes that were read in
 *                              order to parse the msg part.
 * @param [out] error_out       If an error occurs it can be returned here.
 *
 * @return SEE_SUCCESS, SEE_INVALID_ARGUMENT, SEE_MSG_INVALID, SEE_ERROR_RUNTIME
 */
SEE_EXPORT int
see_msg_part_read (
    SeeMsgPart** part_out,
    const void*  buffer,
    size_t       bufsiz,
    size_t*      part_size_out,
    SeeError**   error_out
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

/**
 * \brief represents a buffer into which SeeMsgParts can be added.
 *
 * The buffer contains an id. The id can be used to describe a message
 * The message conveys 0 or any number of parts. The receiver can use the
 * id of the msg to know which messages should be available.
 */
struct _SeeMsgBuffer {

    /** It's a SeeObject */
    SeeObject parent_obj;

    /**
     * \brief The SeeMsgParts stored in the parts member
     *
     * A message can consist of multiple parts, all the parts are put into
     * the buffer in such a way that the remote end is able to tell which
     * parts are embedded in the message.
     *
     * \private
     */
    SeeDynamicArray*    parts;

    /**
     * \brief The id is used by the receiver to determine what type of message is
     * received.
     */
    uint16_t            id;
};

/**
 * \brief Represents a the class of a SeeMsgBuffer
 *
 * \private
 */
struct _SeeMsgBufferClass {

    /**
     * \brief SeeMsgBufferClass inherits from SeeObjectClass.
     *
     * \private
     */
    SeeObjectClass parent_cls;

    /**
     * \brief Every message send will start with this msg.
     */
    const char* msg_start;

    /**
     * \brief Initialize a freshly allocated SeeMsgBuffer.
     *
     * @param [in]  msg_buffer      A newly allocated msg buffer
     * @param [in]  msg_buffer_cls  The class this object is an instance of.
     * @param [in]  msg_type        The id that the receiver can use
     *                              to determine what message is being send.
     * @param [out] error_out       If an error occurs, useful info
     *                              can be returned here.
     * @return
     */
    int (*msg_buffer_init)(
        SeeMsgBuffer*            msg_buffer,
        const SeeMsgBufferClass* msg_buffer_cls,
        uint16_t                 msg_type,
        SeeError**               error_out
        );

    /**
     * \brief Reset the id of the message.
     *
     * @param [in,out] msg      Reset the id of this message
     * @param [in]     msg_type The new id.
     * @return
     * \private
     */
    int (*set_id) (
        SeeMsgBuffer*       msg,
        uint16_t            msg_type
        );

    /**
     * \brief obtain the id of the message
     *
     * @param [in]  msg         The message whose id you would like to obtain.
     * @param [out] msg_type    The id of the message is returned here.
     * @return
     * \private
     */
    int (*get_id) (
        const SeeMsgBuffer* msg,
        uint16_t*           msg_type
        );

    /**
     * \brief Obtain the length in bytes that a buffer requires to transmit
     * the message.
     *
     * @param [in]  msg     The message whose bufferlength you would lik to know
     * @param [out] out     The length of the buffer is returned here.
     * @param [out] error   If an error occurs it is returned here.
     * @return
     *
     * \private
     */
    int (*length) (
        SeeMsgBuffer*       msg,
        uint32_t*           out,
        SeeError**          error
        );

    /**
     * \brief   Add a new SeeMsgPart to the buffer.
     * @param [in]  msg  if an error occurs it will be returned here.
     * @param [in]  part The part you would like to add to the buffer. This
     *                   feeds a reference, so this method will call
                         see_object_ref on the part.
     * @param [out] error If an error occurs it will be returned here.
     * @return SEE_SUCCESS, SEE_ERROR_RUNTIME
     *
     * \private
     */
    int (*add_part) (
        SeeMsgBuffer*       msg,
        SeeMsgPart*         part,
        SeeError**          error
        );

    /**
     * \brief Obtain a SeeMsgPart from the message.
     *
     * @param [in]  msg
     * @param [in]  index
     * @param [out] part_out
     * @param [out] error_out
     *
     * @return SEE_SUCCESS, SEE_ERROR_INDEX
     */
    int (*get_part) (
        SeeMsgBuffer*       msg,
        size_t              index,
        SeeMsgPart**        part_out,
        SeeError**          error_out
        );

    /**
     * \brief Obtain the number of parts.
     *
     * This method obtains the number of parts available in the buffer.
     * If you use an index greater or equal to the result of this function,
     * you shall be served a SeeIndexError and SEE_ERROR_INDEX from the
     * get_part function.
     *
     * @param [in]  msg  The SeeMsgBuffer whose number of part you want to know
     * @param [out] size The number of parts is returned here.
     * @return
     */
    int (*num_parts) (
        const SeeMsgBuffer* msg,
        size_t*             size
        );

    /**
     * \brief Obtain a buffer that precisely contains 1 the bytes that represent
     * one SeeMsgBuffer.
     *
     * @param [in]  msg         The msg to be turned into a bytestream
     * @param [out] buffer_out  The returned bytestream
     * @param [out] bufsize_out The size of the buffer.
     * @param [out] error_out   If an error occurs it will be returned here.
     *
     * @return
     */
    int (*get_buffer) (
        SeeMsgBuffer*       msg,
        void**              buffer_out,
        size_t*             bufsize_out,
        SeeError**          error_out
        );

    /**
     * \brief construct a SeeMessageBuffer from a bytestream.
     *
     * This method is the inverse from the get_buffer method. With one major
     * difference, this method should be considered a static classmethod.
     * whether the get_buffer is an instance_method.
     *
     * @param [out] msg_out   If this points to a previous initialized SeeMsgBuffer
     *                        the reference count to that object will be decrememted
     *                        and a new object will be returned.
     * @param [in]  buffer    A pointer to the bytestream
     * @param [in]  buf_size  The size of the buffer, this is use to determine
     *                        whether the size is appropriat or more bytes need
     *                        to be aquired.
     * @param [out] error_out If an error occurs it will be returned here.
     * @return
     */
    int (*from_buffer) (
        SeeMsgBuffer**  msg_out,
        const void*     buffer,
        size_t          buf_size,
        SeeError**      error_out
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
 * @brief Create a new message buffer.
 *
 * @param [out] buf_out   If successful a new buffer is returned here.
 * @param [in]  id        The id that identifies the message.
 * @param [out] error_out If unsuccessful a error might be returned here.
 *
 * @return SEE_SUCCESS, SEE_NOT_INITIALISED, SEE_INVALID_ARGUMENT
 */
SEE_EXPORT int
see_msg_buffer_new(
    SeeMsgBuffer**  buf_out,
    uint16_t        id,
    SeeError**      error_out
    );

/**
 * \brief Set the message id of an SeeMsgBuffer.
 *
 * Typically you already specify it when constructing a new SeeMsgBuffer,
 * but one can set the identifier with this function
 *
 * @param [in,out] msg          The message whose id you want to set.
 * @param [in]     id           The new id for the message
 * @param [out]    error_out    Error are returned here.
 *
 * @return SEE_SUCCESS, SEE_INVALID_ARGUMENT
 */
SEE_EXPORT int
see_msg_buffer_set_type(
    SeeMsgBuffer*   msg,
    uint16_t        id,
    SeeError**      error_out
    );

/**
 * \brief Set the message id of a SeeMsgBuffer.
 *
 * @param [in, out] msg The SeeMsgError whose id you want to set.
 * @param [in]      id  The new id.
 *
 * @return SEE_SUCCESS
 */
SEE_EXPORT int
see_msg_buffer_set_id(
    SeeMsgBuffer*   msg,
    uint16_t        id
    );

/**
 * \brief Obtain the id of a message.
 *
 * This id tells what message a sender is trying to get across. So once
 * the id is know, the receiver can use the id to determine which parts will
 * be available in the message and/or which message the sender is trying to
 * get across. The meaning of the id is not determined by the see-objects
 * library, but rather by user.
 *
 * @param [in]      msg         The msg whose id you would like to know
 * @param [out]     id_out      The id is returned here
 *
 * @return SEE_SUCCESS, SEE_INVALID_ARGUMENT
 */
SEE_EXPORT int
see_msg_buffer_get_id (
    const SeeMsgBuffer*   msg,
    uint16_t*             id_out
    );

/**
 * \brief add a MsgPart to the MsgBuffer
 *
 * @param [in, out]     msg
 * @param [in]          part
 * @param [out]         error_out
 *
 * @return SEE_SUCCESS, SEE_INVALID_ARGUMENT, SEE_RUNTIME_ERROR
 */
SEE_EXPORT int
see_msg_buffer_add_part(
    SeeMsgBuffer*       msg,
    SeeMsgPart*         part,
    SeeError**          error_out
    );

/**
 * \brief Obtain a part from the message buffer.
 *
 * @param [in]  msg     The message from whom you would like to receive a part.
 * @param [in]  index   The index of the desired part, make sure it is not to
 *                      large. Or you'll be served by an SEE_ERROR_INDEX.
 * @param [out] part    The part will be returned here, if *part already points
 *                      to a valid part, it will be decreffed and it will point
 *                      to a new instance. The instance returned will already
 *                      be increffed, hence the user should decref this part
 *                      when finished.
 * @param [out] error   If an error occurs, the an SeeError object might be
 *                      returned here to provide context about the error.
 *
 * @return SEE_SUCCESS, SEE_INVALID_ARGUMENT, SEE_ERROR_INDEX
 */
SEE_EXPORT int
see_msg_buffer_get_part(
    SeeMsgBuffer*   msg,
    size_t          index,
    SeeMsgPart**    part,
    SeeError**      error
    );

/**
 * \brief obtain the number of parts in the msg.
 *
 * @param [in]  msg
 * @param [out] size_out
 * @return SEE_SUCCESS, SEE_INVALID_ARGUMENT
 */
SEE_EXPORT int
see_msg_buffer_num_parts(
    const SeeMsgBuffer* msg,
    size_t*             size_out
    );

/**
 * \brief Construct a byte buffer that can be send over a wire.
 *
 * This method constructs a bytestream that can be send over a wire/bus
 * The size_out argument contains the size of the message. The buffer
 * that is returned represent precisely 1 SeeBufferMsg.
 *
 * @param [in]  msg         The message to convey.
 * @param [out] buffer_out  The bytestream that represents 1 SeeBufferMsg
 * @param [out] size_out    The number of bytes the stream is long.
 * @param [out] error_out   If an error occurs return it here.
 *
 * @return SEE_SUCCESS, SEE_INVALID_ARGUMENT, SEE_ERROR_RUNTIME
 */
SEE_EXPORT int
see_msg_buffer_get_buffer(
    SeeMsgBuffer*   msg,
    void**          buffer_out,
    size_t*         size_out,
    SeeError**      error_out
    );

/**
 * \brief Construct a new message from a buffer.
 *
 * \note Consider this function to be a static class method.
 *
 * @param [out] msg         If there was a valid message at buffer it will
 *                          be returned here. If *msg is not NULL, this function
 *                          will assume, there is a message there and will
 *                          decrement the reference count of *msg and return
 *                          a new one.
 * @param [in]  buffer      The buffer from which a msg part can be constructed.
 * @param [out] size        the size of the buffer, this must be large enough
 *                          to parse an entire buffer from the bytestream.
 * @param [out] error_out   If there is something wrong an SeeError might
 *                          be returned here, from which useful info might
 *                          be gathered.
 *
 * @return SEE_SUCCESS, SEE_INVALID_ARGUMENT, SEE_ERROR_RUNTIME
 */
SEE_EXPORT int
see_msg_buffer_from_buffer(
    SeeMsgBuffer**  msg,
    const void*     buffer,
    size_t          size,
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

/**
 * \brief SeeMsgPartTypeError is an error that is raise when trying to
 * parse a value from at MsgPart that contains another type.
 *
 * \private
 */
struct _SeeMsgPartTypeError {
    /**
     * \brief This object inherits from SeeError
     */
    SeeError parent_obj;
    /*expand SeeMsgPartTypeError data here*/

};

/**
 * \brief A class that belongs To SeeMsgPartTypeError's.
 *
 * \private
 */
struct _SeeMsgPartTypeErrorClass {
    /**
     * \brief This class inherits from SeeErrorClass.
     * \private
     */
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

/**
 * \brief Allocate a new SeeMsgPartTypeError
 *
 * @param [out] error    A new error is returned here.
 * @param [in]  expected The datatype the operation expected.
 * @param [in]  asked    The datatype the caller asked form
 *
 * @return SEE_SUCCESS
 */
SEE_EXPORT int
see_msg_part_type_error_new(
    SeeError**              error,
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

/* ********************************* */
/* **** Message Invalid Error   **** */
/* ********************************* */

typedef struct _SeeMsgInvalidError SeeMsgInvalidError;
typedef struct _SeeMsgInvalidErrorClass SeeMsgInvalidErrorClass;


/**
 * \brief This instances can be returned when there is something wrong with
 *        the construction of a SeeMsgPart or SeeMsgBuffer from a bytestream.
 *
 * This error is the result when you obtain a bytestream to construct a new
 * SeeMsgBuffer from a received buffer over a line. If the buffer isn't
 * correctly formatted or to short for the message it contains, this error
 * is returned.
 */
struct _SeeMsgInvalidError {

    /**
     * parent is SeeError
     * @private
     */
    SeeError parent_obj;

};

/**
 * @brief SeeMsgInvalidErrorClass inherits from SeeErrorClass.
 */
struct _SeeMsgInvalidErrorClass {

    /**
     * @brief inherit from SeeErrorClass
     *
     * @private
     */
    SeeErrorClass parent_cls;

    /**
     * \brief Initialize a new SeeMsgInvalidError
     * @param msg_invalid_error
     * @param msg_invalid_error_cls
     * @return
     */
    int (*msg_invalid_error_init)(
        SeeMsgInvalidError*            msg_invalid_error,
        const SeeMsgInvalidErrorClass* msg_invalid_error_cls
        );

};

/* **** function style macro casts **** */

/**
 * \brief cast a pointer from a SeeMsgInvalidError derived instance back to a
 *        pointer to SeeMsgInvalidError.
 */
#define SEE_MSG_INVALID_ERROR(obj)                      \
    ((SeeMsgInvalidError*) obj)

/**
 * \brief cast a pointer to pointer from a SeeMsgInvalidError derived instance back to a
 *        reference to SeeMsgInvalidError*.
 */
#define SEE_MSG_INVALID_ERROR_REF(ref)                      \
    ((SeeMsgInvalidError**) ref)

/**
 * \brief cast a pointer to SeeMsgInvalidErrorClass derived class back to a
 *        pointer to SeeMsgInvalidErrorClass.
 */
#define SEE_MSG_INVALID_ERROR_CLASS(cls)                      \
    ((const SeeMsgInvalidErrorClass*) cls)

/**
 * \brief obtain a pointer to SeeMsgInvalidErrorClass from a instance of
 *        derived from SeeMsgInvalidError. This macro is preferably
 *        used when obtaining the class of a instance. When this
 *        macro is used. Calling methods on the class will enable
 *        polymorphism, because you'll get the derived class.
 */
#define SEE_MSG_INVALID_ERROR_GET_CLASS(obj)                \
    (SEE_MSG_INVALID_ERROR_CLASS(see_object_get_class(SEE_OBJECT(obj)) )  )

/* **** public functions **** */

/**
 * \brief Return a new SeeInvalidMsgError
 *
 * @param [out] error The newly generated error is returned here.
 *
 * @return SEE_SUCCESS
 */
SEE_EXPORT int
see_msg_invalid_error_new(
    SeeError**   error
    );

/**
 * Gets the pointer to the SeeMsgInvalidErrorClass table.
 */
SEE_EXPORT const SeeMsgInvalidErrorClass*
see_msg_invalid_error_class();

/* Expand the class with public functions here, don't forget the SEE_EXPORT
 * macro, because otherwise you'll run into troubles when exporting function
 * in a windows dll.
 */

/* **** class initialization functions **** */

/**
 * Initialize SeeMsgInvalidError; make it ready for use.
 */
SEE_EXPORT
int see_msg_invalid_error_init();

/**
 * Deinitialize SeeMsgInvalidError, after SeeMsgInvalidError has been deinitialized,
 * all functions in this header shouldn't be used anymore.
 */
SEE_EXPORT
void see_msg_invalid_error_deinit();
#ifdef __cplusplus
}
#endif

#endif //ifndef SEE_MSG_BUFFER_H
