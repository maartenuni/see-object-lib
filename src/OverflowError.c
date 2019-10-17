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
#include "OverflowError.h"

/* **** functions that implement SeeOverflowError or override SeeError **** */

static int
overflow_error_init(
    SeeOverflowError* overflow_error,
    const SeeOverflowErrorClass* overflow_error_cls,
    const char* msg
    )
{
    int ret = SEE_SUCCESS;
    const SeeErrorClass* parent_cls = SEE_ERROR_GET_CLASS(
        overflow_error
        );

    const char* default_message = "An overflowing operation is caught.";
    if (!msg)
        msg = default_message;
        
    parent_cls->error_init(
        SEE_ERROR(overflow_error),
        SEE_ERROR_CLASS(overflow_error_cls),
        msg
    );
     
    return ret;
}

static int
init(const SeeObjectClass* cls, SeeObject* obj, va_list args)
{
    const SeeOverflowErrorClass* overflow_error_cls = SEE_OVERFLOW_ERROR_CLASS(cls);
    SeeOverflowError* overflow_error = SEE_OVERFLOW_ERROR(obj);
    
    /*Extract parameters here from va_list args here.*/
    const char* msg = va_arg(args, const char*);
    
    return overflow_error_cls->overflow_error_init(
        overflow_error,
        overflow_error_cls,
        msg
        );
}

/* **** implementation of the public API **** */

int see_overflow_error_new(
    SeeError**  error_out,
    const char* msg
    )
{
    const SeeObjectClass* cls = SEE_OBJECT_CLASS(
        see_overflow_error_class()
    );
    if (!cls)
        return SEE_NOT_INITIALIZED;

    return cls->new_obj(cls, 0, SEE_OBJECT_REF(error_out), msg);
}

/* **** initialization of the class **** */

SeeOverflowErrorClass* g_SeeOverflowErrorClass = NULL;

static int see_overflow_error_class_init(SeeObjectClass* new_cls)
{
    int ret = SEE_SUCCESS;
    
    /* Override the functions on the parent here */
    new_cls->init = init;
    
    /* Set the function pointers of the own class here */
    SeeOverflowErrorClass* cls = (SeeOverflowErrorClass*) new_cls;
    cls->overflow_error_init = overflow_error_init;
    
    return ret;
}

/**
 * \private
 * \brief this class initializes SeeOverflowError(Class).
 *
 * You might want to call this from the library initialization func.
 */
int
see_overflow_error_init()
{
    int ret;
    const SeeMetaClass* meta = see_meta_class_class();

    ret = see_meta_class_new_class(
        meta,
        (SeeObjectClass**) &g_SeeOverflowErrorClass,
        sizeof(SeeOverflowErrorClass),
        sizeof(SeeOverflowError),
        SEE_OBJECT_CLASS(see_error_class()),
        sizeof(SeeErrorClass),
        see_overflow_error_class_init
        );

    return ret;
}

void
see_overflow_error_deinit()
{
    if(!g_SeeOverflowErrorClass)
        return;

    see_object_decref(SEE_OBJECT(g_SeeOverflowErrorClass));
    g_SeeOverflowErrorClass = NULL;
}

const SeeOverflowErrorClass*
see_overflow_error_class()
{
    return g_SeeOverflowErrorClass;
}

