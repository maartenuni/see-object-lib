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
#include "Error.h"

/* **** functions that implement SeeError or override SeeObject **** */

static int
init(const SeeObjectClass* cls, SeeObject* obj, va_list args)
{
    int ret, selector;
    const SeeObjectClass* super = cls->psuper;
    //SeeErrorClass* own_class = (SeeErrorClass*) cls;
    
    // Generally you could set some default values here.
    // The init loop can still override them when necessary.
    const char* msg = "No error";
    
    ret = super->init(cls, obj, args);
    if (ret != SEE_SUCCESS)
        return ret;
    
    while ((selector = va_arg(args, int)) != SEE_ERROR_INIT_FINAL) {
        switch (selector) {
            // handle your cases here and remove this comment.
            case SEE_ERROR_INIT_MSG:
                msg = va_arg(args, const char*);
                break;
            default:
                return SEE_INVALID_ARGUMENT;        
        }
    }
    
    // Do some extra initialization here (on demand).
    SeeError* error = (SeeError*)obj;
    error->msg = msg;
    
    return SEE_SUCCESS;
}

static const char*
msg(const SeeError* error)
{
    assert(error);
    return error->msg;
}

static void
set_msg(SeeError* error, const char* msg)
{
    assert(error);
    assert(msg);

    error->msg = msg;
}

/* **** implementation of the public API **** */

int see_error_new(SeeError** out)
{
    int ret;
    if (!out || *out)
        return SEE_INVALID_ARGUMENT;

    const SeeErrorClass* cls = see_error_class();

    if (!cls)
        return SEE_NOT_INITIALIZED;

    const SeeObjectClass* obj_cls = (const SeeObjectClass*) cls;
    ret = obj_cls->new(
        (const SeeObjectClass*) see_error_class(),
        (SeeObject**) out,
        SEE_OBJECT_INIT_FINAL,
        SEE_ERROR_INIT_FINAL
        );
    return ret;
}

int see_error_new_msg(SeeError** out, const char* msg)
{
    int ret;
    if (!out || *out)
        return SEE_INVALID_ARGUMENT;
    if (!msg)
        return SEE_INVALID_ARGUMENT;

    const SeeErrorClass* cls = see_error_class();

    if (!cls)
        return SEE_NOT_INITIALIZED;

    const SeeObjectClass* obj_cls = (const SeeObjectClass*) cls;
    ret = obj_cls->new(
        (const SeeObjectClass*) see_error_class(),
        (SeeObject**) out,
        SEE_OBJECT_INIT_FINAL,  // no argument
        SEE_ERROR_INIT_MSG,     msg,
        SEE_ERROR_INIT_FINAL    // no argument
        );
    return ret;
}

const char*
see_error_msg(const SeeError* error)
{
    if (!error)
        return NULL;

    return see_error_class()->msg(error);
}

void
see_error_set_msg(SeeError* error, const char* msg)
{
    if (!error)
        return;

    see_error_class()->set_msg(error, msg);
}

/* **** initialization of the class **** */

SeeErrorClass* g_SeeErrorClass = NULL;

static int
see_error_class_init(SeeObjectClass* new_cls) {
    int ret = SEE_SUCCESS;
    
    /* Override the functions on the parent here */
    new_cls->init = init;
    
    /* Set the function pointers of the own class here */
    SeeErrorClass* cls = (SeeErrorClass*) new_cls;
    cls->msg = msg;
    cls->set_msg = set_msg;
    
    return ret;
}

/**
 * \private
 * \brief this class initializes SeeError(Class).
 *
 * You might want to call this from the library initialization func.
 */
int
see_error_init() {
    int ret;
    const SeeMetaClass* meta = see_meta_class_class();

    ret = see_meta_class_new_class(
        meta,
        (SeeObjectClass**) &g_SeeErrorClass,
        sizeof(SeeErrorClass),
        sizeof(SeeError),
        see_object_class(),
        sizeof(SeeObjectClass),
        see_error_class_init
        );

    return ret;
}

void
see_error_deinit()
{
    if(!g_SeeErrorClass)
        return;

    see_object_decref((SeeObject*) g_SeeErrorClass);
    g_SeeErrorClass = NULL;
}

const SeeErrorClass*
see_error_class()
{
    return g_SeeErrorClass;
}
