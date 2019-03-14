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

/**
 * \file MetaClass.c the implementation of the metaclass in order to generate
 * new classes.
 * \private
 */

#include "MetaClass.h"
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>


static int new_class_init(
    SeeObjectClass* new_cls,
    const SeeObjectClass* meta,
    size_t instance_size,
    const SeeObjectClass* parent,
    size_t parent_cls_size,
    see_class_init_func init_func
    )
{
    /* call supers initializer. */
    const SeeObjectClass* meta_super = meta->psuper;
    meta_super->object_init(SEE_OBJECT(new_cls), meta);

    assert(new_cls != NULL);
    assert(instance_size != 0);
    assert(parent_cls_size != 0);
    assert(init_func != NULL);

    if (parent == NULL     || new_cls == NULL  ||
        instance_size == 0 || parent_cls_size == 0 ||
        init_func == 0
        )
        return SEE_INVALID_ARGUMENT;

    // copies pointers from parent to the child.
    memcpy(new_cls, parent, parent_cls_size);
    new_cls->psuper     = parent;
    new_cls->inst_size  = instance_size;

    return init_func(new_cls);
}

static int
meta_init(const SeeObjectClass* meta, SeeObject* out, va_list list)
{
    size_t instance_size, parent_class_size;
    const SeeObjectClass* parent = NULL;
    see_class_init_func cls_init = NULL;
    const SeeMetaClass* meta_cls = SEE_META_CLASS(meta);

    // Call the parent init function.
    meta->object_init(out, meta);

    instance_size       = va_arg(list, size_t);
    parent              = va_arg(list, const SeeObjectClass*);
    parent_class_size   = va_arg(list, size_t);
    cls_init            = va_arg(list, see_class_init_func);

    return meta_cls->class_init(
        (SeeObjectClass*)out,
        meta,
        instance_size,
        parent,
        parent_class_size,
        cls_init
        );
}

static int
new_cls(
    const SeeObjectClass*   meta,
    size_t                  cls_size,
    SeeObject**             out,
    ...
    )
{
    int ret;
    va_list args;
    SeeObjectClass* new_cls = NULL;

    new_cls = calloc(1, cls_size);
    if (!new_cls)
        return SEE_ERROR_RUNTIME;

    va_start(args, out);

    const SeeObjectClass* cls = SEE_OBJECT_CLASS(meta);
    ret = cls->init(
        meta,
        (SeeObject*) new_cls,
        args
        );

    va_end(args);

    if (ret != SEE_SUCCESS) {
        if (new_cls) {
            see_object_decref(SEE_OBJECT(new_cls));
        }
    }

    *out = (SeeObject*) new_cls;

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
    const SeeObjectClass* cls = SEE_OBJECT_CLASS(meta);
    if (!meta)
        return SEE_INVALID_ARGUMENT;

    if (!out || *out)
        return SEE_INVALID_ARGUMENT;

    if (!parent)
        return SEE_INVALID_ARGUMENT;

    if (!init_func)
        return SEE_INVALID_ARGUMENT;

    return cls->new_obj(
        (const SeeObjectClass*)meta,
        class_instance_size,
        (SeeObject**)out,
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
        return SEE_ERROR_RUNTIME;

    memcpy(
        g_see_meta_class_instance,
        see_object_class(),
        sizeof(SeeObjectClass)
        );

    SeeObject* obj  = SEE_OBJECT(g_see_meta_class_instance);
    obj->cls        = SEE_OBJECT_CLASS(g_see_meta_class_instance);
    obj->refcount   = 1;

    // The macro cast provides a const pointer while we want to modify the class.
    SeeObjectClass* see_obj_cls = (SeeObjectClass*)g_see_meta_class_instance;
    see_obj_cls->new_obj= new_cls;
    see_obj_cls->init   = meta_init;
    see_obj_cls->psuper = see_object_class();

    g_see_meta_class_instance->class_init = new_class_init;

    return SEE_SUCCESS;
}

void see_meta_class_deinit()
{
    see_object_decref(SEE_OBJECT(g_see_meta_class_instance));
    g_see_meta_class_instance = NULL;
}
