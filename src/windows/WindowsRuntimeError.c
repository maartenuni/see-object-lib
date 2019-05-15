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


#include "../MetaClass.h"
#include "WindowsRuntimeError.h"

/* **** functions that implement SeeWindowsRuntimeError or override SeeRuntimeError **** */

static int
windows_runtime_error_init(
    SeeWindowsRuntimeError*             windows_runtime_error,
    const SeeWindowsRuntimeErrorClass*  windows_runtime_error_cls,
    DWORD                               last_error
    )
{
    int ret = SEE_SUCCESS;
    const SeeErrorClass* parent_cls = SEE_ERROR_GET_CLASS(
        windows_runtime_error
        );
    LPSTR out = NULL;

    FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER |
        FORMAT_MESSAGE_FROM_SYSTEM |
        FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL,
        last_error,
        MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US),
        (LPTSTR) &out, // It's everywhere, but this cast remains unclear...
        0,
        NULL
        );

    parent_cls->error_init(
        SEE_ERROR(windows_runtime_error),
        SEE_ERROR_CLASS(windows_runtime_error_cls),
        out
        );

    LocalFree(out);

    return ret;
}

static int
init(const SeeObjectClass* cls, SeeObject* obj, va_list args)
{
    const SeeWindowsRuntimeErrorClass* windows_runtime_error_cls = SEE_WINDOWS_RUNTIME_ERROR_CLASS(cls);
    SeeWindowsRuntimeError* windows_runtime_error = SEE_WINDOWS_RUNTIME_ERROR(obj);
    
    /*Extract parameters here from va_list args here.*/
    DWORD error_no = va_arg(args, DWORD);
    
    return windows_runtime_error_cls->windows_runtime_error_init(
        windows_runtime_error,
        windows_runtime_error_cls,
        error_no
        );
}

/* **** implementation of the public API **** */

int
see_windows_runtime_error_new(
    SeeWindowsRuntimeError**    error_out,
    DWORD                       error_no
    )
{
    const SeeObjectClass* cls = SEE_OBJECT_CLASS(
        see_windows_runtime_error_class()
        );
    if (!cls)
        return SEE_NOT_INITIALIZED;

    if(!error_out || *error_out)
        return SEE_INVALID_ARGUMENT;
    
    return cls->new_obj(cls, 0, SEE_OBJECT_REF(error_out), error_no);
}

/* **** initialization of the class **** */

SeeWindowsRuntimeErrorClass* g_SeeWindowsRuntimeErrorClass = NULL;

static int see_windows_runtime_error_class_init(SeeObjectClass* new_cls)
{
    int ret = SEE_SUCCESS;
    
    /* Override the functions on the parent here */
    new_cls->init = init;
    
    /* Set the function pointers of the own class here */
    SeeWindowsRuntimeErrorClass* cls = (SeeWindowsRuntimeErrorClass*) new_cls;
    cls->windows_runtime_error_init = windows_runtime_error_init;

    return ret;
}

/**
 * \private
 * \brief this class initializes SeeWindowsRuntimeError(Class).
 *
 * You might want to call this from the library initialization func.
 */
int
see_windows_runtime_error_init()
{
    int ret;
    const SeeMetaClass* meta = see_meta_class_class();

    ret = see_meta_class_new_class(
        meta,
        (SeeObjectClass**) &g_SeeWindowsRuntimeErrorClass,
        sizeof(SeeWindowsRuntimeErrorClass),
        sizeof(SeeWindowsRuntimeError),
        SEE_OBJECT_CLASS(see_error_class()),
        sizeof(SeeErrorClass),
        see_windows_runtime_error_class_init
        );

    return ret;
}

void
see_windows_runtime_error_deinit()
{
    if(!g_SeeWindowsRuntimeErrorClass)
        return;

    see_object_decref(SEE_OBJECT(g_SeeWindowsRuntimeErrorClass));
    g_SeeWindowsRuntimeErrorClass = NULL;
}

const SeeWindowsRuntimeErrorClass*
see_windows_runtime_error_class()
{
    return g_SeeWindowsRuntimeErrorClass;
}

