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


#ifndef SEE_STACK_H
#define SEE_STACK_H

#include "SeeObject.h"
#include "Error.h"
#include "DynamicArray.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct SeeStack SeeStack;
typedef struct SeeStackClass SeeStackClass;

struct SeeStack {
    SeeObject parent_obj;

    /*expand SeeStack data here*/

    SeeDynamicArray* array;
};

struct SeeStackClass {
    SeeObjectClass parent_cls;
    
    int (*stack_init) (
        SeeStack*               stack,
        const SeeStackClass*    stack_cls,
        size_t                  element_size,
        see_copy_func           copy_func,
        see_init_func           init_func,
        see_free_func           free_func,
        SeeError**              error_out
        );
        
    /* expand SeeStack class with extra functions here.*/

    int (*top)(SeeStack* stack, void* element_out);

    int (*pop)(SeeStack* stack, SeeError* error_out);

    int (*push)(SeeStack* stack, const void* element, SeeError** error_out);
};

/* **** function style macro casts **** */

/**
 * \brief cast a pointer from a SeeStack derived instance back to a
 *        pointer to SeeStack.
 */
#define SEE_STACK(obj)                      \
    ((SeeStack*) obj)

/**
 * \brief cast a pointer to pointer from a SeeStack derived instance back to a
 *        reference to SeeStack*.
 */
#define SEE_STACK_REF(ref)                      \
    ((SeeStack**) ref)

/**
 * \brief cast a pointer to SeeStackClass derived class back to a
 *        pointer to SeeStackClass.
 */
#define SEE_STACK_CLASS(cls)                      \
    ((const SeeStackClass*) cls)

/**
 * \brief obtain a pointer to SeeStackClass from a instance of
 *        derived from SeeStack. This macro is preferably
 *        used when obtaining the class of a instance. When this
 *        macro is used. Calling methods on the class will enable
 *        polymorphism, because you'll get the derived class.
 */
#define SEE_STACK_GET_CLASS(obj)                \
    (SEE_STACK_CLASS(see_object_get_class(SEE_OBJECT(obj)) )  )

/* **** public functions **** */

/**
 * \brief Create a new Stack
 *
 * @param obj_out [out]     The newly created stack is returned here.
 * @param element_size [in] The size in bytes of a single element.
 * @param cp_func [in]      The function that can be used to copy an element
 *                          into the array.
 * @param init_func [in]    The function to initialize members after the
 *                          underlying dynamic array is resized.
 *                          Can be NULL.
 * @param free_func [in]    Frees elements from the array, this can be
 *                          handy when pointers to objects are inserted
 *                          to the array. May be NULL.
 * @param error_out [out]   A hopefully useful error is returned here if
 *                          something goes wrong.
 *
 * @return SEE_SUCCESS if everything works out, a non-zero value when an error
 *         has occurred.
 */
SEE_EXPORT int
see_stack_new(SeeStack**    obj_out,
              size_t        element_size,
              see_copy_func cp_func,
              see_init_func init_func,
              see_free_func free_func,
              SeeError**    error_out
              );

/**
 * \brief Pop the top item from the stack.
 *
 * Popping the top item from the stack decreases the stack by one. The stack
 * should not be emtpy.
 *
 * @param stack [in, out] The stack from which to pop the top item.
 * @param error_out[out]  An error will be returned here, when something goes
 *                        wrong.
 * @return SEE_SUCCESS when everything goes alright.
 */
SEE_EXPORT int
see_stack_pop(SeeStack* stack, SeeError** error_out);

/**
 * \brief Push a new item on the stack.
 *
 * By default a stack is created empty. When a item is pushed onto the stack
 * one can retrieve it by calling "see_stack_top()", then the top item is
 * retrieved from the stack.
 *
 * @param stack [in, out]   The stack on which to push a new element
 * @param new_element [in]  The element to copy on the stack.
 * @param error_out [out]   The a descriptive error about what went wrong.
 *
 * @return SEE_SUCCESS when everything is alright a non zero value otherwise.
 */
SEE_EXPORT int
see_stack_push(SeeStack* stack, const void* new_element, SeeError** error_out);

/**
 * Gets the pointer to the SeeStackClass table.
 */
SEE_EXPORT const SeeStackClass*
see_stack_class();

/* Expand the class with public functions here, don't forget the SEE_EXPORT
 * macro, because otherwise you'll run into troubles when exporting function
 * in a windows dll.
 */

/* **** class initialization functions **** */

/**
 * Initialize SeeStack; make it ready for use.
 */
SEE_EXPORT
int see_stack_init();

/**
 * Deinitialize SeeStack, after SeeStack has been deinitialized,
 * all functions in this header shouldn't be used anymore.
 */
SEE_EXPORT
void see_stack_deinit();

#ifdef __cplusplus
}
#endif

#endif //ifndef SEE_STACK_H
