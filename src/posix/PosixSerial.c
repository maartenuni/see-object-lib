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

#include "see_object_config.h"

// Only compile on for a POSIX platform
#if defined(HAVE_UNISTD_H) && defined(HAVE_TERMIOS_H)

#include "../MetaClass.h"
#include "PosixSerial.h"
#include "../RuntimeError.h"
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <assert.h>

static int
get_posix_speed(see_speed_t s)
{
    switch(s) {
        case SEE_B0: return B0;
        case SEE_B50: return B50;
        case SEE_B75: return B75;
        case SEE_B110: return B110;
        case SEE_B134: return B134;
        case SEE_B150: return B150;
        case SEE_B200: return B200;
        case SEE_B300: return B300;
        case SEE_B600: return B600;
        case SEE_B1200: return B1200;
        case SEE_B1800: return B1800;
        case SEE_B2400: return B2400;
        case SEE_B4800: return B4800;
        case SEE_B9600: return B9600;
        case SEE_B19200: return B19200;
        case SEE_B38400: return B38400;
        case SEE_B57600: return B57600;
        case SEE_B115200: return B115200;
        case SEE_B230400: return B230400;
        default: assert(0 == 1);
    }
}

static see_speed_t
get_see_speed(speed_t s)
{
    switch (s) {
        case B0:
            return SEE_B0;
        case B50:
            return SEE_B50;
        case B75:
            return SEE_B75;
        case B110:
            return SEE_B110;
        case B134:
            return SEE_B134;
        case B150:
            return SEE_B150;
        case B200:
            return SEE_B200;
        case B300:
            return SEE_B300;
        case B600:
            return SEE_B600;
        case B1200:
            return SEE_B1200;
        case B1800:
            return SEE_B1800;
        case B2400:
            return SEE_B2400;
        case B4800:
            return SEE_B4800;
        case B9600:
            return SEE_B9600;
        case B19200:
            return SEE_B19200;
        case B38400:
            return SEE_B38400;
        case B57600:
            return SEE_B57600;
        case B115200:
            return SEE_B115200;
        case B230400:
            return SEE_B230400;
        default:
            assert(0 == 1);
    }
}



/* **** functions that implement SeePosixSerial or override SeeSerial **** */

static int
posix_serial_init(
    SeePosixSerial*             posix_serial,
    const SeePosixSerialClass*  posix_serial_cls,
    const char*                 device_fn,
    SeeError**                  error
    )
{
    int ret = SEE_SUCCESS;
    const SeeSerialClass* parent_cls = SEE_SERIAL_GET_CLASS(
        posix_serial
        );

    posix_serial->fd = -1;
    ret = parent_cls->serial_init(
        SEE_SERIAL(posix_serial),
        SEE_SERIAL_CLASS(posix_serial_cls),
        device_fn,
        error
        );

    return ret;
}

static int
init(const SeeObjectClass* cls, SeeObject* obj, va_list args)
{
    const SeePosixSerialClass* posix_serial_cls = SEE_POSIX_SERIAL_CLASS(cls);
    SeePosixSerial* posix_serial = SEE_POSIX_SERIAL(obj);

    /*Extract parameters here from va_list args here.*/
    const char* dev         = va_arg(args, const char*);
    SeeError**  error_out   = va_arg(args, SeeError**);

    return posix_serial_cls->posix_serial_init(
        posix_serial,
        posix_serial_cls,
        dev,
        error_out
        );
}

static int
posix_serial_open(SeeSerial* self, const char* dev, SeeError** error_out)
{
    struct termios out_settings;

    SeePosixSerial* pself = SEE_POSIX_SERIAL(self);
    pself->fd = open(dev, O_RDWR | O_NOCTTY);
    if (pself->fd < 0) {
        see_runtime_error_create(error_out, errno);
        return SEE_ERROR_RUNTIME;
    }

    if (tcgetattr(pself->fd, &pself->settings) == -1) {
        see_runtime_error_create(error_out, errno);
        return SEE_ERROR_RUNTIME;
    }

    memcpy(&out_settings, &pself->settings, sizeof(struct termios));
    cfmakeraw(&out_settings);
    if (tcsetattr(pself->fd, TCSANOW, &out_settings) == -1)  {
        see_runtime_error_create(error_out, errno);
        return SEE_ERROR_RUNTIME;
    }

    tcdrain(pself->fd);

    if (tcgetattr(pself->fd, &pself->settings) == -1)  {
        see_runtime_error_create(error_out, errno);
        return SEE_ERROR_RUNTIME;
    }

    return SEE_SUCCESS;
}

