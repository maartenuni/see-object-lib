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


#ifndef SeeClass_H
#define SeeClass_H

#include "SeeObject.h"

#ifdef __cplusplus
extern "C" {
#endif

struct SeeClass;
typedef struct _SeeClass SeeClass;

struct _SeeClass {
    const SeeObject _;
    const char*     class_name;
    size_t          instance_size;
    void*         (*new_instance)(SeeClass* cls, size_t size);
    void*         (*init)(SeeObject* self, ...);
    void          (*destroy) (SeeClass* cls);
    //int           (*differ)(SeeObject* self, SeeObject* other);
};

/**
 * @brief Allocate a new class instance.
 * @param class SeeClass* to the class
 * @param ... Name of the class, size of the instances
 * @return
 */
void* see_class_new_instance(SeeClass* class, ...);

/**
 * @brief Initialize a new class instance.
 *
 * @param [in] class A pointer to a SeeClass derived Class
 * @param [in] ... An intializer list with a pointer to a function that should
 *                 start with the size of an object, then the name of a class
 *                 The last element should be 0 to indicate that there are
 *                 no more elements coming.
 * @return The newly initialized class instance.
 */
 void* see_class_init(SeeClass* self, SeeObject* instance);


 /**
  * @brief Destroy a class.
  *
  * @param class
  */
 void see_class_destroy(SeeClass* class);

#ifdef __cplusplus
}
#endif

#endif //ifndef SeeClass_H
