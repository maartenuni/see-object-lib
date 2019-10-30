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


#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "MetaClass.h"
#include "RuntimeError.h"
#include "see_object_config.h"

/* **** functions that implement SeeRuntimeError or override SeeError **** */

static int
runtime_error_init(
    SeeRuntimeError* runtime_error,
    const SeeRuntimeErrorClass* runtime_error_cls,
    int error_num
    )
{
    int ret = SEE_SUCCESS;
    const SeeErrorClass* parent_cls = SEE_ERROR_GET_CLASS(
        runtime_error
        );

#if defined(HAVE_STRERROR_R) || defined(HAVE_STRERROR_S)
    char msg[BUFSIZ];
#else
    const char* msg;
#endif

#if defined(HAVE_STRERROR_S)
    strerror_s(msg, sizeof(msg), error_num);
#elif defined(HAVE_STRERROR_R)
    strerror_r(error_num, msg, sizeof(msg));
#else
    msg = strerror(error_num);
#endif

    parent_cls->error_init(
        SEE_ERROR(runtime_error),
        SEE_ERROR_CLASS(runtime_error_cls),
        msg
        );

    return ret;
}

static int
init(const SeeObjectClass* cls, SeeObject* obj, va_list args)
{
    const SeeRuntimeErrorClass* runtime_error_cls = SEE_RUNTIME_ERROR_CLASS(cls);
    SeeRuntimeError* runtime_error = SEE_RUNTIME_ERROR(obj);
    
    /*Extract parameters here from va_list args here.*/
    int errornum = va_arg(args, int);
    
    return runtime_error_cls->runtime_error_init(
        runtime_error,
        runtime_error_cls,
        errornum
        );
}

/* **** implementation of the public API **** */

int see_runtime_error_new(SeeError** error, int errnum)
{
    const SeeObjectClass* cls = SEE_OBJECT_CLASS(see_runtime_error_class());

    if (!cls)
        return SEE_NOT_INITIALIZED;

    assert(error && *error == NULL);
    if (!error || *error)
        return SEE_INVALID_ARGUMENT;

    return cls->new_obj(cls, 0, SEE_OBJECT_REF(error), errnum);
}

/* **** initialization of the class **** */

SeeRuntimeErrorClass* g_SeeRuntimeErrorClass = NULL;

static int see_runtime_error_class_init(SeeObjectClass* new_cls)
{
    int ret = SEE_SUCCESS;
    
    /* Override the functions on the parent here */
    new_cls->init = init;
    new_cls->name = "SeeRuntimeError";

    /* Set the function pointers of the own class here */
    SeeRuntimeErrorClass* cls = (SeeRuntimeErrorClass*) new_cls;
    cls->runtime_error_init   = runtime_error_init;
    
    return ret;
}

/**
 * \private
 * \brief this class initializes SeeRuntimeError(Class).
 *
 * You might want to call this from the library initialization func.
 */
int
see_runtime_error_init()
{
    int ret;
    const SeeMetaClass* meta = see_meta_class_class();

    ret = see_meta_class_new_class(
        meta,
        (SeeObjectClass**) &g_SeeRuntimeErrorClass,
        sizeof(SeeRuntimeErrorClass),
        sizeof(SeeRuntimeError),
        SEE_OBJECT_CLASS(see_error_class()),
        sizeof(SeeErrorClass),
        see_runtime_error_class_init
        );

    return ret;
}

void
see_runtime_error_deinit()
{
    if(!g_SeeRuntimeErrorClass)
        return;

    see_object_decref(SEE_OBJECT(g_SeeRuntimeErrorClass));
    g_SeeRuntimeErrorClass = NULL;
}

const SeeRuntimeErrorClass*
see_runtime_error_class()
{
    return g_SeeRuntimeErrorClass;
}

