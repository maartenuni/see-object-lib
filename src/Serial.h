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
 * \file Serial.h Export the API of a serial device.
 */

#ifndef SEE_SERIAL_H
#define SEE_SERIAL_H

#include "SeeObject.h"
#include "Error.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum see_serial_dir{
    SEE_SERIAL_INPUT    = 1u << 0u,
    SEE_SERIAL_OUTPUT   = 1u << 1u,
    SEE_SERIAL_INOUT     = SEE_SERIAL_INPUT | SEE_SERIAL_OUTPUT
}see_serial_dir_t;

/**
 * \brief The predefined baudrates of the connection
 */
typedef enum see_speed {
    SEE_B0,         /** terminates the connection */
    SEE_B50,
    SEE_B75,
    SEE_B110,
    SEE_B134,
    SEE_B150,
    SEE_B200,
    SEE_B300,
    SEE_B600,
    SEE_B1200,
    SEE_B1800,
    SEE_B2400,
    SEE_B4800,
    SEE_B9600,
    SEE_B19200,
    SEE_B38400,
    SEE_B57600,
    SEE_B115200,
    SEE_B230400
}see_speed_t;

typedef struct _SeeSerial SeeSerial;
typedef struct _SeeSerialClass SeeSerialClass;

struct _SeeSerial {
    SeeObject parent_obj;
};

struct _SeeSerialClass {
    SeeObjectClass parent_cls;

    /**
     * @brief Initializes the serial class.
     *
     * @param [in]  serial      The serial connection to initialize
     * @param [in]  serial_cls  The class, likely a derived class
     * @param [in]  device_fn   may be null, when specified the device may
     *                          be opened directly.
     * @param [out] error_out   If an error occurs, a handy output function
     *                          may be returned here.
     * @return SEE_SUCCESS, SEE_INVALID_ARUGMENT, SEE_ERROR_RUNTIME
     * @private
     */
    int (*serial_init)(
        SeeSerial*            serial,
        const SeeSerialClass* serial_cls,
        const char*           device_fn,
        SeeError**            error_out
        /* Put instance specific arguments here and remove this comment. */
        );

    /**
     * @brief Opens the device, once it is opened, settings may be altered
     *        and one can read from or write to the device.
     * @param [in] self The device to open
     * @param [in] dev File/device to openen eg /dev/tty1 or COM1
     * @param [out] error_out If an error occurs a hopefully useful message
     *              will be returned here.
     * @return SEE_SUCCESS, SEE_INVALID_ARGUMENT, SEE_ERROR_RUNTIME
     * @private
     */
    int (*open) (
        SeeSerial*  self,
        const char* dev,
        SeeError**  error_out
        );

    /**
     * \brief closes the serial connection if it is open, otherwise does nothing
     *
     * This function closes a connection when it is opened, otherwise it does
     * nothing. Closing the device also frees the resources linked to it
     * like the file that is opened.
     *
     * @param [in] self The device to close.
     * @param [out] error_out Errors are returned here.
     * @return SEE_SUCCESS, SEE_INVALID_ARGUMENT
     * @private
     */
    int (*close)(SeeSerial* self, SeeError** error_out);

    /**
     * @brief Write to the device, make sure the device is opened and in the
     *        right settings first .
     * @param [in]      self        The device to write to
     * @param [in]      bytes       A pointer to the bytes that should be written to
     *                              over the serial connection.
     * @param [in,out]  length      The length of the bytes buffer. The number
     *                              of bytes written will be returned here.
     * @param [out]     error_out   If an error occurs a hopefully useful message
     *                              will be returned here.
     * @return SEE_SUCCESS, SEE_INVALID_ARGUMENT, SEE_ERROR_RUNTIME
     * @private
     */
    int (*write) (
        const SeeSerial*    self,
        const void*         bytes,
        size_t*             length,
        SeeError**          error_out
        );