static int
posix_serial_close(SeeSerial* self, SeeError** error_out)
{
    const SeeSerialClass* cls = SEE_SERIAL_CLASS(
        see_object_get_class(SEE_OBJECT(self))
        );
    SeePosixSerial* pself = SEE_POSIX_SERIAL(self);
    struct termios new_settings;
    int open;
    cls->is_open(self, &open);

    if (!open)
        return SEE_SUCCESS;

    // Hangup by setting baudrate to 0.
    memcpy(&new_settings, &pself->settings, sizeof(new_settings));
    cfsetispeed(&new_settings, B0);
    cfsetospeed(&new_settings, B0);
    if (tcsetattr(pself->fd, TCSANOW, &new_settings) == -1) {
        int oops = 1;
        assert(oops != 1);
    }

    if (close(pself->fd)) {
        see_runtime_error_create(error_out, errno);
        return SEE_ERROR_RUNTIME;
    }
    pself->fd = -1;

    return SEE_SUCCESS;
}

static int
posix_serial_write(
    const SeeSerial* self,
    const void* bytes,
    size_t* length,
    SeeError** error_out
    )
{
    const SeeSerialClass* cls = SEE_SERIAL_CLASS(
        see_object_get_class(SEE_OBJECT(self))
    );
    SeePosixSerial* pself = SEE_POSIX_SERIAL(self);
    int open;
    cls->is_open(self, &open);
    if (!open) {
        //TODO make special exception to handle this
        ;
    }

    ssize_t nwritten = write(pself->fd, bytes, *length);
    if (nwritten < 0) {
        see_runtime_error_create(error_out, errno);
        return SEE_ERROR_RUNTIME;
    }
    *length = (size_t) nwritten;
    return SEE_SUCCESS;
}

static int
posix_serial_read(
    const SeeSerial* self,
    void* buffer,
    size_t* length,
    SeeError** error_out
    )
{
    const SeeSerialClass* cls = SEE_SERIAL_CLASS(
        see_object_get_class(SEE_OBJECT(self))
    );
    SeePosixSerial* pself = SEE_POSIX_SERIAL(self);
    int open;
    cls->is_open(self, &open);
    if (!open) {
        //TODO make special exception to handle this
        ;
    }

    ssize_t nread = read(pself->fd, buffer, *length);
    if (nread < 0) {
        see_runtime_error_create(error_out, errno);
        return SEE_ERROR_RUNTIME;
    }

    *length = (size_t) nread;
    return SEE_SUCCESS;
}

static int
posix_serial_flush(
    const SeeSerial* self,
    see_serial_dir_t d,
    SeeError** error_out
    )
{
    SeePosixSerial* pself = SEE_POSIX_SERIAL(self);

    int queue;
    if (d == SEE_SERIAL_INPUT)
        queue = TCIFLUSH;
    else if(d == SEE_SERIAL_OUTPUT)
        queue = TCOFLUSH;
    else if (d == SEE_SERIAL_INOUT)
        queue = TCIOFLUSH;
    else
        assert("Unhandled condition" == NULL);

    int ret = tcflush(pself->fd, queue);
    if (ret < 0) {
        see_runtime_error_create(error_out, errno);
        return SEE_ERROR_RUNTIME;
    }
    return SEE_SUCCESS;
}

static int
posix_serial_drain(
    const SeeSerial* self,
    SeeError** error_out
    )
{
    SeePosixSerial* pself = SEE_POSIX_SERIAL(self);

    int ret = tcdrain(pself->fd);
    if (ret < 0) {
        see_runtime_error_create(error_out, errno);
        return SEE_ERROR_RUNTIME;
    }
    return SEE_SUCCESS;
}

