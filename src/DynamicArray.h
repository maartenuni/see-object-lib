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

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _SeeDynamicArray SeeDynamicArray;
typedef struct _SeeDynamicArrayClass SeeDynamicArrayClass;

struct _SeeDynamicArray {
    SeeObject   parent_obj;
    size_t      element_size;
    size_t      capacity;
    void*       elements;

    /**
     * \private
     * \brief frees elements from the array.
     * \param element [in] The element to be freed from the array.
     */
    void      (*free_element)(void* element);

    /**
     * \brief copies a element into the array.
     * @param element [in] a pointer to the element to be copied into the array.
     */
    void      (*copy_element)(void* element);
};

struct _SeeDynamicArrayClass {
    SeeObjectClass parent_cls;

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
    int        (*add)(SeeDynamicArray* array, void* element);

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
     * @param array
     * @param count
     * @return
     */
    int        (*resize)(SeeDynamicArray* array, size_t count);

    int        (*insert)(
        SeeDynamicArray* array,
        size_t pos,
        void* elements,
        size_t n
    );

    // private  use resize instead.
    void       (*shrink)(SeeDynamicArray* array, size_t nelements);
    int        (*grow)(SeeDynamicArray* array, size_t nelements);
};


/*
 * See Objects are initialized with a var_args list. Typically,
 * The init func sees one of the values below and then expects the value
 * that initializes the class.
 */
enum SeeDynamicArrayInitValues {

    /**
     * \brief SEE_DYNAMIC_ARRAY_INIT_FIRST document here what kind of member
     * the initializer function should expect.
     *
     * NB remove this comment and member to replace it with your own!!!
     */
    SEE_DYNAMIC_ARRAY_INIT_FIRST = SEE_OBJECT_INIT_SENTINAL,
     

    /**
     * \brief SEE_DYNAMIC_ARRAY_INIT_FINALE init func expects no arguments
     * it exists to tell the initializer function to stop expecting arguments
     * for the current class.
     */
    SEE_DYNAMIC_ARRAY_INIT_FINAL,
    
    /**
     * \brief Mainly used as a starting point for deriving classes.
     */
    SEE_DYNAMIC_ARRAY_INIT_SENTINAL
}; 

/* **** public functions **** */

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
