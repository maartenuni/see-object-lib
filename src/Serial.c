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
 * \file Serial.c
 * \brief Implements the base class of the Serial devices.
 *
 * \private
 */

#include <assert.h>
#include <string.h>
#include <errno.h>
#include "see_object_config.h"
#include "MetaClass.h"
#include "Serial.h"
#include "utilities.h"

#if defined(HAVE_TERMIOS_H)
#include "posix/PosixSerial.h"
#include "RuntimeError.h"

#elif defined(_WIN32)
#include "windows/WindowsSerial.h"
#endif


/* **** functions that implement SeeSerial or override SeeObject **** */


// ToDo find out whether this should only be implemented in derived classes.

static int
serial_init(
    SeeSerial* serial,
    const SeeSerialClass* serial_cls,
    const char* dev,
    SeeError** error_out
    )
{
    int ret = SEE_SUCCESS;
    const SeeObjectClass* parent_cls = SEE_OBJECT_GET_CLASS(
        serial
        );

    parent_cls->object_init(
        SEE_OBJECT(serial),
        SEE_OBJECT_CLASS(serial_cls)
        );

    if (dev)
        ret = serial_cls->open(serial, dev, error_out);

    return ret;
}

// TODO examine only do this in derived classes.
static int
init(const SeeObjectClass* cls, SeeObject* obj, va_list args)
{
    const SeeSerialClass* serial_cls = SEE_SERIAL_CLASS(cls);
    SeeSerial* serial = SEE_SERIAL(obj);

    /*Extract parameters here from va_list args here.*/
    const char* dev         = va_arg(args, const char*);
    SeeError**  error_out   = va_arg(args, SeeError**);

    return serial_cls->serial_init(
        serial,
        serial_cls,
        dev,
        error_out
        );
}

static void
serial_destroy(SeeObject* obj)
{
    int open = 0;
    if (!obj)
        return;
    SeeSerial* serial = SEE_SERIAL(obj);
    const SeeSerialClass* cls = SEE_SERIAL_CLASS(
        see_object_get_class(obj)
        );

    assert(cls->is_open);
    cls->is_open(serial, &open);
    SeeError* error = NULL;
    if (open)
        cls->close(serial, &error);

    see_object_class()->destroy(obj);
}

static int
serial_write_msg(
    const SeeSerial* self,
    SeeMsgBuffer* msg,
    SeeError** error
    )
{
    const SeeSerialClass* cls = SEE_SERIAL_GET_CLASS(self);
    void* buffer_data = NULL;
    char* write_ptr = NULL;
    size_t n_to_write;
    int ret = SEE_SUCCESS;

    ret = see_msg_buffer_get_buffer(msg, &buffer_data, &n_to_write, error);
    if (ret)
        goto fail;

    write_ptr = buffer_data;
    while (n_to_write) {
        ret = cls->write(self, &write_ptr, &n_to_write, error);
        if (ret)
            goto fail;
    }

fail:
    free(buffer_data);
    return ret;
}

