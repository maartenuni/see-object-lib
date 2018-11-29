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
#include <stdarg.h>

#include "see_export.h"
#include "errors.h"

#ifdef __cplusplus
extern "C" {
#endif

struct _SeeObject;
typedef struct _SeeObject SeeObject;
struct _SeeObjectClass;
typedef struct _SeeObjectClass SeeObjectClass;

struct _SeeClass;

/**
 * The definition of a SeeObject.
 *
 * This object mainly contains the data of a SeeObject.
 */
struct _SeeObject {
    const SeeObjectClass*   cls;
    int                     refcount;
};

enum see_object_init_args {
    /**
     * \brief Used to indicate that no more arguments are expected.
     * To initialize the see object, expects no argument.
     */
    SEE_OBJECT_INIT_FINAL,

    /**
     * Mainly here to dictate what is the end and is used by children
     * of the See object class to see what is their first initializer value.
     */
    SEE_OBJECT_INIT_SENTINAL
};

/**
 * Table with all the class attributes of a SeeObject.
 *
 * Where a SeeObject contains mainly data, its class object/type table
 * merely exists of methods that operate on the object. Although it is 
 * certainly possible to put class attribute values/data on a class too. 
 */
struct _SeeObjectClass {
    /** Classes are SeeObjects too */
    SeeObject obj;

    /** Classes except for SeeObject class have a parent/super class.*/
    const SeeObjectClass *psuper;

    /** The size of an instance */
    size_t inst_size;  ///< size of an instance

    /**
     * \brief create a new object instance.
     * @param[in]  cls The class for which we want to generate an instance
     * @param[out] obj The new uninitialized object will be stored in out.
     * @param ...   instance specific arguments.
     * @return  SEE_SUCCESS, SEE_INVALID_ARGUMENT or SEE_RUNTIME_ERROR
     */
    int (*new)(
        const SeeObjectClass*   cls,
        SeeObject**             obj,
        ...
        );

    /**
     * \brief initialize a new instance.
     * @param [in] cls The class that is initializing this new object.
     * @param [in] obj The new instance to be initialized
     * @param list instance specific arguments.
     * @return  SEE_SUCCESS when everything is alright.
     */
    int (*init)(
        const SeeObjectClass* cls,
        SeeObject* obj,
        va_list list
        );

    /** A function that destroys and frees instance */
    void (*destroy)(SeeObject* obj);

    /** representation of an object.*/
    int (*repr)(const SeeObject* obj, char* out, size_t size);

    /** Reference increment function.*/
    void* (*incref)(SeeObject* obj);

    /** reference decrement function.*/
    void (*decref)(SeeObject* obj);
};

/**
 * Creates a new object for any SeeObject derived class.
 */
SEE_EXPORT int see_object_new(const SeeObjectClass* cls, SeeObject** out);

/**
 * Allocates a new SeeObject instance.
 *
 * This is short for see_object_new(see_object_class());
 */
SEE_EXPORT SeeObject* see_object_create();


/**
 * Get the class of a SeeObject.
 *
 * Returns a pointer to the SeeObject of the class instance. Note in contrast
 * to the functions see_object_class() also in this header, this function
 * Doesn't return a the SeeObjectClass instance, but the class of the SeeObject
 * that is called with this function. So it will only return the SeeObject class
 * when it is called on a SeeObject, otherwise it will return the derived class.
 *
 * @param object The SeeObject (derived) instance of which we would like to
 *               have the class.
 *
 * \returns The class of the current object. If necessary the return value of
 * this function needs to be cast to the proper SeeObjectClass derived class.
 */
SEE_EXPORT const SeeObjectClass* see_object_get_class(const SeeObject* obj);

/**
 * Atomically increment the reference count of a see object
 *
 * The memory allocation/freeing strategy of SeeObjects is mainly done via
 * reference counting. When a object is created it has a reference count of
 * 1. When the reference count drops to zero, an object is freed, and thereafter
 * not available anymore.
 */
SEE_EXPORT void* see_object_ref(SeeObject* obj);

/**
 * Atomically decrement the reference count of a see object.
 *
 * This function decrements the reference count of a see object by one. If
 * The reference count drops to 0, this means no one is referring to the
 * object. Hence, the object will be destroyed and its resources will
 * be given back to the system.
 *
 * \param[in] A pointer to a valid instance of a SeeObject.
 */
SEE_EXPORT void see_object_decref(SeeObject* obj);


// A short standard representation of an object.
SEE_EXPORT int see_object_repr(const SeeObject* obj, char* out, size_t size);

/*** class management ***/
SEE_EXPORT int see_object_class_init();

/**
 * Get the class thé SeeObject.
 */
SEE_EXPORT const SeeObjectClass* see_object_class();

#ifdef __cplusplus
}
#endif

#endif //ifndef SeeObject_H
