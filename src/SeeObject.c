/*
 * This file is part of see-objects.
 *
 * see-objects is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * see-objects is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with see-objects.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "SeeObject.h"
#include "stdlib.h"
#include "stdio.h"
#include "string.h"
#include "assert.h"

static int representation_func(const SeeObject* obj, char* out, size_t size)
{
    return snprintf(out, size, "See object at %p", obj);
}

static int init_func(SeeObject* obj, SeeObjectClass* cls)
{
    assert(obj);
    //TODO  make guaranteed atomic
    obj->refcount = 1;
    obj->cls = cls;
    return 0;
}

static void destroy_func(SeeObject* obj)
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
    init_func,
    destroy_func,
    representation_func
};

SeeObjectClass* 
see_object_class()
{
    return &see_object_class_instance;
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

void see_object_destroy(SeeObject* obj)
{
    SeeObjectClass* cls = obj->cls;
    cls->destroy(obj);
}

int see_object_repr(const SeeObject* obj, char* out, size_t size)
{
    SeeObjectClass* cls = obj->cls;
    return cls->repr(obj, out, size);
}
