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
 * \file see_functions.h
 *
 * This header defines prototypes for frequently used functions
 */

#ifndef see_functions_H
#define see_functions_H

#include <stdlib.h>
#include "see_export.h"
#include "Error.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * A typedef for a copying function where for example memcpy could
 * be used. Notice, memcpy generally requires that memory locations
 * do not overlap.
 *
 * @param [out] dest  The location to which we are copying
 * @param [in]  src   The location from which we are copying
 * @param [in]  n     The number of bytes to copy.
 *
 * @return a pointer to dest.
 */
typedef void* (*see_copy_func)(void* dest, const void* src, size_t n);

/**
 * typedef for a function that is designed to free a member.
 */
typedef void (*see_free_func)(void* free_me);

/**
 * \brief initializes a new object. This function can be used for example
 * when a array resizes to initialize the newly allocated objects.
 */
typedef int (*see_init_func)(void* element, size_t num_bytes, void* data);

/**
 * \brief compares two opaque objects.
 *
 * This function can be used to compare two object. It computes *rhs - *lhs,
 * hence if the result is 0 the two objects are equal, if the result is positive
 * lhs < rhs and if the result negative, than lhs > rhs. This type of function
 * has to be implemented for ordering in data structures or sorting.
 * Generally the user has to implement this for a given data type.
 * For example in order to sort cards:
 * \code {.c}
 * enum color {CLUBS, DIAMONDS, HEARTS, SPADES};
 * typedef struct Card {
 *    color color;
 *    int   rank; 1 - 10 and 11 - 14 Jack, Queen, King, Ace
 * } Card
 * \endcode
 * A see_cmp_function might be implemented like:
 * \code{.c}
 * int my_color_above_rank_cmp(const void* lhs, const void* rhs)
 * {
 *     const Card* c_lhs = lhs;
 *     const Card* c_rhs = lhs;
 *     int cmp = c_rhs->color - c_lhs->color;
 *     if (cmp) // if cmp != 0 colors are unequal
 *         return cmp;
 *      c_rhs->rank - c_lhs->rank;
 *      return cmp;
 * }
 * \endcode
 * If one would compare them with rank first, it might be
 */
typedef int (*see_cmp_func)(const void* lhs, const void* rhs);

/**
 * @brief Copy a seeobject by copying the pointer and increasing the reference
 *        count.
 *
 * @param dest_seeobj
 * @param src_seeobj
 * @param unused
 *
 * @return dest_seeobj
 */
SEE_EXPORT void*
see_copy_by_ref(void* dest_seeobj, const void* src_seeobj, size_t unused);

/**
 * \brief initialize the bytes to a given bytes or 0.
 *
 * @param [in, out] the object to initialize
 * @param [in]      nbytes
 * @param [in]      bytes NULL, or a pointer to a byte, if NULL memset
 *                  will initialize the bytes to '0'.
 *
 * @return SEE_SUCCESS.
 */
SEE_EXPORT int
see_init_memset(void* object, size_t nbytes, void* byte);

/**
 * \brief A function that will free a see object from a pointer.
 *
 * Sometimes it might be handy to have a function that is able to destroy
 * another see_object, for instance when a container object is destroy, it
 * might want to see_object_decref() all instances it contains. This function
 * does exactly that.
 *
 * @param see_object, this must be a see_object.
 */
SEE_EXPORT void
see_free_see_object(void* see_object);


#ifdef __cplusplus
}
#endif

#endif //ifndef see_functions_H
