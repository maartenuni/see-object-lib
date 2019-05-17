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
 * @file RuntimeError.h
 * @brief Create errors that help to determine what type of error has occurred.
 *
 * Some of the function in SeeObject can fail. When they fail, errno from
 * <errno.h> might be set. SeeRuntimeError is designed to throw when these
 * errors occur. Typically the function returns SEE_ERROR_RUNTIME. Then
 * one can obtain a useful message about why the error occurred.
 *
 * @code
 *
 * SeeError*  error  = NULL;
 * SeeSerial* serial = NULL;
 * int ret;
 *
 * const char* devfn = "/dev/ty1"; // tty1 might have been more capable.
 *
 * ret = see_serial_new(&serial, &error);
 * ret = see_serial_open(serial, devfn, &error);
 * if (ret == SEE_ERROR_RUNTIME) {
 *     fprintf("Oops, unable to open %s, because :%s",
 *         devfn,
 *         see_error_msg(error)
 *         );
 *     see_object_decref(SEE_OBJECT(error));
 *     exit(EXIT_FAILURE);
 * }
 *
 * @endcode
 *
 */

#ifndef SEE_RUNTIME_ERROR_H
#define SEE_RUNTIME_ERROR_H

#include "Error.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _SeeRuntimeError SeeRuntimeError;
typedef struct _SeeRuntimeErrorClass SeeRuntimeErrorClass;

/**
 * \brief Describes an instance of SeeRuntimeError
 * \private
 */
struct _SeeRuntimeError {

    /**
     * A runtime Error is also an instance of SeeError.
     *
     * \private
     */
    SeeError parent_obj;

};

/**
 * \brief Describe the class - the operations - of SeeRuntimeError
 * \private
 */
struct _SeeRuntimeErrorClass {
    /**
     * \brief SeeRuntimeError derives from SeeError.
     * \private
     */
    SeeErrorClass parent_cls;

    /**
     * \brief Initialize a newly allocated instance of SeeRuntimeError.
     *
     * \private
     *
     * @param [in,out] runtime_error        The SeeRuntimeError to initialize.
     * @param [in]     runtime_error_cls    The class of the error.
     * @param [in]     error_num            The number that describes the error
     *                                      typically errno from <errno.h>
     * @return
     */
    int (*runtime_error_init)(
        SeeRuntimeError*            runtime_error,
        const SeeRuntimeErrorClass* runtime_error_cls,
        int                         error_num
        );
        
};

/* **** function style macro casts **** */

/**
 * \brief cast a pointer from a SeeRuntimeError derived instance back to a
 *        pointer to SeeRuntimeError.
 */
#define SEE_RUNTIME_ERROR(obj)                      \
    ((SeeRuntimeError*) obj)

/**
 * \brief cast a pointer to pointer from a SeeRuntimeError derived instance back to a
 *        reference to SeeRuntimeError*.
 */
#define SEE_RUNTIME_ERROR_REF(ref)                      \
    ((SeeRuntimeError**) ref)

/**
 * \brief cast a pointer to SeeRuntimeErrorClass derived class back to a
 *        pointer to SeeRuntimeErrorClass.
 */
#define SEE_RUNTIME_ERROR_CLASS(cls)                      \
    ((const SeeRuntimeErrorClass*) cls)

/**
 * \brief obtain a pointer to SeeRuntimeErrorClass from a instance of
 *        derived from SeeRuntimeError. This macro is preferably
 *        used when obtaining the class of a instance. When this
 *        macro is used. Calling methods on the class will enable
 *        polymorphism, because you'll get the derived class.
 */
#define SEE_RUNTIME_ERROR_GET_CLASS(obj)                \
    (SEE_RUNTIME_ERROR_CLASS(see_object_get_class(SEE_OBJECT(obj)) )  )

/* **** public functions **** */

/**
 * Gets the pointer to the SeeRuntimeErrorClass table.
 */
SEE_EXPORT const SeeRuntimeErrorClass*
see_runtime_error_class();

/* Expand the class with public functions here, don't forget the SEE_EXPORT
 * macro, because otherwise you'll run into troubles when exporting function
 * in a windows dll.
 */

/* **** class initialization functions **** */

/**
 * \brief create an error from errno
 *
 * This function mainly exists to generate an error, that contains the
 * message that describes the runtime error that just has occurred. Eg,
 * when the library allocates memory, but the allocation fails, one
 * can obtain an error that describes it.
 *
 * @param[out] error        The created RuntimeError will be returned here.
 * @param[in]  error_num    The value of errno.
 *
 * @code
 *
 * #include <errno.h>
 *
 * SeeError* error = NULL;
 * int* array = malloc(1000000);
 * if (!array) {
 *     see_runtime_error_create(&error, errno);
 *     return SEE_ERROR_RUNTIME;
 * }
 *
 * @endcode
 *
 * @return SEE_SUCCESS, hopefully.
 */
SEE_EXPORT int
see_runtime_error_create(SeeError** error, int error_num);

/**
 * Initialize SeeRuntimeError; make it ready for use.
 */
SEE_EXPORT
int see_runtime_error_init();

/**
 * Deinitialize SeeRuntimeError, after SeeRuntimeError has been deinitialized,
 * all functions in this header shouldn't be used anymore.
 */
SEE_EXPORT
void see_runtime_error_deinit();

#ifdef __cplusplus
}
#endif

#endif //ifndef SEE_RUNTIME_ERROR_H
