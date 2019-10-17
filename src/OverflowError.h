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


#ifndef SEE_OVERFLOW_ERROR_H
#define SEE_OVERFLOW_ERROR_H

#include "Error.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _SeeOverflowError SeeOverflowError;
typedef struct _SeeOverflowErrorClass SeeOverflowErrorClass;

struct _SeeOverflowError {
    SeeError parent_obj;
    /*expand SeeOverflowError data here*/
        
};

struct _SeeOverflowErrorClass {
    SeeErrorClass parent_cls;
    
    /**
     * \brief Initialize a newly created SeeOverflowError instance
     * \private
     * @param [in] msg The message may be NULL, than a general
     *                 error message will be routed to SeeError->error_init
     *                 that describes the error condition, however it might
     *                 be preferable to describe the valid range for instance.
     */
    int (*overflow_error_init)(
        SeeOverflowError*            overflow_error,
        const SeeOverflowErrorClass* overflow_error_cls,
        const char*                  msg
        );
        
    /* expand SeeOverflowError class with extra functions here.*/
};

/* **** function style macro casts **** */

/**
 * \brief cast a pointer from a SeeOverflowError derived instance back to a
 *        pointer to SeeOverflowError.
 */
#define SEE_OVERFLOW_ERROR(obj)                      \
    ((SeeOverflowError*) obj)

/**
 * \brief cast a pointer to pointer from a SeeOverflowError derived instance back to a
 *        reference to SeeOverflowError*.
 */
#define SEE_OVERFLOW_ERROR_REF(ref)                      \
    ((SeeOverflowError**) ref)

/**
 * \brief cast a pointer to SeeOverflowErrorClass derived class back to a
 *        pointer to SeeOverflowErrorClass.
 */
#define SEE_OVERFLOW_ERROR_CLASS(cls)                      \
    ((const SeeOverflowErrorClass*) cls)

/**
 * \brief obtain a pointer to SeeOverflowErrorClass from a instance of
 *        derived from SeeOverflowError. This macro is preferably
 *        used when obtaining the class of a instance. When this
 *        macro is used. Calling methods on the class will enable
 *        polymorphism, because you'll get the derived class.
 */
#define SEE_OVERFLOW_ERROR_GET_CLASS(obj)                \
    (SEE_OVERFLOW_ERROR_CLASS(see_object_get_class(SEE_OBJECT(obj)) )  )

/* **** public functions **** */

/**
 * Create a new overflow error
 *
 * @param [out] error The newly create error will be returned here.
 * @param [in]  msg   Maybe null, then a general message will
 *                    be used to describe the error, prefer a more
 *                    descriptive eg a valid range for the parameter
 *                    that overflowed instead.
 *
 * @return 0 if the error was succesfully created.
 */
SEE_EXPORT int
see_overflow_error_new(SeeError** error, const char* msg);

/**
 * Gets the pointer to the SeeOverflowErrorClass table.
 */
SEE_EXPORT const SeeOverflowErrorClass*
see_overflow_error_class();

/* Expand the class with public functions here, don't forget the SEE_EXPORT
 * macro, because otherwise you'll run into troubles when exporting function
 * in a windows dll.
 */

/* **** class initialization functions **** */

/**
 * Initialize SeeOverflowError; make it ready for use.
 */
SEE_EXPORT
int see_overflow_error_init();

/**
 * Deinitialize SeeOverflowError, after SeeOverflowError has been deinitialized,
 * all functions in this header shouldn't be used anymore.
 */
SEE_EXPORT
void see_overflow_error_deinit();

#ifdef __cplusplus
}
#endif

#endif //ifndef SEE_OVERFLOW_ERROR_H
