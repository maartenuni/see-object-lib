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
#include "MetaClass.h"
#include "IncomparableError.h"

/* **** functions that implement SeeIncomparableError or override SeeError **** */

static int
incomparable_error_init(
    SeeIncomparableError* incomparable_error,
    const SeeIncomparableErrorClass* incomparable_error_cls,
    const SeeObjectClass* self_class,
    const SeeObjectClass* other_class
    )
{
    int ret = SEE_SUCCESS;
    const SeeErrorClass* parent_cls = SEE_ERROR_GET_CLASS(
        incomparable_error
        );

    char msg[BUFSIZ];

    if (self_class && other_class)
        snprintf(msg, sizeof(msg), "Classes %s and %s are not comparable",
            self_class->name,
            other_class->name
            );
    else if (self_class)
        snprintf(msg, sizeof(msg), "Class %s is not comparable",
        self_class->name
        );
    else
        sprintf(msg, "Incomparable Error");
        
    parent_cls->error_init(
        SEE_ERROR(incomparable_error),
        SEE_ERROR_CLASS(incomparable_error_cls),
        msg
        );

    return ret;
}

static int
init(const SeeObjectClass* cls, SeeObject* obj, va_list args)
{
    const SeeIncomparableErrorClass* incomparable_error_cls = SEE_INCOMPARABLE_ERROR_CLASS(cls);
    SeeIncomparableError* incomparable_error = SEE_INCOMPARABLE_ERROR(obj);
    
    /*Extract parameters here from va_list args here.*/
    const SeeObjectClass* self_class, *other_class;

    self_class  = va_arg(args, const SeeObjectClass*);
    other_class = va_arg(args, const SeeObjectClass*);

    return incomparable_error_cls->incomparable_error_init(
        incomparable_error,
        incomparable_error_cls,
        self_class,
        other_class
        );
}

/* **** implementation of the public API **** */

int
see_incomparable_error_create(
    SeeError** error,
    const SeeObjectClass* self_cls,
    const SeeObjectClass* other_cls
    )
{
    const SeeIncomparableErrorClass* cls = see_incomparable_error_class();

    if (!cls)
        return SEE_NOT_INITIALIZED;

    if (!error || *error)
        return SEE_INVALID_ARGUMENT;

    return SEE_OBJECT_CLASS(cls)->new_obj(
        SEE_OBJECT_CLASS(cls),
        0,
        SEE_OBJECT_REF(error),
        self_cls,
        other_cls
        );
}

/* **** initialization of the class **** */

SeeIncomparableErrorClass* g_SeeIncomparableErrorClass = NULL;

static int see_incomparable_error_class_init(SeeObjectClass* new_cls)
{
    int ret = SEE_SUCCESS;
    
    /* Override the functions on the parent here */
    new_cls->init = init;
    
    /* Set the function pointers of the own class here */
    SeeIncomparableErrorClass* cls = (SeeIncomparableErrorClass*) new_cls;
    cls->incomparable_error_init = incomparable_error_init;
    
    return ret;
}

/**
 * \private
 * \brief this class initializes SeeIncomparableError(Class).
 *
 * You might want to call this from the library initialization func.
 */
int
see_incomparable_error_init()
{
    int ret;
    const SeeMetaClass* meta = see_meta_class_class();

    ret = see_meta_class_new_class(
        meta,
        (SeeObjectClass**) &g_SeeIncomparableErrorClass,
        sizeof(SeeIncomparableErrorClass),
        sizeof(SeeIncomparableError),
        SEE_OBJECT_CLASS(see_error_class()),
        sizeof(SeeErrorClass),
        see_incomparable_error_class_init
        );

    return ret;
}

void
see_incomparable_error_deinit()
{
    if(!g_SeeIncomparableErrorClass)
        return;

    see_object_decref(SEE_OBJECT(g_SeeIncomparableErrorClass));
    g_SeeIncomparableErrorClass = NULL;
}

const SeeIncomparableErrorClass*
see_incomparable_error_class()
{
    return g_SeeIncomparableErrorClass;
}

