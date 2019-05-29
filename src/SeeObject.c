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
 * \file SeeObject.c
 *
 * This file contains the implementations of the most general SeeObject and
 * class.
 *
 * \private
 */

#include "see_object_config.h"
#include "SeeObject.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "atomic_operations.h"
#include "errors.h"
#include "IncomparableError.h"

/* **** implementation of SeeObjects **** */

static int
object_representation(const SeeObject* obj, char** out)
{
	const SeeObjectClass* cls = SEE_OBJECT_GET_CLASS(obj);
	char* str;
	const char* format = "Instance of %s at %p";
	int size = snprintf(NULL, 0, format, cls->name, obj);

	str = malloc(size + 1);
	if (!str)
		return SEE_ERROR_RUNTIME;

	sprintf(str, format, cls->name, obj);

	*out = str;

	return SEE_SUCCESS;
}

static void
object_object_init(SeeObject* obj, const SeeObjectClass* cls)
{
    assert(obj);
    assert(cls);

    obj->cls = cls;
    obj->refcount = 1;
}

static int
object_init(const SeeObjectClass* cls, SeeObject* object, va_list list)
{
    (void) list;
    cls->object_init(object, cls);

    return SEE_SUCCESS;
}

static int
object_new(const SeeObjectClass* cls, size_t cls_sz, SeeObject** out, ...)
{
    (void) cls_sz; // Not used for regular object instances(its for the metaclass).
    SeeObject* new_instance = NULL;
    int ret;

    assert(cls);
    assert(out);
    assert(*out == NULL);

    new_instance = calloc(1, cls->inst_size);

    if (!new_instance)
        return SEE_ERROR_RUNTIME;

    // set class.
    new_instance->cls = cls;

    va_list args;
    va_start(args, out);
    ret = cls->init(cls, new_instance, args);
    va_end(args);

    // free allocated memory and mark out as invalid.
    if (ret != SEE_SUCCESS) {
        see_object_decref(new_instance);
        new_instance = NULL;
    }

    *out = new_instance;
    return ret;
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
    int refcount;
    if (!obj) {
        assert(obj != NULL);
        return;
    }

    refcount = see_atomic_decrement(&obj->refcount);
    if (refcount == 0)
        obj->cls->destroy(obj);
}

static void
object_destroy(SeeObject* obj)
{
    assert(obj);
    free(obj);
}

static int
object_less (
        const SeeObject*    self,
        const SeeObject*    other,
        int*                result,
        struct SeeError**   error
        )
{
    const SeeObjectClass* cls = SEE_OBJECT_GET_CLASS(self);
    int ret = SEE_SUCCESS;

    if (cls->compare) {
        int cmp;
        ret = cls->compare(self, other, &cmp, error);
        if (ret)
            return ret;
        *result = cmp < 0 ? 1 : 0;
    }
    else {
        see_incomparable_error_create(
            error,
            SEE_OBJECT_GET_CLASS(self),
            NULL
        );
        ret = SEE_ERROR_INCOMPARABLE;
    }
    return ret;
}

static int
object_less_equal (
    const SeeObject*    self,
    const SeeObject*    other,
    int*                result,
    struct SeeError**   error
    )
{
    const SeeObjectClass* cls = SEE_OBJECT_GET_CLASS(self);
    int ret = SEE_SUCCESS;

    if (cls->compare) {
        int cmp;
        ret = cls->compare(self, other, &cmp, error);
        if (ret)
            return ret;
        *result = cmp <= 0 ? 1 : 0;
    }
    else {
        see_incomparable_error_create(
            error,
            SEE_OBJECT_GET_CLASS(self),
            NULL
        );
        ret = SEE_ERROR_INCOMPARABLE;
    }
    return ret;
}

static int
object_equal (
    const SeeObject*    self,
    const SeeObject*    other,
    int*                result,
    struct SeeError**   error
    )
{
    const SeeObjectClass* cls = SEE_OBJECT_GET_CLASS(self);
    int ret = SEE_SUCCESS;

    if (cls->compare) {
        int cmp;
        ret = cls->compare(self, other, &cmp, error);
        if (ret)
            return ret;
        *result = cmp == 0 ? 1 : 0;
    }
    else {
        see_incomparable_error_create(
            error,
            SEE_OBJECT_GET_CLASS(self),
            NULL
        );
        ret = SEE_ERROR_INCOMPARABLE;
    }
    return ret;
}

