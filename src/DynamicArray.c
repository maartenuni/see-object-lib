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

#include <string.h>
#include <stdlib.h>

#include "MetaClass.h"
#include "DynamicArray.h"
#include "see_functions.h"

/* **** some private helper macro's **** */

#define ARRAY_ELEM_ADDRESS(array, pos)\
    ((array)->elements + (array)->element_size * (pos))

#define ARRAY_NUM_BYTES(array, n)\
    ((array)->element_size * (n))


/* **** functions that implement SeeDynamicArray or override SeeObject **** */

static int
dynamic_array_init(
    SeeDynamicArray* array,
    const SeeDynamicArrayClass* cls,
    size_t elem_size,
    see_copy_func copy_func,
    see_init_func init_func,
    see_free_func free_func
    )
{
    const SeeObjectClass* obj_cls = (const SeeObjectClass*) cls;
    obj_cls->object_init((SeeObject*) array, (const SeeObjectClass*) cls);

    // Generally you could set some default values here.
    // The init loop can still override them when necessary.

    if (elem_size == 0)
        return SEE_INVALID_ARGUMENT;

    array->element_size = elem_size;
    array->size         = 0;
    array->capacity     = 0;
    array->copy_element = copy_func;
    array->init_element = init_func;
    array->free_element = free_func;
    array->elements     = NULL;

    return SEE_SUCCESS;
}

static int
init(const SeeObjectClass* cls, SeeObject* obj, va_list list)
{
    const SeeDynamicArrayClass* array_cls = (const SeeDynamicArrayClass*) cls;
    SeeDynamicArray* array = (SeeDynamicArray*) obj;

    size_t          elem_size = va_arg(list, size_t);
    see_copy_func   copy_func = va_arg(list, see_copy_func);
    see_init_func   init_func = va_arg(list, see_init_func);
    see_free_func   free_func = va_arg(list, see_free_func);

    if (copy_func == NULL)
        copy_func = memcpy;

    return array_cls->array_init(
        array, array_cls, elem_size, copy_func, init_func, free_func
        );
}

static void
dynamic_array_destroy(SeeObject* obj)
{
    const SeeDynamicArrayClass* cls = (const SeeDynamicArrayClass*)
        see_object_get_class(obj);
    SeeDynamicArray* array = (SeeDynamicArray*) obj;

    // clear the elements
    if (array) {
        if (array->free_element) {
            for (size_t i = 0; i < array->size; i++) {
                char** element_ptr = (char**) ARRAY_ELEM_ADDRESS(array, i);
                array->free_element(*element_ptr);
            }
        }
        free(array->elements);
    }

    // Let the parent destructor handle the rest.
    ((SeeObjectClass*)cls)->psuper->destroy(obj);
}

static void
array_set(SeeDynamicArray* array, size_t pos, const void* element)
{
    char* elem = ARRAY_ELEM_ADDRESS(array, pos);
    if (array->free_element)
        array->free_element( *((char**)elem));
    memcpy(elem, element, array->element_size);
}

static void*
array_get(const SeeDynamicArray* array, size_t pos)
{
    return ARRAY_ELEM_ADDRESS(array, pos);
}

static int
array_reserve(SeeDynamicArray* array, size_t n_elements)
{
    if (n_elements <= array->capacity)
        return SEE_SUCCESS;

    size_t num_bytes = ARRAY_NUM_BYTES(array, n_elements);
    char* new_mem = realloc(array->elements, num_bytes);
    if (new_mem == NULL)
        return SEE_RUNTIME_ERROR;

    array->elements = new_mem;
    array->capacity = n_elements;

    return SEE_SUCCESS;
}

static int
array_shrink_to_fit(SeeDynamicArray* array)
{
    if (array->size == array->capacity)
        return SEE_SUCCESS;

    size_t n_bytes = ARRAY_NUM_BYTES(array, array->size);
    char* new_mem = realloc(array->elements, n_bytes);
    if (new_mem == NULL)
        return SEE_RUNTIME_ERROR;

    array->elements = new_mem;
    array->capacity = array->size;

    return SEE_SUCCESS;
}

static int
array_add(SeeDynamicArray* array, const void* element)
{
    int result = SEE_SUCCESS;
    if (array->size < array->capacity) {
        array->copy_element(
            ARRAY_ELEM_ADDRESS(array, array->size),
            element,
            array->element_size
            );
        array->size++;
        return result;
    }
    else {
        if (array->capacity == 0)
            result = array_reserve(array, 1);
        else
            result = array_reserve(array, array->capacity * 2);
        if (result)
            return result;
        return array_add(array, element);
    }
}

static int
array_pop_back(SeeDynamicArray* array, void* element)
{
    array->copy_element(
        element,
        ARRAY_ELEM_ADDRESS(array, array->size-1),
        array->element_size
        );

    array->size--;

    return SEE_SUCCESS;
}

static int
array_resize(SeeDynamicArray* array, size_t n, void* init_data)
{
    const SeeDynamicArrayClass* cls =
        (SeeDynamicArrayClass*) see_object_get_class(
            (SeeObject*) array
            );

    if (n == array->size)
        return SEE_SUCCESS;
    else if (n < array->size)
        return cls->shrink(array, n);
    else
        return cls->grow(array, n, init_data);
}

static int
array_shrink(SeeDynamicArray* array, size_t size)
{
    const SeeDynamicArrayClass* cls =
        (SeeDynamicArrayClass*) see_object_get_class(
            (SeeObject*) array
            );

    if (array->free_element) {
        for (size_t i = size; i < array->size; i++) {
            void* element = cls->get(array, i);
            array->free_element(element);
        }
    }
    array->size = size;
    return SEE_SUCCESS;
}

