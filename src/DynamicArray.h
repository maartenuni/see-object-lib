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


#ifndef SEE_DYNAMIC_ARRAY_H
#define SEE_DYNAMIC_ARRAY_H

#include <stdlib.h>
#include "SeeObject.h"
#include "see_functions.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _SeeDynamicArray SeeDynamicArray;
typedef struct _SeeDynamicArrayClass SeeDynamicArrayClass;

struct _SeeDynamicArray {
    SeeObject   parent_obj;
    size_t      element_size;
    size_t      size;
    size_t      capacity;
    char*       elements;

    /**
     * \private
     * \brief frees elements from the array.
     * \param element [in] The element to be freed from the array.
     */
    void        (*free_element)(void* element);

    /**
     * \private
     * \brief initializes new elements.
     *
     * @param element, the new element added to the array.
     * @param nbytes, the number of bytes that need to be initialized.
     * @param data a pointer to data that the init function needs to initialize.
     *
     * @return The function should return SEE_SUCCESS, any other value
     *        will stop the resize function and resize will return this value.
     */
     int        (*init_element)(void* element, size_t nbytes, void* data);

    /**
     * \brief copies a element into the array.
     * @param element [in] a pointer to the element to be copied into the array.
     */
    void*       (*copy_element)(void* destination, const void* source, size_t n);
};

struct _SeeDynamicArrayClass {
    SeeObjectClass parent_cls;

    int (*array_init)(
        SeeDynamicArray*            array,
        const SeeDynamicArrayClass* array_cls,
        size_t                      element_size,
        see_copy_func               copy_func,
        see_init_func               init_func,
        see_free_func               free_func
        );

    /**
     * \brief Set the element at index pos to the value of element
     *
     * @param array [in, out]   Non NULL pointer to array in which we would like
     *                          to set element[pos] to the value of element.
     * @param pos[in]           The index of the value we want to set to the
     *                          value of element.
     * @param element[in]       Element a non NULL pointer to an element
     *                          whose value we want to insert into the array.
     *                          The copy function that was specified while
     *                          creating the array will be used to copy the
     *                          member into the array.
     */
    void       (*set)(SeeDynamicArray* array, size_t pos, const void* element);

    /**
     * \brief Get a pointer to the item at pos
     *
     * @param array [in]    A non NULL pointer to the array from which we would
     *                      like to obtain an element
     * @param pos[IN]       The index of the item to which we would like to
     *                      retrieve a pointer
     *
     * @return              The pointer to the element stored inside of the
     *                      array. Note that you'll still have to dereference
     *                      the item in order to use it.
     */
    void*      (*get)(const SeeDynamicArray* array, size_t pos);

    /**
     * \brief Add a new item to the back of the array.
     *
     * This function adds one element to the end of the array. This can usually
     * be done very quickly. Sometimes the capacity of the array must be changed
     * as well, than the performance is a bit slower. If the function succeeds
     * The last item of the array will be the one inserted and the size will
     * be increased by one. If it was necessary, the capacity has increased.
     *
     * @param array[in, out]    A non NULL pointer to a array to which we would
     *                          like to copy a new item.
     * @param element[in]       The item that is going to be appended to the
     *                          end of the array.
     *
     * @return  see_success when the item can be successfully appended
     *          to the end of the array.
     */
    int        (*add)(SeeDynamicArray* array, const void* element);

    /**
     * \brief   pop an array from the back of the array and store the value
     *          in element.
     *
     * One can append, but also shrink arrays. When the array is not empty
     * the size of the array will be decreased by one. Optionally, the
     * item that is popped from the array, can be saved at the pointer of element.
     * When the size of the array shrinks to half of the capacity the
     * preallocated space, the array shrinks to size.
     *
     * @param array[in, out]    A non NULL pointer to a SeeDynamicArray
     * @param element[out]      A pointer to store the element in that is
     *                          popped from the array.
     * @return 0 when the operation was successful.
     */
    int        (*pop_back)(SeeDynamicArray* array, void *element);

    /**
     * \brief grow or shrink to the size specified by count
     * @param array the array that is going to be resized.
     * @param count The desired number of elements after the resize operation
     * @param initdata Data for the initialization function
     *                 may be NULL, but depends on the init_data_func.
     *                 and it is probably only used when the desired size
     *                 is larger than the current size.
     * @return SEE_SUCCESS when everything is alright, another value if not.
     */
    int        (*resize)(SeeDynamicArray* array, size_t count, void* initdata);

    /**
     * \brief allocate space for a number of elements.
     * @param array [in,out] The new array to initialize.
     * @param nelements      The number of elements to reserve room for.
     *
     * @return
     */
    int        (*reserve)(SeeDynamicArray* array, size_t nelements);

    /**
     * \brief tries to shrink the array when you think it reserves more
     * memory than it needs.
     *
     * @return SEE_SUCCESS when the operation was successful.
     */
    int        (*shrink_to_fit)(SeeDynamicArray* array);

    /**
     * \brief insert new elements into the array at the nth index.
     * @param array
     * @param pos
     * @param elements
     * @param n
     * @return
     */

