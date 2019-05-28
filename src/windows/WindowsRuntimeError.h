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


#ifndef SEE_WINDOWS_RUNTIME_ERROR_H
#define SEE_WINDOWS_RUNTIME_ERROR_H

#include <see_object_config.h>
#ifdef HAVE_WINDOWS_H
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

#include "../Error.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _SeeWindowsRuntimeError SeeWindowsRuntimeError;
typedef struct _SeeWindowsRuntimeErrorClass SeeWindowsRuntimeErrorClass;

struct _SeeWindowsRuntimeError {
    SeeError parent_obj;
    /*expand SeeWindowsRuntimeError data here*/
        
};

struct _SeeWindowsRuntimeErrorClass {
    SeeErrorClass parent_cls;
    
    int (*windows_runtime_error_init)(
        SeeWindowsRuntimeError*            windows_runtime_error,
        const SeeWindowsRuntimeErrorClass* windows_runtime_error_cls,
        DWORD last_error
        );
        
    /* expand SeeWindowsRuntimeError class with extra functions here.*/
};

/* **** function style macro casts **** */

/**
 * \brief cast a pointer from a SeeWindowsRuntimeError derived instance back to a
 *        pointer to SeeWindowsRuntimeError.
 */
#define SEE_WINDOWS_RUNTIME_ERROR(obj)                      \
    ((SeeWindowsRuntimeError*) obj)

/**
 * \brief cast a pointer to pointer from a SeeWindowsRuntimeError derived instance back to a
 *        reference to SeeWindowsRuntimeError*.
 */
#define SEE_WINDOWS_RUNTIME_ERROR_REF(ref)                      \
    ((SeeWindowsRuntimeError**) ref)

/**
 * \brief cast a pointer to SeeWindowsRuntimeErrorClass derived class back to a
 *        pointer to SeeWindowsRuntimeErrorClass.
 */
#define SEE_WINDOWS_RUNTIME_ERROR_CLASS(cls)                      \
    ((const SeeWindowsRuntimeErrorClass*) cls)

/**
 * \brief obtain a pointer to SeeWindowsRuntimeErrorClass from a instance of
 *        derived from SeeWindowsRuntimeError. This macro is preferably
 *        used when obtaining the class of a instance. When this
 *        macro is used. Calling methods on the class will enable
 *        polymorphism, because you'll get the derived class.
 */
#define SEE_WINDOWS_RUNTIME_ERROR_GET_CLASS(obj)                \
    (SEE_WINDOWS_RUNTIME_ERROR_CLASS(see_object_get_class(SEE_OBJECT(obj)) )  )

/* **** public functions **** */

/**
 * Gets the pointer to the SeeWindowsRuntimeErrorClass table.
 */
SEE_EXPORT const SeeWindowsRuntimeErrorClass*
see_windows_runtime_error_class();

/**
 * @brief Create a new windows runtime error.
 *
 * This error is typically created when see_object_someting call
 * a function from the windows error, something goes wrong and
 * one then has to call GetLastError(). The value returned from get last
 * error is then used to create a message for the user.
 *
 * @param [out] error_out The newly generated error will be returned here.
 * @param [in]  error_no A number retrieved from `GetLastError()`.
 *
 * @return SEE_SUCCESS or the program is pretty much out of mem and then
 *         all hope is lost...
 */
SEE_EXPORT int
see_windows_runtime_error_new(
    SeeWindowsRuntimeError** error_out,
    DWORD error_no
    );

/* **** class initialization functions **** */

/**
 * Initialize SeeWindowsRuntimeError; make it ready for use.
 */
SEE_EXPORT
int see_windows_runtime_error_init();

/**
 * Deinitialize SeeWindowsRuntimeError, after SeeWindowsRuntimeError has been deinitialized,
 * all functions in this header shouldn't be used anymore.
 */
SEE_EXPORT
void see_windows_runtime_error_deinit();

#ifdef __cplusplus
}
#endif

#endif //ifndef SEE_WINDOWS_RUNTIME_ERROR_H