    /**
     * @brief Read from the device, make sure the device is opened and in the
     *        right settings first .
     * @param [in]      self The device to read from
     * @param [in]      bytes A pointer to the bytes where the read bytes
     *                        can be stored, make sure the buffer is large
     *                        enough.
     * @param [in,out]  length The length of the bytes buffer. The number of
     *                         bytes read will be returned here.
     * @param [out]     error_out If an error occurs a hopefully useful message
     *                            will be returned here.
     * @return SEE_SUCCESS, SEE_INVALID_ARGUMENT, SEE_ERROR_RUNTIME
     * @private
     */
    int (*read) (
        const SeeSerial*    self,
        void*               buffer,
        size_t*             length,
        SeeError**          error_out
        );

    /**
     * @brief Flush the in or output queue of the serial connection
     *
     * Flush discards the data inside of the queue.
     *
     * @param [in]  self    The serial device to flush
     * @param [out] q       Whether the input or output
     * @param [out] error_out If an error occurs it will be returned here
     * @return SEE_SUCCESS, SEE_INVALID_ARGUMENT, SEE_RUNTIME_ERROR
     * @private
     */
    int (*flush) (
        const SeeSerial*    self,
        see_serial_dir_t    q,
        SeeError**          error_out
        );

    /**
     * @brief Drain nicely waits until all data has been written.
     * @param [in]  self      Wait for this serial device to be drained.
     * @param [out] error_out Errors are returned here.
     * @private
     * @return SEE_SUCCESS, SEE_INVALID_ARGUMENT, SEE_RUNTIME_ERROR
     */
    int (*drain) (
        const SeeSerial*    self,
        SeeError**          error_out
        );

    /**
     * @brief Try to set the speed of the device
     *
     * @private
     * @param [in] self  Set the input or output speed on the device.
     * @param [in] dir   Set the in or output speed.
     * @param [in] speed The desired speed.
     * @param error_out  Errors are returned here.
     * @return
     */
    int (*set_speed) (
        SeeSerial*          self,
        see_serial_dir_t    dir,
        see_speed_t         speed,
        SeeError**          error_out
        );

    /**
     * @brief get the speed from the input or output direction
     *
     * @param [in] self The device from which we would like to obtain the speed.
     * @param [in] dir  Should be SEE_DIRECTION_INPUT or OUTPUT, because in or
     *                  output might not yield the same result.
     * @param [out] speed The speed will be returned here.
     * @param error_out
     * @private
     * @return
     */
    int (*get_speed) (
        SeeSerial*          self,
        see_serial_dir_t    dir,
        see_speed_t*        speed,
        SeeError**          error_out
        );

    /**
     * @brief Check whether the device has been opened.
     * @param [in] self  The device from which you would like to know whether it
     *                   has been opened.
     * @param [out] result at *out will be stored a non zero value when the
     *                   device is open.
     * @private
     * @return SEE_SUCCESS SEE_INVALID_ARGUMENT
     */
    int (*is_open) (
        const SeeSerial*    self,
        int*                result
        );

};

/* **** function style macro casts **** */

/**
 * \brief cast a pointer from a SeeSerial derived instance back to a
 *        pointer to SeeSerial.
 */
#define SEE_SERIAL(obj)                      \
    ((SeeSerial*) obj)

/**
 * \brief cast a pointer to pointer from a SeeSerial derived instance back to a
 *        reference to SeeSerial*.
 */
#define SEE_SERIAL_REF(ref)                      \
    ((SeeSerial**) ref)

/**
 * \brief cast a pointer to SeeSerialClass derived class back to a
 *        pointer to SeeSerialClass.
 */
#define SEE_SERIAL_CLASS(cls)                      \
    ((const SeeSerialClass*) cls)

/**
 * \brief obtain a pointer to SeeSerialClass from a instance of
 *        derived from SeeSerial. This macro is preferably
 *        used when obtaining the class of a instance. When this
 *        macro is used. Calling methods on the class will enable
 *        polymorphism, because you'll get the derived class.
 */
#define SEE_SERIAL_GET_CLASS(obj)                \
    (SEE_SERIAL_CLASS(see_object_get_class(SEE_OBJECT(obj)) )  )

/* **** public functions **** */

/**
 * \brief Create a new serial device.
 *
 * Currently the this device is mainly intended for
 * non canonical raw serial communication.
 *
 * @param [out] serial    serial must not be NULL, whereas *serial should.
 * @param [out] error_out error_ot must not be NULL, whereas *error_out should.
 * @return SEE_SUCCESS
 */