static int
obtain_synced_msg_buffer(
    const SeeSerial*    self,
    void**              buffer_out,
    size_t*             buf_len_out,
    SeeError**          error
    )
{
    char sync[10] = {0};
    const char sync_expected[] = "SMSG";
    const size_t sync_length = strlen(sync_expected);
    uint16_t id;
    uint32_t size;
    const SeeSerialClass* cls = SEE_SERIAL_GET_CLASS(self);
#if !defined(NDEBUG)
    const SeeMsgBuffer* buf = NULL; // used for sizeof operation below.
#endif
    int ret = SEE_SUCCESS;
    size_t num_to_read;
    char* bytes = NULL;

#if !defined(NDEBUG)
    const SeeMsgBufferClass* msg_cls = see_msg_buffer_class();
    if (!msg_cls)
        return SEE_NOT_INITIALIZED;

    const char* start_header = msg_cls->msg_start;

    assert(strcmp(start_header, sync_expected) == 0);
    assert(sizeof(id) == sizeof(buf->id));
    assert(sizeof(size) == sizeof(buf->length));
#endif

    num_to_read = sizeof(sync)/sizeof(sync[0]);
    char* start = &sync[0];

    // Try to read an entire header.
    while (num_to_read) {
        ret = cls->read(self, &start, &num_to_read, error);
        if(ret)
            return ret;
    }

    // While we haven't got a valid header, discard the first byte and append
    // a new byte to the end and try again.
    while (sync[0] != sync_expected[0] &&
           sync[1] != sync_expected[1] &&
           sync[2] != sync_expected[2] &&
           sync[3] != sync_expected[3]
           )
    {
        memmove(&sync[0], &sync[1], sizeof(sync) - 1);
        size_t one = 1;
        char* byte = &sync[sizeof(start)-1];

        while (one) {
            ret = see_serial_read(self, &byte, &one, error);
            if (ret)
                return ret;
        }
    }

    // obtain size and id in network byteorder.
    memcpy(&id, &sync[sync_length], sizeof(id));
    memcpy(&size, &sync[sync_length + sizeof(id)], sizeof(size));

    size = see_network_to_host32(size);
    bytes = malloc(size);
    if (!bytes) {
        see_runtime_error_new(error, errno);
        return SEE_ERROR_RUNTIME;
    }

    // Copy the info we've gathered here to the final buffer and read the remainder
    memcpy(&bytes[0], &sync[0], sizeof(sync)/sizeof(sync[0]));
    start = &bytes[sizeof(sync)];
    num_to_read = size - sizeof(sync);

    while (num_to_read) {
        ret = see_serial_read(self, &start, &num_to_read, error);
        if (ret)
            goto fail;
    }

    *buffer_out  = bytes;
    *buf_len_out = size;

    return ret;

    fail:

    free(bytes);
    return ret;

}

static int
serial_read_msg(
    const SeeSerial* self,
    SeeMsgBuffer**   msg_out,
    SeeError**       error_out
    )
{
    SeeMsgBuffer* ret_buf = NULL;
    void*         buffer  = NULL;
    size_t        buf_len = 0;

    int ret = obtain_synced_msg_buffer(self, &buffer, &buf_len, error_out);
    if (ret)
        goto fail;
    assert(buffer != NULL);

    ret = see_msg_buffer_from_buffer(&ret_buf, buffer, buf_len, error_out);
    if (ret)
        goto fail;

    if (*msg_out) // if there is a message there drop its reference
        see_object_decref(SEE_OBJECT(*msg_out));

    // return the message.
    *msg_out = ret_buf;

fail:
    free(buffer);

    return ret;
}

/* **** implementation of the public API **** */

int
see_serial_new_dev(SeeSerial** out, const char* dev, SeeError** error_out)
{
    const SeeSerialClass* cls = NULL;

    // examine which backends are available and load the right one.
#if defined(HAVE_TERMIOS_H) && defined(HAVE_UNISTD_H)
    cls = SEE_SERIAL_CLASS(see_posix_serial_class());
#elif defined(_WIN32)
    cls = SEE_SERIAL_CLASS(see_windows_serial_class());
#else
#pragma warning "No serial classes defined."
#endif

    if (!cls)
        return SEE_NOT_INITIALIZED;

    if (!out || *out)
        return SEE_INVALID_ARGUMENT;

    if (!error_out || *error_out)
        return SEE_INVALID_ARGUMENT;

    return SEE_OBJECT_CLASS(cls)->new_obj(
        SEE_OBJECT_CLASS(cls),
        0,
        SEE_OBJECT_REF(out),
        dev,
        error_out
        );
}

int
see_serial_new(SeeSerial** out, SeeError** error_out)
{
    return see_serial_new_dev(out, NULL, error_out);
}

int see_serial_open(SeeSerial* self, const char* devfn, SeeError** error_out)
{
    if(!self || !error_out || *error_out || !devfn)
        return SEE_INVALID_ARGUMENT;

    const SeeSerialClass* cls = SEE_SERIAL_GET_CLASS(self);

    return cls->open(self, devfn, error_out);
}

int
see_serial_close(SeeSerial* dev, SeeError** error_out)
{
    if (!dev || !error_out || *error_out)
        return SEE_INVALID_ARGUMENT;

    const SeeSerialClass* cls = SEE_SERIAL_GET_CLASS(dev);

    return cls->close(dev, error_out);
}

int see_serial_write(
    const SeeSerial*    self,
    char* * const       bytes,
    size_t*             length,
    SeeError**          error_out
    )
{
    if (!self || !error_out || *error_out)
        return SEE_INVALID_ARGUMENT;

    if (!bytes || !length)
        return SEE_INVALID_ARGUMENT;

    const SeeSerialClass* cls = SEE_SERIAL_GET_CLASS(self);

    return cls->write(self, bytes, length, error_out);
}

