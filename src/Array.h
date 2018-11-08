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

#ifdef __cplusplus
extern "C" {
#endif

struct _SeeArray;
typedef struct _SeeArray SeeArray;
struct _SeeArrayClass;
typedef struct _SeeArrayClass SeeArrayClass;

struct _SeeArray {
     SeeObj     obj;
     size_t     element_size;
     size_t     capacity;
     size_t     size;
     void       (*set)(SeeArray* array, void* element);
     void       (*free_element)(void* element);
};

#ifdef __cplusplus
}
#endif

#endif //ifndef Array_H
