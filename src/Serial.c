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

// TODO exmine only do this in derived classes.
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

    ret  = cls->is_open(serial, &open);
    assert(ret == SEE_SUCCESS);
    SeeError* error = NULL;
    if (open)
        cls->close(serial, &error);
    assert (error == NULL);

    see_object_class()->destroy(obj);
}

/* **** implementation of the public API **** */

/* **** initialization of the class **** */

SeeSerialClass* g_SeeSerialClass = NULL;


static int see_serial_class_init(SeeObjectClass* new_cls)
{
    int ret = SEE_SUCCESS;

    /* Override the functions on the parent here */
    new_cls->init = init;
    new_cls->destroy = serial_destroy;

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
#if defined(HAVE_TERMIOS_H)
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
}

const SeeSerialClass*
see_serial_class()
{
#if defined(HAVE_TERMIOS_H)
    return SEE_SERIAL_CLASS(see_posix_serial_class());
#elif
    return SEE_SERIAL_CLASS(see_windows_serial_class());
#endif
    return NULL;
}