int see_serial_read(
    const SeeSerial*    self,
    char**              bytes,
    size_t*             length,
    SeeError**          error_out
    )
{
    if (!self || !error_out || *error_out)
        return SEE_INVALID_ARGUMENT;

    if (!bytes || !length)
        return SEE_INVALID_ARGUMENT;

    const SeeSerialClass* cls = SEE_SERIAL_GET_CLASS(self);

    return cls->read(self, bytes, length, error_out);
}

int
see_serial_flush(
    const SeeSerial* self,
    see_serial_dir_t dir,
    SeeError**       error_out
    )
{
    if (!self || !error_out || *error_out)
        return SEE_INVALID_ARGUMENT;

    const SeeSerialClass* cls = SEE_SERIAL_GET_CLASS(self);

    return cls->flush(self, dir, error_out);
}

int
see_serial_drain(
    const SeeSerial* self,
    SeeError**       error_out
)
{
    if (!self || !error_out || *error_out)
        return SEE_INVALID_ARGUMENT;

    const SeeSerialClass* cls = SEE_SERIAL_GET_CLASS(self);

    return cls->drain(self, error_out);
}

int
see_serial_set_speed(
    SeeSerial*      self,
    see_serial_dir_t dir,
    see_speed_t     speed,
    SeeError**      error_out
    )
{
    if (!self || !error_out || *error_out)
        return SEE_INVALID_ARGUMENT;

    const SeeSerialClass* cls = SEE_SERIAL_GET_CLASS(self);

    return cls->set_speed(self, dir, speed, error_out);
}

int
see_serial_get_speed(
    SeeSerial*          self,
    see_serial_dir_t    dir,
    see_speed_t*        speed_out,
    SeeError**          error_out
    )
{

    if (!self || !error_out || *error_out)
        return SEE_INVALID_ARGUMENT;

    if (!speed_out)
        return SEE_INVALID_ARGUMENT;

    const SeeSerialClass* cls = SEE_SERIAL_GET_CLASS(self);

    return cls->get_speed(self, dir, speed_out, error_out);
}

int
see_serial_is_open(
    const SeeSerial*    self,
    int*                result
    )
{
    if (!self || !result)
        return SEE_INVALID_ARGUMENT;

    const SeeSerialClass* cls = SEE_SERIAL_GET_CLASS(self);

    return cls->is_open(self, result);
}

int
see_serial_set_timeout(
    SeeSerial* self,
    const SeeDuration* dur,
    SeeError** error_out
    )
{
    if (!self || !dur ||!error_out || *error_out)
        return SEE_INVALID_ARGUMENT;

    const SeeSerialClass* cls = SEE_SERIAL_GET_CLASS(self);

    return cls->set_timeout(self, dur, error_out);
}

int see_serial_get_timeout(
    const SeeSerial* self,
    SeeDuration** dur,
    SeeError** error_out
    )
{
    if (!self || !dur || !error_out || *error_out)
        return SEE_INVALID_ARGUMENT;

    const SeeSerialClass* cls = SEE_SERIAL_GET_CLASS(self);

    return cls->get_timeout(self, dur, error_out);
}

int see_serial_set_min_rd_chars(
    SeeSerial*  self,
    uint8_t     nchars,
    SeeError**  error_out
    )
{
    const SeeSerialClass* cls;

    if (!self || !error_out || *error_out)
        return SEE_INVALID_ARGUMENT;

    cls = SEE_SERIAL_GET_CLASS(self);
    return cls->set_min_rd_chars(self, nchars, error_out);
}

int see_serial_get_min_rd_chars(
    SeeSerial*  self,
    uint8_t*    nchars,
    SeeError**  error_out
)
{
    const SeeSerialClass* cls;

    if (!self || !nchars || !error_out || *error_out)
        return SEE_INVALID_ARGUMENT;

    cls = SEE_SERIAL_GET_CLASS(self);
    return cls->get_min_rd_chars(self, nchars, error_out);
}

int
see_serial_write_msg (
    const SeeSerial*    self,
    SeeMsgBuffer*       msg,
    SeeError**          error_out
    )
{
    const SeeSerialClass* cls;

    if (!self || !msg)
        return SEE_INVALID_ARGUMENT;

    if (!error_out || *error_out)
        return SEE_INVALID_ARGUMENT;

    cls = SEE_SERIAL_GET_CLASS(self);

    return cls ->write_msg(self, msg, error_out);
}

