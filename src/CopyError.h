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
 * \brief File that exports CopyError
 *
 * Typically CopyErrors are raised when the class of an instance doesn't
 * implement the SeeObjectClass->copy method.
 */

#ifndef SEE_COPY_ERROR_H
#define SEE_COPY_ERROR_H

#include "Error.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _SeeCopyError SeeCopyError;
typedef struct _SeeCopyErrorClass SeeCopyErrorClass;

/**
 * \brief SeeCopyError is an error raised when someone tries to copy an non copyable
 * object.
 */
struct _SeeCopyError {
    SeeError parent_obj;
    /*expand SeeCopyError data here*/
        
};

struct _SeeCopyErrorClass {
    SeeErrorClass parent_cls;
    
    int (*copy_error_init)(
        SeeCopyError*            copy_error,
        const SeeCopyErrorClass* copy_error_cls,
        const SeeObjectClass*    instance_cls
        );
        
    /* expand SeeCopyError class with extra functions here.*/
};

/* **** function style macro casts **** */

/**
 * \brief cast a pointer from a SeeCopyError derived instance back to a
 *        pointer to SeeCopyError.
 */
#define SEE_COPY_ERROR(obj)                      \
    ((SeeCopyError*) obj)

/**
 * \brief cast a pointer to pointer from a SeeCopyError derived instance back to a
 *        reference to SeeCopyError*.
 */
#define SEE_COPY_ERROR_REF(ref)                      \
    ((SeeCopyError**) ref)

/**
 * \brief cast a pointer to SeeCopyErrorClass derived class back to a
 *        pointer to SeeCopyErrorClass.
 */
#define SEE_COPY_ERROR_CLASS(cls)                      \
    ((const SeeCopyErrorClass*) cls)

/**
 * \brief obtain a pointer to SeeCopyErrorClass from a instance of
 *        derived from SeeCopyError. This macro is preferably
 *        used when obtaining the class of a instance. When this
 *        macro is used. Calling methods on the class will enable
 *        polymorphism, because you'll get the derived class.
 */
#define SEE_COPY_ERROR_GET_CLASS(obj)                \
    (SEE_COPY_ERROR_CLASS(see_object_get_class(SEE_OBJECT(obj)) )  )

/* **** public functions **** */

/**
 * \brief Create a new CopyError instance.
 * @param [out] error         The new instance will be returned here.
 * @param [in]  instance_cls  The class of the instance that isn't copyable.
 * @return SEE_SUCCESS, SEE_INVALID_ARGUMENT
 */
int
see_copy_error_new(SeeError** error, const SeeObjectClass* instance_cls);

/**
 * Gets the pointer to the SeeCopyErrorClass table.
 */
SEE_EXPORT const SeeCopyErrorClass*
see_copy_error_class();

/* Expand the class with public functions here, don't forget the SEE_EXPORT
 * macro, because otherwise you'll run into troubles when exporting function
 * in a windows dll.
 */

/* **** class initialization functions **** */

/**
 * Initialize SeeCopyError; make it ready for use.
 */
SEE_EXPORT
int see_copy_error_init();

/**
 * Deinitialize SeeCopyError, after SeeCopyError has been deinitialized,
 * all functions in this header shouldn't be used anymore.
 */
SEE_EXPORT
void see_copy_error_deinit();

#ifdef __cplusplus
}
#endif

#endif //ifndef SEE_COPY_ERROR_H