static int
object_not_equal (
    const SeeObject*    self,
    const SeeObject*    other,
    int*                result,
    struct SeeError**   error
)
{
    const SeeObjectClass* cls = SEE_OBJECT_GET_CLASS(self);
    int ret = SEE_SUCCESS;

    if (cls->compare) {
        int cmp;
        ret = cls->compare(self, other, &cmp, error);
        if (ret)
            return ret;
        *result = cmp != 0 ? 1 : 0;
    }
    else {
        see_incomparable_error_create(
            error,
            SEE_OBJECT_GET_CLASS(self),
            NULL
        );
        ret = SEE_ERROR_INCOMPARABLE;
    }
    return ret;
}

static int
object_greater_equal (
    const SeeObject*    self,
    const SeeObject*    other,
    int*                result,
    struct SeeError**   error
    )
{
    const SeeObjectClass* cls = SEE_OBJECT_GET_CLASS(self);
    int ret = SEE_SUCCESS;

    if (cls->compare) {
        int cmp;
        ret = cls->compare(self, other, &cmp, error);
        if (ret)
            return ret;
        *result = cmp >= 0 ? 1 : 0;
    }
    else {
        see_incomparable_error_create(
            error,
            SEE_OBJECT_GET_CLASS(self),
            NULL
        );
        ret = SEE_ERROR_INCOMPARABLE;
    }
    return ret;
}

static int
object_greater(
    const SeeObject*    self,
    const SeeObject*    other,
    int*                result,
    struct SeeError**   error
    )
{
    const SeeObjectClass* cls = SEE_OBJECT_GET_CLASS(self);
    int ret = SEE_SUCCESS;

    if (cls->compare) {
        int cmp;
        ret = cls->compare(self, other, &cmp, error);
        if (ret)
            return ret;
        *result = cmp > 0 ? 1 : 0;
    }
    else {
        see_incomparable_error_create(
            error,
            SEE_OBJECT_GET_CLASS(self),
            NULL
        );
        ret = SEE_ERROR_INCOMPARABLE;
    }
    return ret;
}

/* **** Initialization of the SeeObjectClass **** */

static const SeeObjectClass g_class = {
	.obj = {
		.cls		= &g_class,
		.refcount	= 1
	},
	.psuper		= NULL,
	.name		= "SeeObject",
	.new_obj	= object_new,
	.inst_size  = sizeof(SeeObject),
    .object_init= object_object_init,
    .init       = object_init,
    .destroy    = object_destroy,
    .repr       = object_representation,
    .incref     = object_ref,
    .decref     = object_decref,
    .compare    = NULL,
    .less       = object_less,
    .less_equal = object_less_equal,
    .equal      = object_equal,
    .not_equal  = object_not_equal,
    .greater_equal = object_greater_equal,
    .greater    = object_greater
};

static const SeeObjectClass* see_object_class_instance = &g_class;

const SeeObjectClass*
see_object_class()
{
    return see_object_class_instance;
}

int
see_object_class_init()
{
//  The see object class is always initialized.
    return SEE_SUCCESS;
}

int see_object_new(const SeeObjectClass* cls, SeeObject** out)
{
    if (!cls)
        return SEE_INVALID_ARGUMENT;
    if (!out || *out)
        return SEE_INVALID_ARGUMENT;

    return cls->new_obj(cls, 0, out);
}

SeeObject* see_object_create()
{
    SeeObject* obj = NULL;
    int ret = see_object_new(see_object_class_instance, &obj);
    if (ret) {
        return NULL;
    }
    return obj;
}

int see_object_repr(const SeeObject* obj, char** out)
{
    const SeeObjectClass* cls = SEE_OBJECT_GET_CLASS(obj);
	if (!obj)
		return SEE_INVALID_ARGUMENT;
	if (!out || *out)
		return SEE_INVALID_ARGUMENT;

    return cls->repr(obj, out);
}