int
see_serial_read_msg (
    const SeeSerial*    self,
    SeeMsgBuffer**      msg,
    SeeError**          error
    )
{
    const SeeSerialClass* cls;

    if (!self || !msg)
        return SEE_INVALID_ARGUMENT;

    if (!error || *error)
        return SEE_INVALID_ARGUMENT;

    cls = SEE_SERIAL_GET_CLASS(self);

    return cls->read_msg(self, msg, error);
}

int
see_serial_fd(
    const SeeSerial*    self,
    SeeFileDescriptor*  fd_out,
    SeeError**          error
    )
{
    const SeeSerialClass* cls;

    if (!self || !fd_out)
        return SEE_INVALID_ARGUMENT;

    if (!error || *error)
        return SEE_INVALID_ARGUMENT;

    cls = SEE_SERIAL_GET_CLASS(self);

    return cls->fd(self, fd_out, error);
}

/* **** public function that do not belong to a class **** */

see_speed_t
see_serial_nearest_speed(unsigned speed)
{
    if (speed == 0)
        return SEE_B0;
    else if(speed <= 50)
        return SEE_B50;
    else if(speed <= 75)
        return SEE_B75;
    else if(speed <= 110)
        return SEE_B110;
    else if(speed <= 134)
        return SEE_B134;
    else if(speed <= 150)
        return SEE_B150;
    else if(speed <= 200)
        return SEE_B200;
    else if(speed <= 300)
        return SEE_B300;
    else if(speed <= 600)
        return SEE_B600;
    else if(speed <= 1200)
        return SEE_B1200;
    else if(speed <= 1800)
        return SEE_B1800;
    else if(speed <= 2400)
        return SEE_B2400;
    else if(speed <= 4800)
        return SEE_B4800;
    else if(speed <= 9600)
        return SEE_B9600;
    else if(speed <= 19200)
        return SEE_B19200;
    else if(speed <= 38400)
        return SEE_B38400;
    else if(speed <= 57600)
        return SEE_B57600;
    else if(speed <= 115200)
        return SEE_B115200;
    else
        return SEE_B230400;
}


/* **** initialization of the class **** */

SeeSerialClass* g_SeeSerialClass = NULL;


static int see_serial_class_init(SeeObjectClass* new_cls)
{
    int ret = SEE_SUCCESS;

    /* Override the functions on the parent here */
    new_cls->init       = init;
    new_cls->name       = "SeeSerial";
    new_cls->destroy    = serial_destroy;

    /* Set the function pointers of the own class here */
    SeeSerialClass* cls = (SeeSerialClass*) new_cls;
    cls->serial_init    = serial_init;
    cls->write_msg      = serial_write_msg;
    cls->read_msg       = serial_read_msg;

    // most functions are abstract...
    (void) cls;

    return ret;
}

/**
 * \private
 * \brief this class initializes SeeSerial(Class).
 *
 * You might want to call this from the library initialization func.
 */
int
see_serial_init()
{
    int ret = SEE_NOT_INITIALIZED;
    const SeeMetaClass* meta = see_meta_class_class();
    ret = see_meta_class_new_class(
        meta,
        (SeeObjectClass**)&g_SeeSerialClass,
        sizeof(SeeSerialClass),
        sizeof(SeeSerial),
        see_object_class(),
        sizeof(SeeObjectClass),
        see_serial_class_init
        );
    if (ret)
        return ret;

    // init private subclasses.
#if defined(HAVE_TERMIOS_H) && defined(HAVE_UNISTD_H)
    ret = see_posix_serial_init();
#elif defined(_WIN32)
    ret = see_windows_serial_init();
#endif
    return ret;
}

void
see_serial_deinit()
{
#if defined(HAVE_TERMIOS_H)
    see_posix_serial_deinit();
#elif defined(_WIN32)
    see_windows_serial_deinit();
#endif

    if (!g_SeeSerialClass)
        return;

    see_object_decref(SEE_OBJECT(g_SeeSerialClass));
    g_SeeSerialClass = NULL;
}

const SeeSerialClass*
see_serial_class()
{
    return g_SeeSerialClass;
}

