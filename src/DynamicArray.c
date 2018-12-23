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
#include "DynamicArray.h"

/* **** functions that implement SeeDynamicArray or override SeeObject **** */

static int
init(const SeeObjectClass* cls, SeeObject* obj, va_list* args)
{
    int ret, selector;
    const SeeObjectClass* super = cls->psuper;
    SeeDynamicArrayClass* own_class = (SeeDynamicArrayClass*) cls;
    
    // Generally you could set some default values here.
    // The init loop can still override them when necessary.
    
    ret = super->init(cls, obj, args);
    if (ret != SEE_SUCCESS)
        return ret;
    
    while ((selector = va_arg(*args, int)) != SEE_DYNAMIC_ARRAY_INIT_FINAL) {
        switch (selector) {
            // handle your cases here and remove this comment.
            default:
                return SEE_INVALID_ARGUMENT;        
        }
    }
    
    // Do some extra initialization here (on demand).
    
    return SEE_SUCCESS;
}

/* **** implementation of the public API **** */

/* **** initialization of the class **** */

SeeDynamicArrayClass* g_SeeDynamicArrayClass = NULL;

static int see_dynamic_array_class_init(SeeObjectClass* new_cls) {
    int ret = SEE_SUCCESS;
    
    /* Override the functions on the parent here */
    new_cls->init = init;
    
    /* Set the function pointers of the own class here */
    SeeDynamicArrayClass* cls = (SeeDynamicArrayClass*) new_cls;
    
    return ret;
}

/**
 * \private
 * \brief this class initializes SeeDynamicArray(Class).
 *
 * You might want to call this from the library initialization func.
 */
int
see_dynamic_array_init() {
    int ret;
    const SeeMetaClass* meta = see_meta_class_class();

    ret = see_meta_class_new_class(
        meta,
        (SeeObjectClass**) &g_SeeDynamicArrayClass,
        sizeof(SeeDynamicArrayClass),
        sizeof(SeeDynamicArray),
        see_object_class(),
        sizeof(SeeObjectClass),
        see_dynamic_array_class_init
        );

    return ret;
}

void
see_dynamic_array_deinit()
{
    if(!g_SeeDynamicArrayClass)
        return;

    see_object_decref((SeeObject*) g_SeeDynamicArrayClass);
    g_SeeDynamicArrayClass = NULL;
}