static int
array_grow(SeeDynamicArray* array, size_t count, void* init_data)
{
    int success;
    const SeeDynamicArrayClass* cls =
        (SeeDynamicArrayClass*) see_object_get_class(
            (SeeObject*) array
        );

    success = cls->reserve(array, count);
    size_t diff = count - array->size;
    if (array->init_element) {
        for (size_t i = array->size; i < array->size + diff; i++) {
            void* element = cls->get(array, i);
            success = array->init_element(
                element,
                array->element_size,
                init_data
                );
            if (success != SEE_SUCCESS)
                return success;
        }
    }
    return success;
}

/* **** implementation of the public API **** */

int
see_dynamic_array_new(
    SeeDynamicArray**   array,
    size_t              element_size,
    see_copy_func       copy_func,
    see_init_func       init_func,
    see_free_func       free_func
    )
{
    const SeeObjectClass* cls =
        (const SeeObjectClass*) see_dynamic_array_class();

    if (!cls)
        return SEE_NOT_INITIALIZED;

    if (!array || *array)
        return SEE_INVALID_ARGUMENT;

    if (element_size == 0)
        return SEE_INVALID_ARGUMENT;

    return cls->new(
        cls,
        0,
        (SeeObject**) array,
        element_size,
        copy_func,
        init_func,
        free_func
        );
}

int
see_dynamic_array_new_capacity(
    SeeDynamicArray**   array,
    size_t              element_size,
    see_copy_func       copy_func,
    see_init_func       init_func,
    see_free_func       free_func,
    size_t              desired_capacity
    )
{
    const SeeDynamicArrayClass* array_cls = NULL;
    int ret = see_dynamic_array_new(
        array,
        element_size,
        copy_func,
        init_func,
        free_func
        );

    if (ret != SEE_SUCCESS)
        return ret;

    array_cls = (const SeeDynamicArrayClass*) see_object_get_class(
        (SeeObject*) *array
        );

    ret = array_cls->reserve(*array, desired_capacity);
    if (ret != SEE_SUCCESS) {
        see_object_decref((SeeObject *) *array);
        *array = NULL;
    }

    return ret;
}

size_t
see_dynamic_array_size(const SeeDynamicArray* array)
{
    return array->size;
}

size_t
see_dynamic_array_capacity(const SeeDynamicArray* array)
{
    return array->capacity;
}

int
see_dynamic_array_add(SeeDynamicArray* array, void* element)
{
    const SeeDynamicArrayClass* cls =
        (const SeeDynamicArrayClass*) see_object_get_class(
                (const SeeObject*) array
                );

    if (!array || !element)
        return SEE_INVALID_ARGUMENT;

    return cls->add(array, element);
}

void*
see_dynamic_array_get(const SeeDynamicArray* array, size_t index)
{
    const SeeDynamicArrayClass* cls =
        (const SeeDynamicArrayClass*) see_object_get_class(
                (const SeeObject*) array
                );

    return cls->get(array, index);
}

int
see_dynamic_array_set(SeeDynamicArray* array, size_t index, const void* element) {
    const SeeDynamicArrayClass *cls =
        (const SeeDynamicArrayClass *) see_object_get_class(
            (const SeeObject *) array
        );

    if (!array)
        return SEE_INVALID_ARGUMENT;

    if (index >= array->size)
        return SEE_INDEX_ERROR;

    cls->set(array, index, element);
    return SEE_SUCCESS;
}

int
see_dynamic_array_reserve(SeeDynamicArray* array, size_t n_elements)
{
    const SeeDynamicArrayClass *cls =
        (const SeeDynamicArrayClass *) see_object_get_class(
            (const SeeObject *) array
        );

    if (!cls)
        return SEE_NOT_INITIALIZED;

    if (!array)
        return SEE_INVALID_ARGUMENT;

    return cls->reserve(array, n_elements);
}

int
see_dynamic_array_shrink_to_fit(SeeDynamicArray* array)
{
    const SeeDynamicArrayClass *cls =
        (const SeeDynamicArrayClass *) see_object_get_class(
            (const SeeObject *) array
        );

    if (!cls)
        return SEE_NOT_INITIALIZED;

    if (!array)
        return SEE_INVALID_ARGUMENT;

    return cls->shrink_to_fit(array);
}

/* **** initialization of the class **** */

SeeDynamicArrayClass* g_SeeDynamicArrayClass = NULL;

static int see_dynamic_array_class_init(SeeObjectClass* new_cls) {
    int ret = SEE_SUCCESS;
    
    /* Override the functions on the parent here */
    new_cls->init   = init;
    new_cls->destroy= dynamic_array_destroy;
    /* Set the function pointers of the own class here */
    SeeDynamicArrayClass* cls = (SeeDynamicArrayClass*) new_cls;
    cls->array_init = dynamic_array_init;
    cls->set        = array_set;
    cls->get        = array_get;
    cls->add        = array_add;
    cls->pop_back   = array_pop_back;
    cls->resize     = array_resize;
    cls->reserve    = array_reserve;
    cls->shrink_to_fit = array_shrink_to_fit;
    cls->shrink     = array_shrink;
    cls->grow       = array_grow;
    
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

const SeeDynamicArrayClass*
see_dynamic_array_class()
{
    return g_SeeDynamicArrayClass;
}
