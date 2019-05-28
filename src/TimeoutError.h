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


#ifndef SEE_TIMEOUT_ERROR_H
#define SEE_TIMEOUT_ERROR_H

#include "Error.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _SeeTimeoutError SeeTimeoutError;
typedef struct _SeeTimeoutErrorClass SeeTimeoutErrorClass;

struct _SeeTimeoutError {
    SeeError parent_obj;
    /*expand SeeTimeoutError data here*/
        
};

struct _SeeTimeoutErrorClass {
    SeeErrorClass parent_cls;
    
    int (*timeout_error_init)(
        SeeTimeoutError*            timeout_error,
        const SeeTimeoutErrorClass* timeout_error_cls
        /* Put instance specific arguments here and remove this comment. */
        );
        
    /* expand SeeTimeoutError class with extra functions here.*/
};

/* **** function style macro casts **** */

/**
 * \brief cast a pointer from a SeeTimeoutError derived instance back to a
 *        pointer to SeeTimeoutError.
 */
#define SEE_TIMEOUT_ERROR(obj)                      \
    ((SeeTimeoutError*) obj)

/**
 * \brief cast a pointer to pointer from a SeeTimeoutError derived instance back to a
 *        reference to SeeTimeoutError*.
 */
#define SEE_TIMEOUT_ERROR_REF(ref)                      \
    ((SeeTimeoutError**) ref)

/**
 * \brief cast a pointer to SeeTimeoutErrorClass derived class back to a
 *        pointer to SeeTimeoutErrorClass.
 */
#define SEE_TIMEOUT_ERROR_CLASS(cls)                      \
    ((const SeeTimeoutErrorClass*) cls)

/**
 * \brief obtain a pointer to SeeTimeoutErrorClass from a instance of
 *        derived from SeeTimeoutError. This macro is preferably
 *        used when obtaining the class of a instance. When this
 *        macro is used. Calling methods on the class will enable
 *        polymorphism, because you'll get the derived class.
 */
#define SEE_TIMEOUT_ERROR_GET_CLASS(obj)                \
    (SEE_TIMEOUT_ERROR_CLASS(see_object_get_class(SEE_OBJECT(obj)) )  )

/* **** public functions **** */

SEE_EXPORT int
see_timeout_error_new(SeeError** error);

/**
 * Gets the pointer to the SeeTimeoutErrorClass table.
 */
SEE_EXPORT const SeeTimeoutErrorClass*
see_timeout_error_class();

/* Expand the class with public functions here, don't forget the SEE_EXPORT
 * macro, because otherwise you'll run into troubles when exporting function
 * in a windows dll.
 */

/* **** class initialization functions **** */

/**
 * Initialize SeeTimeoutError; make it ready for use.
 */
SEE_EXPORT
int see_timeout_error_init();

/**
 * Deinitialize SeeTimeoutError, after SeeTimeoutError has been deinitialized,
 * all functions in this header shouldn't be used anymore.
 */
SEE_EXPORT
void see_timeout_error_deinit();

#ifdef __cplusplus
}
#endif

#endif //ifndef SEE_TIMEOUT_ERROR_H
