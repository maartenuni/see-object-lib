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
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>


static int new_class_init(
    const SeeObjectClass* meta,
    SeeObject* new_cls,
    va_list args
    )
{
    int selector = 0;

    /* call supers initializer. */
    const SeeObjectClass* meta_super = meta->psuper;
    int ret = meta_super->init(meta, new_cls, args);
    if (ret != SEE_SUCCESS)
        return ret;

    SeeObjectClass* cls = (SeeObjectClass*) new_cls;
    SeeObjectClass* parent = NULL;

    size_t parent_size = 0, instance_size = 0;
    see_class_init_func cls_init = NULL;

    while ((selector = va_arg(args, int)) != SEE_META_INIT_FINISHED) {
        switch (selector) {
            case SEE_META_INIT_INSTANCE_SIZE:
                instance_size = va_arg(args, size_t);
                break;
            case SEE_META_INIT_PARENT_CLS_SIZE:
                parent_size = va_arg(args, size_t);
                break;
            case SEE_META_INIT_PARENT:
                parent = va_arg(args, SeeObjectClass*);
                break;
            case SEE_META_INIT_CLS_INIT_FUNC:
                cls_init = va_arg(args, see_class_init_func);
                break;
            default:
                /* TODO this is still quite harsh, perhaps ignore unknowns.
                 * But then how do we establish the the size of
                 * the second argument, or do we break out of the loop and
                 * hope the calling class has made a copy??
                 */
                return SEE_INVALID_ARGUMENT;
        }
    }

    assert(parent != NULL);
    assert(cls != NULL);
    assert(instance_size != 0);
    assert(parent_size != 0);
    assert(cls_init != NULL);
    if (parent == NULL || cls == NULL ||
        instance_size == 0 || parent_size == 0 ||
        cls_init == 0
        )
        return SEE_INVALID_ARGUMENT;

    // copies pointers from parent to the child.
    memcpy(cls, parent, parent_size);
    cls->psuper     = parent;
    cls->inst_size  = instance_size;

    return cls_init((SeeObjectClass*)new_cls);
}

static int
new_cls(
    const SeeObjectClass*   meta,
    SeeObject**             out,
    ...
    )
{
    int ret;
    SeeObjectClass* new_cls = NULL;

    if (!out || *out)
        return SEE_INVALID_ARGUMENT;


    va_list args;
    va_start(args, out);

    int arg = va_arg(args, int);
    if (arg != SEE_META_NEW_CLASS_SIZE) {
        ret = SEE_INVALID_ARGUMENT;
        goto new_cls_error;
    }

    size_t cls_size = va_arg(args, size_t);

    new_cls = calloc(1, cls_size);
    if (!new_cls) {
        ret = SEE_RUNTIME_ERROR;
        goto  new_cls_error;
    }

    const SeeObjectClass* cls = (SeeObjectClass*) meta;
    ret = cls->init(
        meta,
        (SeeObject*) new_cls,
        args
        );
    if (ret != SEE_SUCCESS)
        goto new_cls_error;

    *out = (SeeObject*) new_cls;

    va_end(args);
    return ret;

new_cls_error:

    if (new_cls)
        see_object_decref((SeeObject*)new_cls);

    va_end(args);

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
    const SeeObjectClass* cls = (SeeObjectClass*) meta;
    if (!meta)
        return SEE_INVALID_ARGUMENT;

    if (!out || *out)
        return SEE_INVALID_ARGUMENT;

    if (!parent)
        return SEE_INVALID_ARGUMENT;

    if (!init_func)
        return SEE_INVALID_ARGUMENT;

    return cls->new(
        (const SeeObjectClass*)meta,
        (SeeObject**)out,
        SEE_META_NEW_CLASS_SIZE, class_instance_size,
        SEE_META_INIT_INSTANCE_SIZE, instance_size,
        SEE_META_INIT_PARENT, parent,
        SEE_META_INIT_PARENT_CLS_SIZE, parent_cls_size,
        SEE_META_INIT_CLS_INIT_FUNC, init_func,
        SEE_META_INIT_FINISHED
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

    SeeObjectClass* see_obj_cls = (SeeObjectClass*) g_see_meta_class_instance;
    see_obj_cls->new    = new_cls;
    see_obj_cls->init   = new_class_init;
    see_obj_cls->psuper = see_object_class();

    return SEE_SUCCESS;
}

void see_meta_class_deinit()
{
    see_object_decref((SeeObject*) g_see_meta_class_instance);
    g_see_meta_class_instance = NULL;
}