const SeeObjectClass*
see_object_get_class(const SeeObject* obj)
{
    if (obj)
        return obj->cls;
    else
        return NULL;
}

void* see_object_ref(SeeObject* obj)
{
    return see_object_get_class(obj)->incref(obj);
}

void see_object_decref(SeeObject* obj)
{
    if (!obj)
        return;

    const SeeObjectClass* cls = see_object_get_class(obj);
    cls->decref(obj);
}

int
see_object_compare(
    const SeeObject*  obj,
    const SeeObject*  other,
    int*              result,
    struct SeeError** error
    )
{
    if (!obj || !other || !result)
        return SEE_INVALID_ARGUMENT;

    if (!error || *error)
        return SEE_INVALID_ARGUMENT;

    const SeeObjectClass* cls = see_object_get_class(obj);

    if (!cls->compare) {
        see_incomparable_error_create(error, cls, NULL);
        return SEE_ERROR_INCOMPARABLE;
    }

    return cls->compare(obj, other, result, error);
}


int
see_object_less (
    const SeeObject*    self,
    const SeeObject*    other,
    int*                result,
    struct SeeError**   error
    )
{
    if (!self || !other || !result)
        return SEE_INVALID_ARGUMENT;

    if (!error || *error)
        return SEE_INVALID_ARGUMENT;

    const SeeObjectClass* cls = see_object_get_class(self);

    return cls->less(self, other, result, error);
}

int
see_object_less_equal (
    const SeeObject*    self,
    const SeeObject*    other,
    int*                result,
    struct SeeError**   error
    )
{
    if (!self || !other || !result)
        return SEE_INVALID_ARGUMENT;

    if (!error || *error)
        return SEE_INVALID_ARGUMENT;

    const SeeObjectClass* cls = see_object_get_class(self);

    return cls->less_equal(self, other, result, error);
}

int
see_object_equal (
    const SeeObject*    self,
    const SeeObject*    other,
    int*                result,
    struct SeeError**   error
    )
{
    if (!self || !other || !result)
        return SEE_INVALID_ARGUMENT;

    if (!error || *error)
        return SEE_INVALID_ARGUMENT;

    const SeeObjectClass* cls = see_object_get_class(self);

    return cls->equal(self, other, result, error);
}

int
see_object_not_equal (
    const SeeObject*    self,
    const SeeObject*    other,
    int*                result,
    struct SeeError**   error
    )
{
    if (!self || !other || !result)
        return SEE_INVALID_ARGUMENT;

    if (!error || *error)
        return SEE_INVALID_ARGUMENT;

    const SeeObjectClass* cls = see_object_get_class(self);

    return cls->not_equal(self, other, result, error);
}

int
see_object_greater_equal(
    const SeeObject*    self,
    const SeeObject*    other,
    int*                result,
    struct SeeError**   error
    )
{
    if (!self || !other || !result)
        return SEE_INVALID_ARGUMENT;

    if (!error || *error)
        return SEE_INVALID_ARGUMENT;

    const SeeObjectClass* cls = see_object_get_class(self);

    return cls->greater_equal(self, other, result, error);
}

int
see_object_greater(
    const SeeObject*    self,
    const SeeObject*    other,
    int*                result,
    struct SeeError**   error
    )
{
    if (!self || !other || !result)
        return SEE_INVALID_ARGUMENT;

    if (!error || *error)
        return SEE_INVALID_ARGUMENT;

    const SeeObjectClass* cls = see_object_get_class(self);

    return cls->greater(self, other, result, error);
}

int
see_object_is_instance_of(
    const SeeObject*        obj,
    const SeeObjectClass*   cls,
    int*                    result
    )
{
    if (!cls)
        return SEE_INVALID_ARGUMENT;

    *result = 0;
    if (!obj)
        return SEE_SUCCESS;

    const SeeObjectClass* temp_cls = SEE_OBJECT_GET_CLASS(obj);
    while (temp_cls) {
        if (temp_cls == cls) {
            *result = 1;
            break;
        }
        temp_cls = temp_cls->psuper;
    }
    return SEE_SUCCESS;
}

