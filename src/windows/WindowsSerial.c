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

#if defined(_WIN32)

#include "see_object_config.h"
#include "../MetaClass.h"
#include "WindowsSerial.h"

/* **** functions that implement SeeWindowsSerial or override SeeSerial **** */

static int
windows_serial_init(
    SeeWindowsSerial*           windows_serial,
    const SeeWindowsSerialClass* windows_serial_cls,
    const char*                 devfn,
    SeeError**                  error
    )
{
    int ret = SEE_SUCCESS;
    const SeeSerialClass* parent_cls = SEE_SERIAL_GET_CLASS(
        windows_serial
        );
        
    ret = parent_cls->serial_init(
        SEE_SERIAL(windows_serial),
        SEE_SERIAL_CLASS(windows_serial_cls),
        devfn,
        error
        );
    
    return ret;
}

static int
init(const SeeObjectClass* cls, SeeObject* obj, va_list args)
{
    const SeeWindowsSerialClass* windows_serial_cls = SEE_WINDOWS_SERIAL_CLASS(cls);
    SeeWindowsSerial* windows_serial = SEE_WINDOWS_SERIAL(obj);
    
    /*Extract parameters here from va_list args here.*/
    const char* dev         = va_arg(args, const char*);
    SeeError**  error_out   = va_arg(args, SeeError**);
    
    return windows_serial_cls->windows_serial_init(
        windows_serial,
        windows_serial_cls,
        dev,
        error_out
        );
}

/* **** implementation of the public API **** */

/* **** initialization of the class **** */

SeeWindowsSerialClass* g_SeeWindowsSerialClass = NULL;

static int see_windows_serial_class_init(SeeObjectClass* new_cls)
{
    int ret = SEE_SUCCESS;
    
    /* Override the functions on the parent here */
    new_cls->init = init;
    
    /* Set the function pointers of the own class here */
    SeeWindowsSerialClass* cls = (SeeWindowsSerialClass*) new_cls;
    
    return ret;
}

/**
 * \private
 * \brief this class initializes SeeWindowsSerial(Class).
 *
 * You might want to call this from the library initialization func.
 */
int
see_windows_serial_init()
{
    int ret;
    const SeeMetaClass* meta = see_meta_class_class();

    ret = see_meta_class_new_class(
        meta,
        (SeeObjectClass**) &g_SeeWindowsSerialClass,
        sizeof(SeeWindowsSerialClass),
        sizeof(SeeWindowsSerial),
        SEE_OBJECT_CLASS(see_serial_class()),
        sizeof(SeeSerialClass),
        see_windows_serial_class_init
        );

    return ret;
}

void
see_windows_serial_deinit()
{
    if(!g_SeeWindowsSerialClass)
        return;

    see_object_decref(SEE_OBJECT(g_SeeWindowsSerialClass));
    g_SeeWindowsSerialClass = NULL;
}

const SeeWindowsSerialClass*
see_windows_serial_class()
{
    return g_SeeWindowsSerialClass;
}

#endif
