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
 * \file IncomparableError.h
 * \brief Defines an error raised when either an object is not comparable
 *        or a combination of objects is not comparable.
 */

#ifndef SEE_INCOMPARABLE_ERROR_H
#define SEE_INCOMPARABLE_ERROR_H

#include "Error.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _SeeIncomparableError SeeIncomparableError;
typedef struct _SeeIncomparableErrorClass SeeIncomparableErrorClass;

struct _SeeIncomparableError {
    SeeError parent_obj;
    /*expand SeeIncomparableError data here*/
        
};

struct _SeeIncomparableErrorClass {
    SeeErrorClass parent_cls;
    
    int (*incomparable_error_init)(
        SeeIncomparableError*            incomparable_error,
        const SeeIncomparableErrorClass* incomparable_error_cls,
        const SeeObjectClass*            cls_self,
        const SeeObjectClass*            cls_other
        );
        
    /* expand SeeIncomparableError class with extra functions here.*/
};

/* **** function style macro casts **** */

/**
 * \brief cast a pointer from a SeeIncomparableError derived instance back to a
 *        pointer to SeeIncomparableError.
 */
#define SEE_INCOMPARABLE_ERROR(obj)                      \
    ((SeeIncomparableError*) obj)

/**
 * \brief cast a pointer to pointer from a SeeIncomparableError derived instance back to a
 *        reference to SeeIncomparableError*.
 */
#define SEE_INCOMPARABLE_ERROR_REF(ref)                      \
    ((SeeIncomparableError**) ref)

/**
 * \brief cast a pointer to SeeIncomparableErrorClass derived class back to a
 *        pointer to SeeIncomparableErrorClass.
 */
#define SEE_INCOMPARABLE_ERROR_CLASS(cls)                      \
    ((const SeeIncomparableErrorClass*) cls)

/**
 * \brief obtain a pointer to SeeIncomparableErrorClass from a instance of
 *        derived from SeeIncomparableError. This macro is preferably
 *        used when obtaining the class of a instance. When this
 *        macro is used. Calling methods on the class will enable
 *        polymorphism, because you'll get the derived class.
 */
#define SEE_INCOMPARABLE_ERROR_GET_CLASS(obj)                \
    (SEE_INCOMPARABLE_ERROR_CLASS(see_object_get_class(SEE_OBJECT(obj)) )  )

/* **** public functions **** */

/**
 * \brief Create a incomparible error instance.
 *
 * These objects can be raised when something goes wrong with comparing
 * two objects. In order for an object, the object must be comparable. The
 * object can do this via implementing the SeeObjectClass->compare() method.
 *
 * @param [out] error       The new error is returned here.
 * @param [in]  self_class  The class
 * @param [in]  other_class
 * @return
 */
SEE_EXPORT int
see_incomparable_error_new(
    SeeError **error,
    const SeeObjectClass *self_class,
    const SeeObjectClass *other_class
);

/**
 * Gets the pointer to the SeeIncomparableErrorClass table.
 */
SEE_EXPORT const SeeIncomparableErrorClass*
see_incomparable_error_class();

/* Expand the class with public functions here, don't forget the SEE_EXPORT
 * macro, because otherwise you'll run into troubles when exporting function
 * in a windows dll.
 */

/* **** class initialization functions **** */

/**
 * Initialize SeeIncomparableError; make it ready for use.
 */
SEE_EXPORT
int see_incomparable_error_init();

/**
 * Deinitialize SeeIncomparableError, after SeeIncomparableError has been deinitialized,
 * all functions in this header shouldn't be used anymore.
 */
SEE_EXPORT
void see_incomparable_error_deinit();

#ifdef __cplusplus
}
#endif

#endif //ifndef SEE_INCOMPARABLE_ERROR_H
