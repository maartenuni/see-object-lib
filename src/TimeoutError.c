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


#include "MetaClass.h"
#include "TimeoutError.h"

/* **** functions that implement SeeTimeoutError or override SeeError **** */

static int
timeout_error_init(
    SeeTimeoutError* timeout_error,
    const SeeTimeoutErrorClass* timeout_error_cls
    )
{
    int ret = SEE_SUCCESS;
    const SeeErrorClass* parent_cls = SEE_ERROR_GET_CLASS(
        timeout_error
        );
        
    parent_cls->error_init(
        SEE_ERROR(timeout_error),
        SEE_ERROR_CLASS(timeout_error_cls),
        "TimeoutError occurred"
        );

    return ret;
}

static int
init(const SeeObjectClass* cls, SeeObject* obj, va_list args)
{
    const SeeTimeoutErrorClass* timeout_error_cls = SEE_TIMEOUT_ERROR_CLASS(cls);
    SeeTimeoutError* timeout_error = SEE_TIMEOUT_ERROR(obj);

    // No extra arguments to be extracted.
    (void) args;

    return timeout_error_cls->timeout_error_init(
        timeout_error,
        timeout_error_cls
        );
}

/* **** implementation of the public API **** */

int
see_timeout_error_new(SeeError** error)
{
    if (!error || *error)
        return SEE_INVALID_ARGUMENT;

    const SeeObjectClass* cls = SEE_OBJECT_CLASS(see_timeout_error_class());
    return cls->new_obj(cls, 0, SEE_OBJECT_REF(error));
}

/* **** initialization of the class **** */

SeeTimeoutErrorClass* g_SeeTimeoutErrorClass = NULL;

static int see_timeout_error_class_init(SeeObjectClass* new_cls)
{
    int ret = SEE_SUCCESS;
    
    /* Override the functions on the parent here */
    new_cls->init = init;
    new_cls->name = "SeeTimeoutError";
    
    /* Set the function pointers of the own class here */
    SeeTimeoutErrorClass* cls = (SeeTimeoutErrorClass*) new_cls;
    cls->timeout_error_init   = timeout_error_init;
    
    return ret;
}

/**
 * \private
 * \brief this class initializes SeeTimeoutError(Class).
 *
 * You might want to call this from the library initialization func.
 */
int
see_timeout_error_init()
{
    int ret;
    const SeeMetaClass* meta = see_meta_class_class();

    ret = see_meta_class_new_class(
        meta,
        (SeeObjectClass**) &g_SeeTimeoutErrorClass,
        sizeof(SeeTimeoutErrorClass),
        sizeof(SeeTimeoutError),
        SEE_OBJECT_CLASS(see_error_class()),
        sizeof(SeeErrorClass),
        see_timeout_error_class_init
        );

    return ret;
}

void
see_timeout_error_deinit()
{
    if(!g_SeeTimeoutErrorClass)
        return;

    see_object_decref(SEE_OBJECT(g_SeeTimeoutErrorClass));
    g_SeeTimeoutErrorClass = NULL;
}

const SeeTimeoutErrorClass*
see_timeout_error_class()
{
    return g_SeeTimeoutErrorClass;
}

