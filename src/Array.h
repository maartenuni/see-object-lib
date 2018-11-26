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


#ifndef Array_H
#define Array_H

#include "see_export.h"
#include "SeeObject.h"
#include "see_functions.h"

#ifdef __cplusplus
extern "C" {
#endif

struct _SeeArray;
typedef struct _SeeArray SeeArray;
struct _SeeArrayClass;
typedef struct _SeeArrayClass SeeArrayClass;

struct _SeeArray {
     SeeObject  obj;
     size_t     element_size;
     size_t     size;
     size_t     capacity;
     void*      elements;

     /**
      * \brief is called whenever the array feels that an item must be removed.
      * @param element A pointer to the item that is removed from the array.
      */
     void       (*free_element)(void* element);

     /**
      * @brief This function is used to copy a new item into the array.
      * @param element a pointer to the element that should be copied into the
      *                array.
      * @return a pointer to the copy.
      */
     void*      (*copy_element)(void* element);
};

struct _SeeArrayClass {
    SeeObjectClass parent;

    /** the equivalent of the array[i] = element. */
    void       (*set)(SeeArray* array, size_t pos, const void* element);

    /** the equivalent of the element = array[i]. */
    void*      (*get)(const SeeArray* array, size_t pos);

    /**
     * pushes one element to the back of the array, size is incremented with 1
     */
    int        (*add)(SeeArray* array, void* element);

    /**
     * Get the last element from the array and store it in element.
     * Decreases the size with 1.
     */
    int        (*pop_back)(SeeArray* array, void *element);

    /**
     * Resizes the array to count
     *
     * If count is smaller than array, the array is shrunken to size and the
     * elements of which the index is >= count will be freed.
     */
    int        (*resize)(SeeArray* array, size_t count);

    /**
     * Insert a new number of elements into the array.
     *
     * The array size is larger then grow array till the given size is reached.
     * If the new array size is smaller shrink the array freeing all member at
     * an index greater than the new size.
     */
    int        (*insert)(
                   SeeArray* array,
                   size_t pos,
                   void* elements,
                   size_t n
                   );

    // private  use resize instead.
    void       (*shrink)(SeeArray* array, size_t nelements);
    int        (*grow)(SeeArray* array, size_t nelements);
};


/**
 * \brief create a new dynamic array that grows and shrinks when it is needed.
 *
 * @param [in] element_size The bytes of each element in the array
 * @param [in] cf           The function used to copy new items in the array.
 *                          may be NULL, then memcpy will be used to copy
 *                          new items into the array.
 * @param [in] ff           May be NULL, this is a function used to
 *                          free members from the array, this function can be
 *                          handy when eg pointers to objects are used.
 * @return A freshly generated dynamic array or NULL when there is no memory
 *         for it.
 */
SeeArray*
see_array_create(size_t element_size, see_copy_func cf, see_free_func ff);

void
see_array_set(SeeArray* array, size_t pos, const void* element);


#ifdef __cplusplus
}
#endif

#endif //ifndef Array_H
