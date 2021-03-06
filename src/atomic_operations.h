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
 * \file atomic_operations.h
 *
 * One could use the functions from this header, but... I consider them
 * to be a private part, because it is to low level..
 *
 * \brief implements atomic reference counting.
 *
 * \private
 */

#ifndef SeeAtomicOperations_H
#define SeeAtomicOperations_H

#include <stdlib.h>
#include "see_export.h"


#ifdef __cplusplus
extern "C" {
#endif

/**
 * Increments the integer pointed to by val with 1 and returns the value
 * atomically
 *
 * *val is incremented by one and the value resulting after the increment
 * is return to the user
 *
 * @return *val + 1
 */
SEE_EXPORT int see_atomic_increment(int* val);

/**
 * Decrements the integer pointed to by val with 1 and returns the value
 * atomically
 *
 * *val is decremented by one and the value resulting after the decrement
 * is return to the user
 *
 * @return *val - 1
 */
SEE_EXPORT int see_atomic_decrement(int* val);

/**
 * Increments the integer pointed to by val with 1 and returns the value
 * atomically
 *
 * *val is incremented by one and the value resulting after the increment
 * is return to the user
 *
 * @return *val + 1
 */
SEE_EXPORT int see_atomic_increment_by(int* val, int n);
 
/**
 * Decrements the integer pointed to by val with 1 and returns the value
 * atomically
 *
 * *val is decremented by one and the value resulting after the decrement
 * is returned to the user
 * @return *val - 1
 */
SEE_EXPORT int see_atomic_decrement_by(int* val, int n);


/**
 * Fetches an integer atomically.
 *
 * If an int is not aligned to platform specific an int might not be
 * read atomically, other platforms might not support it at all.
 * This function will read the value atomically.
 *
 * @return *val atomically.
 */
SEE_EXPORT int
see_atomic_fetch(int* val);

#ifdef __cplusplus
}
#endif

#endif //ifndef SeeObject_H

