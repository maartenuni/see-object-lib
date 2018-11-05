/*
 * This file is part of see-objects.
 *
 * see-objects is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * see-objects is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with see-objects.  If not, see <http://www.gnu.org/licenses/>.
 */


#ifndef SeeAtomicOperations_H
#define SeeAtomicOperations_H

#include <stdlib.h>

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
int see_atomic_increment(int* val);

/**
 * Decrements the integer pointed to by val with 1 and returns the value
 * atomically
 *
 * *val is decremented by one and the value resulting after the decrement
 * is return to the user
 *
 * @return *val - 1
 */
int see_atomic_decrement(int* val);

/**
 * Increments the integer pointed to by val with 1 and returns the value
 * atomically
 *
 * *val is incremented by one and the value resulting after the increment
 * is return to the user
 *
 * @return *val + 1
 */
int see_atomic_increment_by(int* val, int n);
 
/**
 * Decrements the integer pointed to by val with 1 and returns the value
 * atomically
 *
 * *val is decremented by one and the value resulting after the decrement
 * is returned to the user
 * @return *val - 1
 */
int see_atomic_decrement_by(int* val, int n);


#ifdef __cplusplus
}
#endif

#endif //ifndef SeeObject_H

