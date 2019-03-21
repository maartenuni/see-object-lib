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
#include "IndexError.h"

/* **** functions that implement SeeIndexError or override SeeError **** */

static int
index_error_init(
    SeeIndexError* index_error,
    const SeeIndexErrorClass* index_error_cls,
    size_t index
    )
{
    int ret = SEE_SUCCESS;
    const SeeErrorClass* parent_cls = SEE_ERROR_GET_CLASS(
        index_error
        );

    char temp_buf[128];
    snprintf(temp_buf, sizeof(temp_buf), "%zu", index);
        
    /* Because every class has its own members to initialize, you have
     * to check how your parent is initialized and pass through all relevant
     * parameters here. Typically, this should be parent_name_init, where
     * parent_name is the name of the parent and it should be the first function
     * that extends the parent above its parent.
     * Check how the parent is initialized and pass through the right parameters.
     */
    parent_cls->error_init(
        SEE_ERROR(index_error),
        SEE_ERROR_CLASS(index_error_cls),
        temp_buf
        );

    return ret;
}

static int
init(const SeeObjectClass* cls, SeeObject* obj, va_list args)
{
    const SeeIndexErrorClass* index_error_cls = SEE_INDEX_ERROR_CLASS(cls);
    SeeIndexError* index_error = SEE_INDEX_ERROR(obj);

    /*Extract parameters here from va_list args here.*/
    size_t index = va_arg(args, size_t);

    return index_error_cls->index_error_init(
        index_error,
        index_error_cls,
        index
        );
}

/* **** implementation of the public API **** */

int see_index_error_create(SeeError** error, size_t index)
{
    const SeeObjectClass* cls = SEE_OBJECT_CLASS(see_index_error_class());
    if (!error || *error)
        return SEE_INVALID_ARGUMENT;

    return cls->new_obj(
        SEE_OBJECT_CLASS(see_index_error_class()),
        0,
        SEE_OBJECT_REF(error),
        index
        );
}

/* **** initialization of the class **** */

SeeIndexErrorClass* g_SeeIndexErrorClass = NULL;

static int see_index_error_class_init(SeeObjectClass* new_cls)
{
    int ret = SEE_SUCCESS;
    
    /* Override the functions on the parent here */
	new_cls->name = "SeeIndexError";
    new_cls->init = init;
    
    /* Set the function pointers of the own class here */
    SeeIndexErrorClass* cls = (SeeIndexErrorClass*)(new_cls);
    cls->index_error_init = index_error_init;
    
    return ret;
}

/**
 * \private
 * \brief this class initializes SeeIndexError(Class).
 *
 * You might want to call this from the library initialization func.
 */
int
see_index_error_init()
{
    int ret;
    const SeeMetaClass* meta = see_meta_class_class();

    ret = see_meta_class_new_class(
        meta,
        (SeeObjectClass**) &g_SeeIndexErrorClass,
        sizeof(SeeIndexErrorClass),
        sizeof(SeeIndexError),
        SEE_OBJECT_CLASS(see_error_class()),
        sizeof(SeeErrorClass),
        see_index_error_class_init
        );

    return ret;
}

void
see_index_error_deinit()
{
    if(!g_SeeIndexErrorClass)
        return;

    see_object_decref(SEE_OBJECT(g_SeeIndexErrorClass));
    g_SeeIndexErrorClass = NULL;
}

const SeeIndexErrorClass*
see_index_error_class()
{
    return g_SeeIndexErrorClass;
}
