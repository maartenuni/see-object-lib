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
#include <string.h>
#include <stdlib.h>


static int
new_cls(
    const SeeMetaClass*     meta,
    SeeObjectClass**        out,
    size_t                  class_instance_size,
    size_t                  instance_size,
    const SeeObjectClass*   parent,
    size_t                  parent_cls_size,
    see_class_init_func     init_func
    )
{
    int ret;
    SeeObjectClass* new_cls = NULL;
    if (!out || *out)
        return SEE_INVALID_ARGUMENT;

    if (!parent)
        return SEE_INVALID_ARGUMENT;

    if (!init_func)
        return SEE_INVALID_ARGUMENT;

    const SeeObjectClass* cls = (SeeObjectClass*) meta;
    ret = cls->new(
        (SeeObjectClass*) meta,
        (SeeObject**) &new_cls,
        class_instance_size
        );

    if (ret)
        return ret;

    // Derive all members from our parent.
    memcpy(new_cls, parent, parent_cls_size);

    new_cls->inst_size = instance_size;
    ret = init_func(new_cls);

    if (ret) {
        cls->destroy((SeeObject*) new_cls);
        new_cls = NULL;
    }

    *out = new_cls;
    return ret;
}

SeeMetaClass* g_see_meta_class_instance;

const SeeMetaClass*
see_meta_class_class()
{
    return g_see_meta_class_instance;
}

int see_meta_class_new_class(
    const SeeMetaClass*     meta,
    SeeObjectClass**        out,
    size_t                  class_instance_size,
    size_t                  instance_size,
    const SeeObjectClass*   parent,
    size_t                  parent_cls_size,
    see_class_init_func     init_func
    )
{
    if (!meta)
        return SEE_INVALID_ARGUMENT;

    if (!out || *out)
        return SEE_INVALID_ARGUMENT;

    if (!parent)
        return SEE_INVALID_ARGUMENT;

    if (!init_func)
        return SEE_INVALID_ARGUMENT;

    return meta->new_cls_instance(
        meta,
        out,
        class_instance_size,
        instance_size,
        parent,
        parent_cls_size,
        init_func
        );
}

int see_meta_class_init()
{
    if (g_see_meta_class_instance)
        return SEE_SUCCESS;

    g_see_meta_class_instance = calloc(1, sizeof(SeeMetaClass));
    if (!g_see_meta_class_instance)
        return SEE_RUNTIME_ERROR;

    memcpy(
        g_see_meta_class_instance,
        see_object_class(),
        sizeof(SeeObjectClass)
        );

    SeeObject* obj  = (SeeObject*) g_see_meta_class_instance;
    obj->cls        = (SeeObjectClass*) g_see_meta_class_instance;
    obj->refcount   = 1;

    g_see_meta_class_instance->new_cls_instance = new_cls;

    return SEE_SUCCESS;
}

void see_meta_class_deinit()
{
    see_object_decref((SeeObject*) g_see_meta_class_instance);
    g_see_meta_class_instance = NULL;
}
