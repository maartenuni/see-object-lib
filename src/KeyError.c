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
#include "KeyError.h"

/* **** functions that implement SeeKeyError or override SeeError **** */

static int
key_error_init(
    SeeError* key_error,
    const SeeKeyErrorClass* key_error_cls,
    const char* key
    )
{
    int ret = SEE_SUCCESS;
    const SeeErrorClass* parent_cls = SEE_ERROR_GET_CLASS(
        key_error
        );

    char buffer[80];
    snprintf(buffer, sizeof(buffer), "No such key \"%s\"", key);
        
    /* Because every class has its own members to initialize, you have
     * to check how your parent is initialized and pass through all relevant
     * parameters here. Typically, this should be parent_name_init, where
     * parent_name is the name of the parent and it should be the first function
     * that extends the parent above its parent.
     * Check how the parent is initialized and pass through the right parameters.
     */
    parent_cls->error_init(
            SEE_ERROR(key_error),
            SEE_ERROR_CLASS(key_error_cls),
            buffer
            );
    
    /*
     * Initialize whatever the parents initializer function didn't initialize.
     * Typically, SeeKeyError extends SeeError with one or 
     * a few new members. Those bytes should be 0, since the default 
     * SeeObjectClass->new() uses calloc to allocate 1 instance.
     * However, this is the place to give them a sensible value.
     */
    
    return ret;
}

static int
init(const SeeObjectClass* cls, SeeObject* obj, va_list args)
{
    const SeeKeyErrorClass* key_error_cls = SEE_KEY_ERROR_CLASS(cls);
    SeeError* key_error = SEE_ERROR(obj);
    
    /*Extract parameters here from va_list args here.*/
    const char* key = va_arg(args, const char*);
    
    return key_error_cls->key_error_init(
        key_error,
        key_error_cls,
        key
        );
}

/* **** implementation of the public API **** */

/*
 * Carefully examine whether BETWEEN obj_out and error_out should be another
 * parameter. Eg for SeeError that might be a const char* msg. This
 * should also be added in the header file.
 */
int see_key_error_new(SeeError** obj_out, const char* key)
{
    const SeeObjectClass* cls = SEE_OBJECT_CLASS(
        see_key_error_class()
        );

    if (!cls)
        return SEE_NOT_INITIALIZED;

    if (!obj_out || *obj_out)
        return SEE_INVALID_ARGUMENT;

    return cls->new_obj(
            cls,
            0,
            SEE_OBJECT_REF(obj_out),
            key
            );
}

/* **** initialization of the class **** */

SeeKeyErrorClass* g_SeeKeyErrorClass = NULL;

static int key_error_class_init(SeeObjectClass* new_cls)
{
    int ret = SEE_SUCCESS;
    
    /* Override the functions on the SeeObject here */
    new_cls->init = init;

    // Every class should have a unique name.
    new_cls->name = "SeeKeyError";

    // These can be optionally overwritten
    // Then you need to create static function above with the proper
    // signature.
    // new_cls->compare        = keyerror_compare;
    // new_cls->less           = keyerror_less;
    // new_cls->less_equal     = keyerror_less_equal;
    // new_cls->equal          = keyerror_equal;
    // new_cls->not_equal      = keyerror_not_equal;
    // new_cls->greater_equal  = keyerror_greater_equal;
    // new_cls->greater        = keyerror_greater;
    // new_cls->copy           = keyerror_copy;

    /* Set the function pointers of the own class here */
    SeeKeyErrorClass* cls = (SeeKeyErrorClass*) new_cls;
    cls->key_error_init = key_error_init;

    return ret;
}

/**
 * \private
 * \brief this class initializes SeeKeyError(Class).
 *
 * You might want to call this from the library initialization func.
 */
int
see_key_error_init()
{
    int ret;
    const SeeMetaClass* meta = see_meta_class_class();

    ret = see_meta_class_new_class(
        meta,
        (SeeObjectClass**) &g_SeeKeyErrorClass,
        sizeof(SeeKeyErrorClass),
        sizeof(SeeKeyError),
        SEE_OBJECT_CLASS(see_error_class()),
        sizeof(SeeErrorClass),
        key_error_class_init
        );

    return ret;
}

void
see_key_error_deinit()
{
    if(!g_SeeKeyErrorClass)
        return;

    see_object_decref(SEE_OBJECT(g_SeeKeyErrorClass));
    g_SeeKeyErrorClass = NULL;
}

const SeeKeyErrorClass*
see_key_error_class()
{
    return g_SeeKeyErrorClass;
}

