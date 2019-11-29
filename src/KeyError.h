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


#ifndef SEE_KEY_ERROR_H
#define SEE_KEY_ERROR_H

#include "Error.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct SeeKeyError SeeKeyError;
typedef struct SeeKeyErrorClass SeeKeyErrorClass;

struct SeeKeyError {
    SeeError parent_obj;
    /*expand SeeKeyError data here*/
        
};

struct SeeKeyErrorClass {
    SeeErrorClass parent_cls;
    
    int (*key_error_init)(
        SeeError*               key_error,
        const SeeKeyErrorClass* key_error_cls,
        const char*             key
        );
        
};

/* **** function style macro casts **** */

/**
 * \brief cast a pointer from a SeeKeyError derived instance back to a
 *        pointer to SeeKeyError.
 */
#define SEE_KEY_ERROR(obj)                      \
    ((SeeKeyError*) obj)

/**
 * \brief cast a pointer to pointer from a SeeKeyError derived instance back to a
 *        reference to SeeKeyError*.
 */
#define SEE_KEY_ERROR_REF(ref)                      \
    ((SeeKeyError**) ref)

/**
 * \brief cast a pointer to SeeKeyErrorClass derived class back to a
 *        pointer to SeeKeyErrorClass.
 */
#define SEE_KEY_ERROR_CLASS(cls)                      \
    ((const SeeKeyErrorClass*) cls)

/**
 * \brief obtain a pointer to SeeKeyErrorClass from a instance of
 *        derived from SeeKeyError. This macro is preferably
 *        used when obtaining the class of a instance. When this
 *        macro is used. Calling methods on the class will enable
 *        polymorphism, because you'll get the derived class.
 */
#define SEE_KEY_ERROR_GET_CLASS(obj)                \
    (SEE_KEY_ERROR_CLASS(see_object_get_class(SEE_OBJECT(obj)) )  )

/* **** public functions **** */

/**
 * \brief Create a new KeyError instance.
 *
 * @param [out] obj_out A new key error instance
 * @param [in]  key     A key in string format that wasn't found.
 *
 * @return SEE_SUCCESS hopefully.
 */
SEE_EXPORT int
see_key_error_new(SeeError** obj_out, const char* key);

/**
 * Gets the pointer to the SeeKeyErrorClass table.
 */
SEE_EXPORT const SeeKeyErrorClass*
see_key_error_class();

/* Expand the class with public functions here, don't forget the SEE_EXPORT
 * macro, because otherwise you'll run into troubles when exporting function
 * in a windows dll.
 */

/* **** class initialization functions **** */

/**
 * Initialize SeeKeyError; make it ready for use.
 */
SEE_EXPORT
int see_key_error_init();

/**
 * Deinitialize SeeKeyError, after SeeKeyError has been deinitialized,
 * all functions in this header shouldn't be used anymore.
 */
SEE_EXPORT
void see_key_error_deinit();

#ifdef __cplusplus
}
#endif

#endif //ifndef SEE_KEY_ERROR_H
