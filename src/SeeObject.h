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


#ifndef SeeObject_H
#define SeeObject_H

#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

struct _SeeObject;
typedef struct _SeeObject SeeObject;
struct _SeeObjectClass;
typedef struct _SeeObjectClass SeeObjectClass;

struct _SeeObject {
    SeeObjectClass *cls;
    int             refcount;
};

typedef int     (*reprfunc)(const SeeObject* obj, char* out, size_t size);
typedef int     (*initfunc)(SeeObject* obj, SeeObjectClass* cls);
typedef void    (*destroyfunc)(SeeObject* obj);
typedef void*   (*reffunc)(SeeObject* obj);
typedef void    (*unreffunc)(SeeObject* obj);

struct _SeeObjectClass {
    SeeObject       obj;        ///< Classes are See Objects to.
    SeeObjectClass *psuper;     ///< Classes generally have a super class
                                //   except for SeeObjectClass it self.
    size_t          inst_size;  ///< size of an instance
    initfunc        init;       ///< a function that initializes an instance
    destroyfunc     destroy;    ///< a function that destroys and frees instance
    reprfunc        repr;       ///< representation of an object.
    reffunc         incref;     ///< reference increment function.
    unreffunc       decref;     ///< reference decrement function.
};

// Allocates a new instance for class cls.
SeeObject* see_object_new(SeeObjectClass* cls);
SeeObject* see_object_create();
SeeObjectClass* see_object_class();

/**
 * Atomically increment the reference count of a see object
 */
void* see_object_ref(SeeObject* obj);

/**
 * Atomically decrement the reference count of a see object.
 *
 * This function decrements the reference count of a see object by one. If
 * The reference count drops to 0, this means no one is refering to the
 * object. Hence, the object will be destroyed and its resources will
 * be given back to the system.
 *
 * \param[in] A pointer to a valid instance of a SeeObject.
 */
void see_object_decref(SeeObject* obj);


// A short standard representation of an object.
int see_object_repr(const SeeObject* obj, char* out, size_t size);


#ifdef __cplusplus
}
#endif

#endif //ifndef SeeObject_H
