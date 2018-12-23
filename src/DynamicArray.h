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
    void       (*set)(SeeArray* array, size_t pos, const void* element);

    /**
     * \brief Ob
     * @param array
     * @param pos
     * @return
     */

    void*      (*get)(const SeeArray* array, size_t pos);

    int        (*add)(SeeArray* array, void* element);

    int        (*pop_back)(SeeArray* array, void *element);

    int        (*resize)(SeeArray* array, size_t count);

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