SEE_EXPORT int
see_serial_new(
    SeeSerial**  serial,
    SeeError**   error_out
    );

/**
 * \brief Open a serial device
 *
 * @param [in]  self The serial device to open
 * @param [in]  dev The name of the serial device "/dev/tty1" or "COM1"
 * @param [out] error_out if there is an error opening the device it is
 *              returned here.
 * @return  SEE_SUCCESS, SEE_INVALID_ARGUMENT, SEE_RUNTIME_ERROR
 */
SEE_EXPORT int
see_serial_open(
    const SeeSerial*    self,
    const char*         dev,
    SeeError**          error_out
    );

/**
 * \brief Write a number of bytes to the serial device.
 *
 * @param [in] self     The serial device to which you would like to write.
 * @param [in] bytes    a pointer to the bytes you want to write.
 * @param [in] length   The number of bytes you want to write.
 * @param error_out     If an error occurs it will be returned here.
 *
 * @return  SEE_SUCCESS, SEE_RUNTIME_ERROR
 */
SEE_EXPORT int
see_serial_write (
    const SeeSerial*    self,
    const void*         bytes,
    size_t              length,
    SeeError**          error_out
    );

/**
 * \brief Read a number of bytes from the serial device.
 *
 * @param [in] self     The serial device from which you would like to read.
 * @param [in] bytes    a pointer to the bytes you want to write.
 * @param [in] length   The number of bytes you want to write.
 * @param error_out     If an error occurs it will be returned here.
 *
 * @return  SEE_SUCCESS, SEE_INVALID_ARGUMENT, SEE_RUNTIME_ERROR,
 */
SEE_EXPORT int
see_serial_read (
    const SeeSerial*    self,
    void*               buffer,
    size_t              length
    );

/**
 * \brief flush the serial device
 * @param [in]  self The serial device to flush
 * @param [in]  direction whether you want to flush the in-, output or both
 * @return  SEE_SUCCESS, SEE_RUNTIME_ERROR
 */
SEE_EXPORT int
see_serial_flush (
    const SeeSerial*    self,
    see_serial_dir_t    direction,
    SeeError**          error_out
    );

/**
 * \brief Set the desired speed of the device.
 * @param [in]  self The serial device to modify
 * @param [in]  direction whether you want to flush the in-, output or both
 * @param [in]  speed, the speed at which the device is desired to operate.
 *
 * @return  SEE_SUCCESS, SEE_RUNTIME_ERROR
 */
SEE_EXPORT int
see_serial_set_speed (
    SeeSerial*          self,
    see_serial_dir_t    dir,
    see_speed_t         speed,
    SeeError**          error_out
    );

/**
 * \brief Get the speed at which the device operates.
 *
 * @param self
 * @param dir
 * @param [out] speed
 * @param error_out
 *
 * @return  SEE_SUCCESS, SEE_RUNTIME_ERROR
 */
SEE_EXPORT int
see_serial_get_speed (
    SeeSerial*          self,
    see_serial_dir_t    dir,
    see_speed_t*        speed,
    SeeError**          error_out
    );

/**
 * @brief Check whether the device is successfully opened.
 * @param [in] self The device of which you would like to know whether it
 *              is open
 * @param [out] result The result will be posted here.
 * @return SEE_SUCCESS, SEE_INVALID_ARGUMENT
 */
SEE_EXPORT int
see_serial_is_open(
    const SeeSerial*    self,
    int*                result
    );

/**
 * Gets the pointer to the SeeSerialClass table.
 */
SEE_EXPORT const SeeSerialClass*
see_serial_class();

/* Expand the class with public functions here, don't forget the SEE_EXPORT
 * macro, because otherwise you'll run into troubles when exporting function
 * in a windows dll.
 */

/* **** class initialization functions **** */

/**
 * Initialize SeeSerial; make it ready for use.
 */
SEE_EXPORT
int see_serial_init();

/**
 * Deinitialize SeeSerial, after SeeSerial has been deinitialized,
 * all functions in this header shouldn't be used anymore.
 */
SEE_EXPORT
void see_serial_deinit();

#ifdef __cplusplus
}
#endif

#endif //ifndef SEE_SERIAL_H