static int
posix_serial_set_speed(
    SeeSerial* self,
    see_serial_dir_t d,
    see_speed_t s,
    SeeError** error_out
    )
{
    speed_t speed;
    SeePosixSerial* pself = SEE_POSIX_SERIAL(self);
    struct termios new_settings;

    memcpy(&new_settings, &pself->settings, sizeof(new_settings));
    if (d & SEE_SERIAL_INPUT) {
        speed = get_posix_speed(s);
        cfsetispeed(&new_settings, speed);
    }
    if (d & SEE_SERIAL_OUTPUT) {
        speed = get_posix_speed(s);
        cfsetospeed(&new_settings, speed);
    }
    if (tcsetattr(pself->fd, TCSADRAIN, &new_settings)) {
        see_runtime_error_create(error_out, errno);
        return SEE_ERROR_RUNTIME;
    }
    tcdrain(pself->fd);
    if (tcgetattr(pself->fd, &pself->settings)) {
        see_runtime_error_create(error_out, errno);
        return SEE_ERROR_RUNTIME;
    }
    return SEE_SUCCESS;
}

static int
posix_serial_get_speed(
    SeeSerial*          self,
    see_serial_dir_t    d,
    see_speed_t*        s,
    SeeError**          error_out
    )
{
    (void) error_out;
    const SeeSerialClass* cls = SEE_SERIAL_CLASS(
        see_object_get_class(SEE_OBJECT(self))
    );
    speed_t speed;
    SeePosixSerial* pself = SEE_POSIX_SERIAL(self);
    int open;
    cls->is_open(self, &open);

    if (d & SEE_SERIAL_INPUT)
        speed = cfgetispeed(&pself->settings);
    if (d & SEE_SERIAL_OUTPUT)
        speed = cfgetispeed(&pself->settings);

    *s = get_see_speed(speed);
    return SEE_SUCCESS;
}

static int
posix_serial_is_open(const SeeSerial* self, int* result)
{
    const SeePosixSerial* pself = (const SeePosixSerial*) self;
    if (!self)
        return SEE_INVALID_ARGUMENT;

    if (pself->fd >= 0)
        *result = 1;
    else
        *result = 0;
    return SEE_SUCCESS;
}

/* **** implementation of the public API **** */

int see_posix_terminal_new(SeeSerial** serial, SeeError** error_out)
{
    const SeePosixSerialClass* cls = see_posix_serial_class();
    if (!cls)
        return SEE_NOT_INITIALIZED;

    if (! serial || *serial || !error_out || *error_out)
        return SEE_INVALID_ARGUMENT;

    return SEE_OBJECT_CLASS(cls)->new_obj(
        SEE_OBJECT_CLASS(cls),
        0,
        SEE_OBJECT_REF(serial)
        );
}

/* **** initialization of the class **** */

SeePosixSerialClass* g_SeePosixSerialClass = NULL;

static int see_posix_serial_class_init(SeeObjectClass* new_cls)
{
    int ret = SEE_SUCCESS;
    
    /* Override the functions on the parent here */
    new_cls->init = init;

    SeeSerialClass* serial_cls = (SeeSerialClass*) new_cls;
    serial_cls->open        = posix_serial_open;
    serial_cls->close       = posix_serial_close;
    serial_cls->write       = posix_serial_write;
    serial_cls->read        = posix_serial_read;
    serial_cls->flush       = posix_serial_flush;
    serial_cls->drain       = posix_serial_drain;
    serial_cls->set_speed   = posix_serial_set_speed;
    serial_cls->get_speed   = posix_serial_get_speed;
    serial_cls->is_open     = posix_serial_is_open;

    /* Set the function pointers of the own class here */
    SeePosixSerialClass* cls = (SeePosixSerialClass*) new_cls;
    cls->posix_serial_init  = posix_serial_init;
    
    return ret;
}

/**
 * \private
 * \brief this class initializes SeePosixSerial(Class).
 *
 * You might want to call this from the library initialization func.
 */
int
see_posix_serial_init()
{
    int ret;
    const SeeMetaClass* meta = see_meta_class_class();

    ret = see_meta_class_new_class(
        meta,
        (SeeObjectClass**) &g_SeePosixSerialClass,
        sizeof(SeePosixSerialClass),
        sizeof(SeePosixSerial),
        SEE_OBJECT_CLASS(see_serial_class()),
        sizeof(SeeSerialClass),
        see_posix_serial_class_init
        );

    return ret;
}

void
see_posix_serial_deinit()
{
    if(!g_SeePosixSerialClass)
        return;

    see_object_decref(SEE_OBJECT(g_SeePosixSerialClass));
    g_SeePosixSerialClass = NULL;
}

const SeePosixSerialClass*
see_posix_serial_class()
{
    return g_SeePosixSerialClass;
}

#endif

