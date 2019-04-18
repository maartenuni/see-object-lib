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


#include <assert.h>
#include "MetaClass.h"
#include "Serial.h"
#include "see_object_config.h"

#if defined(HAVE_TERMIOS_H)
#include "posix/PosixSerial.h"
#elif defined(_WIN32)
#include "window/WindowsSerial.h"
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

void serial_destroy(SeeObject* obj)
{
    int ret, open;
    if (!obj)
        return;
    SeeSerial* serial = SEE_SERIAL(obj);
    const SeeSerialClass* cls = SEE_SERIAL_CLASS(
        see_object_get_class(obj)
        );

    assert(cls->is_open);
    ret  = cls->is_open(serial, &open);
    assert(ret == SEE_SUCCESS);
    SeeError* error = NULL;
    if (open)
        cls->close(serial, &error);

    see_object_class()->destroy(obj);
}

/* **** implementation of the public API **** */

int
see_serial_new_dev(SeeSerial** out, const char* dev, SeeError** error_out)
{
    const SeeSerialClass* cls = NULL;

#if defined(HAVE_TERMIOS_H) && defined(HAVE_UNISTD_H)
    cls = SEE_SERIAL_CLASS(see_posix_serial_class());
#elif defined(_WIN32)
    cls = see_windows_serial_class();
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
    const void*         bytes,
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
    void*               bytes,
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
see_serial_set_speed(
    SeeSerial* self,
    see_serial_dir_t dir,
    speed_t    speed,
    SeeError** error_out
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
see_serial_set_timeout(SeeSerial* self, int ms, SeeError** error_out)
{
    if (!self || !error_out || *error_out)
        return SEE_INVALID_ARGUMENT;

    const SeeSerialClass* cls = SEE_SERIAL_GET_CLASS(self);

    return cls->set_timeout(self, ms, error_out);
}

int see_serial_get_timeout(const SeeSerial* self, int* ms, SeeError** error_out)
{
    if (!self || !ms || !error_out || *error_out)
        return SEE_INVALID_ARGUMENT;

    const SeeSerialClass* cls = SEE_SERIAL_GET_CLASS(self);

    return cls->get_timeout(self, ms, error_out);
}


/* **** initialization of the class **** */

SeeSerialClass* g_SeeSerialClass = NULL;


static int see_serial_class_init(SeeObjectClass* new_cls)
{
    int ret = SEE_SUCCESS;

    /* Override the functions on the parent here */
    new_cls->init       = init;
    new_cls->destroy    = serial_destroy;

    /* Set the function pointers of the own class here */
    SeeSerialClass* cls = (SeeSerialClass*) new_cls;
    cls->serial_init    = serial_init;

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

