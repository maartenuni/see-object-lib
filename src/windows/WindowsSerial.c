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
    SeeWindowsSerial* windows_serial,
    const SeeWindowsSerialClass* windows_serial_cls,
    /*Add your parameters here and make sure you obtain them in init below*/
    )
{
    int ret = SEE_SUCCESS;
    const SeeSerialClass* parent_cls = SEE_SERIAL_GET_CLASS(
        windows_serial
        );
        
    /* Because every class has its own members to initialize, you have
     * to check how your parent is initialized and pass through all relevant
     * parameters here. Typically, this should be parent_name_init, where
     * parent_name is the name of the parent and it should be the first function
     * that extends the parent above its parent.
     * Check how the parent is initialized and pass through the right parameters.
     */
    parent_cls->parent_init(windows_serial, windows_serial_cls);
    
     /*
     * Check if the parent initialization was successful.
     * if not return a useful error value.
     */
     
    /*
     * Initialize whatever the parents initializer function didn't initialize.
     * Typically, SeeWindowsSerial extends SeeSerial with one or 
     * a few new members. Those bytes should be 0, since the default 
     * SeeObjectClass->new() uses calloc to allocate 1 instance.
     * However, this is the place to give them a sensible value.
     */
    
    return ret;
}

static int
init(const SeeObjectClass* cls, SeeObject* obj, va_list args)
{
    const SeeWindowsSerialClass* windows_serial_cls = SEE_WINDOWS_SERIAL_CLASS(cls);
    SeeWindowsSerial* windows_serial = SEE_WINDOWS_SERIAL(obj);
    
    /*Extract parameters here from va_list args here.*/
    
    return windows_serial_cls->windows_serial_init(
        windows_serial,
        windows_serial_cls
        /*Add your extra parameters here.*/
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
