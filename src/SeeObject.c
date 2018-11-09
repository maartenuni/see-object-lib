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

#include "see_object_config.h"
#include "SeeObject.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "atomic_operations.h"

static int object_representation(const SeeObject* obj, char* out, size_t size)
{
    return snprintf(out, size, "See object at %p", (void*) obj);
}

static int object_init(SeeObject* obj, SeeObjectClass* cls)
{
    assert(obj);
    assert(cls);
    obj->refcount = 1;
    obj->cls = cls;
    return 0;
}

static SeeObjectClass*
object_get_class(const SeeObject* obj)
{
    return obj->cls;
}

static void*
object_ref(SeeObject* obj)
{
    assert(obj);
    see_atomic_increment(&obj->refcount);
    return obj;
}

static void
object_decref(SeeObject* obj)
{
    assert(obj);
    int refcount = see_atomic_decrement(&obj->refcount);
    if (refcount == 0) {
        obj->cls->destroy(obj);
    }
}

static void object_destroy(SeeObject* obj)
{
    free(obj);
}

SeeObjectClass see_object_class_instance = {
    // SeeObject
    {
        NULL,
        1
    },
    NULL,
    sizeof(SeeObject),
    object_init,
    object_destroy,
    object_representation,
    object_get_class,
    object_ref,
    object_decref
};

SeeObjectClass* 
see_object_class()
{
    return &see_object_class_instance;
}

int
see_object_class_init()
{
//  The see object class is allways initialized.
//    if (&see_object_class_instance != NULL)
//        return 0;
    return 0;
}

SeeObject* see_object_new(SeeObjectClass* cls)
{
    assert(cls != NULL);
    size_t obj_sz = cls->inst_size;
    SeeObject* obj = malloc(obj_sz);
    if (!obj)
        return obj;
    cls->init(obj, cls);
    return obj;
}

SeeObject* see_object_create()
{
    SeeObject* obj = see_object_new(&see_object_class_instance);
    return obj;
}

int see_object_repr(const SeeObject* obj, char* out, size_t size)
{
    SeeObjectClass* cls = obj->cls;
    return cls->repr(obj, out, size);
}

SeeObjectClass* see_object_get_class(const SeeObject* obj)
{
    assert(obj);
    SeeObjectClass* cls = obj->cls;
    if (!obj)
        return NULL;
    else
        return cls->get_class(obj);
}

void* see_object_ref(SeeObject* obj)
{
    return see_object_class_instance.incref(obj);
}

void see_object_decref(SeeObject* obj)
{
    SeeObjectClass* cls = obj->cls;
    cls->decref(obj);
}
