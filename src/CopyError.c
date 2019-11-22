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
#include "CopyError.h"
#include "Error.h"
#include "atomic_operations.h"

#include <stdio.h>
#include <assert.h>

/* **** functions that implement SeeCopyError or override SeeError **** */

static int
copy_error_init(
    SeeCopyError* copy_error,
    const SeeCopyErrorClass* copy_error_cls,
    const SeeObjectClass* instance_cls
    )
{
    int ret = SEE_SUCCESS;
    const SeeErrorClass* parent_cls = SEE_ERROR_GET_CLASS(
        copy_error
        );

    char msg[256];
    snprintf(msg, sizeof(msg), "Instances of %s are not copyable",
        instance_cls->name
        );
        
    parent_cls->error_init(
        SEE_ERROR(copy_error),
        SEE_ERROR_CLASS(copy_error_cls),
        msg
        );
    
    return ret;
}

static int
init(const SeeObjectClass* cls, SeeObject* obj, va_list args)
{
    const SeeCopyErrorClass* copy_error_cls = SEE_COPY_ERROR_CLASS(cls);
    SeeCopyError* copy_error = SEE_COPY_ERROR(obj);
    
    /*Extract parameters here from va_list args here.*/
    const SeeObjectClass* instance_cls = va_arg(args, const SeeObjectClass*);
    assert(instance_cls != NULL);
    
    return copy_error_cls->copy_error_init(
        copy_error,
        copy_error_cls,
        instance_cls
        );
}

/* **** implementation of the public API **** */

int
see_copy_error_new(SeeError** error, const SeeObjectClass* instance_cls)
{
    const SeeCopyErrorClass* cls = see_copy_error_class();
    const SeeObjectClass* obj_cls = (const SeeObjectClass*) cls;

    if (!error || *error)
        return SEE_INVALID_ARGUMENT;
    if (!instance_cls)
        return SEE_INVALID_ARGUMENT;

    if(!cls)
        return SEE_NOT_INITIALIZED;

    return obj_cls->new_obj(
        obj_cls,
        0,
        SEE_OBJECT_REF(error),
        instance_cls
        );
}

/* **** initialization of the class **** */

SeeCopyErrorClass* g_SeeCopyErrorClass = NULL;

int g_copy_error_initialize = 0;

static int see_copy_error_class_init(SeeObjectClass* new_cls)
{
    int ret = SEE_SUCCESS;
    
    /* Override the functions on the parent here */
    new_cls->init = init;
    new_cls->name = "SeeCopyError";
    
    /* Set the function pointers of the own class here */
    SeeCopyErrorClass* cls = (SeeCopyErrorClass*) new_cls;
    cls->copy_error_init   = copy_error_init;
    
    return ret;
}

/**
 * \private
 * \brief this class initializes SeeCopyError(Class).
 *
 * You might want to call this from the library initialization func.
 */
int
see_copy_error_init()
{
    int ret = SEE_SUCCESS;
    if (g_SeeCopyErrorClass)
        return SEE_SUCCESS;

    int init = see_atomic_increment(&g_copy_error_initialize);

    if (init == 1) { // It's our job to initialize the class.
        const SeeMetaClass* meta = see_meta_class_class();

        const SeeObjectClass *parent = SEE_OBJECT_CLASS(see_error_class());
        if (!parent)
            see_error_init();

        parent = SEE_OBJECT_CLASS(see_error_class());
        assert(parent);

        ret = see_meta_class_new_class(
            meta,
            (SeeObjectClass **) &g_SeeCopyErrorClass,
            sizeof(SeeCopyErrorClass),
            sizeof(SeeCopyError),
            SEE_OBJECT_CLASS(see_error_class()),
            sizeof(SeeErrorClass),
            see_copy_error_class_init
            );
    }

    see_atomic_decrement(&g_copy_error_initialize);

    while (g_copy_error_initialize != 0) // Something is still initializing
        ; // TODO It would be better to yield the processor for other threads.

    return ret;
}

void
see_copy_error_deinit()
{
    if(!g_SeeCopyErrorClass)
        return;

    see_object_decref(SEE_OBJECT(g_SeeCopyErrorClass));
    g_SeeCopyErrorClass = NULL;
}

const SeeCopyErrorClass*
see_copy_error_class()
{
    return g_SeeCopyErrorClass;
}