    int        (*insert)(
        SeeDynamicArray* array,
        size_t pos,
        void* elements,
        size_t n
    );

    // private  use resize instead.
    int        (*shrink)(SeeDynamicArray* array, size_t nelements);

    /**
     * \brief grows array to have n elements.
     * \private
     *
     * @param array
     * @param num_elements
     * @param init_data
     * @return
     */
    int        (*grow)(
        SeeDynamicArray* array,
        size_t num_elements,
        void* init_data
        );
};


/* **** public functions **** */

/**
 * \brief create a new empty dynamic array.
 *
 * @param [out] out A pointer to a pointer to array, *array should be NULL,
 *                  but array shouldn't.
 * @param [in] element_size The array isn't a C++ template, hence the array
 *                          doesn't know in advance how large the elements are
 *                          but it is important to know.
 * @param [in] copy_func A copy function copies a element into the array
 *                       This may be NULL, then memcpy will be used to
 *                       copy a new element into the array.
 * @param [in] init_func When the array is resized, not only the memory
 *                       must be big enough to hold all members, but added
 *                       members can also be initialized using this function
 *                       as callback. This may be NULL, then no function
 *                       will be called when the array is resized.
 * @param [in] free_func When an element is removed from the array, it might
 *                       be necessary to call a freeing function. If no
 *                       free function is specified (NULL), no function
 *                       will be called, otherwise this function will be used
 *                       to free the resources of such element.
 * @return SEE_SUCCESS if a new array is successfully created.
 */
SEE_EXPORT int
see_dynamic_array_new(
    SeeDynamicArray** out,
    size_t          element_size,
    see_copy_func   copy_func,
    see_init_func   init_func,
    see_free_func   free_func
    );

/**
 * \brief create a new array with preallocated size.
 *
 * @param [out] out             see doc for "see_dynamic_array_new()"
 * @param [in]  element_size    see doc for "see_dynamic_array_new()"
 * @param [in]  copy_func       see doc for "see_dynamic_array_new()"
 * @param [in]  init_func       see doc for "see_dynamic_array_new()"
 * @param [in]  free_func       see doc for "see_dynamic_array_new()"
 * @param [in]  capacity    This capacity is used to preallocate the size
 *                          of the memory.
 * @return SEE_SUCCES or another value indicating what went wrong.
 */
SEE_EXPORT int
see_dynamic_array_new_capacity(
    SeeDynamicArray**   out,
    size_t              element_size,
    see_copy_func       copy_func,
    see_init_func       init_func,
    see_free_func       free_func,
    size_t              capacity
    );

/**
 * \brief return the current size of the array.
 * @param array
 * @return the size of the array.
 */
SEE_EXPORT size_t
see_dynamic_array_size(const SeeDynamicArray* array);

/**
 * \brief return the current capacity of the array.
 *
 * The capacity of the array is the number of elements that can be stored inside
 * the array without allocating more memory.
 *
 * @param array
 * @return the number of elements that are preallocated.
 */
SEE_EXPORT size_t
see_dynamic_array_capacity(const SeeDynamicArray* array);

/**
 * \brief Append the array with an additional item.
 *
 * @param array The array to append to
 * @param element The element that must be copied into the array.
 *
 * @return SEE_SUCCESS or an error value.
 */
SEE_EXPORT int
see_dynamic_array_add(SeeDynamicArray* array, void* element);

/**
 * \brief Get a pointer to an element inside of the array.
 *
 * @param [in] array the array from which to get a pointer.
 * @param [in] index a zero based index.
 *
 * @return A pointer into the array. So if index is 0 the first item
 *         in the array is returned, if you cast that to a pointer
 *         desired type, you can index it like a regular c-type array.
 *         Note that the return value should be dereferenced before use.
 */
SEE_EXPORT void*
see_dynamic_array_get(const SeeDynamicArray* array, size_t index);

/**
 * \brief set a element into the array.
 *
 * @param [in]array the array in witch we would like to set a new element.
 * @param [in]index the index (0-based) into the array on which position
 *                  we should set a new value. If the array has a free_function
 *                  the element sitting at that index should be freed.
 * @param element   the new element to be inserted.
 * return SEE_SUCCESS if everything is alright.
 */
SEE_EXPORT int
see_dynamic_array_set(SeeDynamicArray* array, size_t index, const void* element);

/**
 * Gets the pointer to the SeeDynamicArrayClass table.
 */
SEE_EXPORT const SeeDynamicArrayClass*
see_dynamic_array_class();


/* Expand the class with public functions here, don't forget the SEE_EXPORT
 * macro, because otherwise you'll run into troubles when exporting function
 * in a windows dll.
 */

/* **** class initialization functions **** */

/**
 * Initialize SeeDynamicArray; make it ready for use.
 */
SEE_EXPORT
int see_dynamic_array_init();

/**
 * Deinitialize SeeDynamicArray, after SeeDynamicArray has been deinitialized,
 * all functions in this header shouldn't be used anymore.
 */
SEE_EXPORT
void see_dynamic_array_deinit();

#ifdef __cplusplus
}
#endif

#endif //ifndef SEE_DYNAMIC_ARRAY_H
