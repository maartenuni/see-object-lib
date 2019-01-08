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


#ifndef SEE_ERROR_H
#define SEE_ERROR_H

#include "SeeObject.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _SeeError SeeError;
typedef struct _SeeErrorClass SeeErrorClass;

struct _SeeError {
    SeeObject parent_obj;

    /*expand SeeError data here*/

    /**
     * \brief holds the error message.
     */
    const char* msg;
};

struct _SeeErrorClass {
    SeeObjectClass parent_cls;
    /* expand SeeError class with extra functions here.*/

    /**
     * \brief initializes a new error instance
     * @param error a pointer to the error instance
     * @param cls   a pointer to the SeeErrorClass*
     * @param msg   a message, this maybe NULL(not so useful though).
     */
    void (*error_init)(
        SeeError*               error,
        const SeeErrorClass*    cls,
        const char*             msg
        );

    /**
     * Obtain the error message.
     * @return A const char* that describes the error that has occurred.
     */
    const char* (*msg)(const SeeError* error);

    /**
     * \brief Set the error message of error.
     */
    void (*set_msg)(SeeError* error, const char* msg);
};

/* **** public functions **** */

/**
 * Gets the pointer to the SeeErrorClass table.
 */
SEE_EXPORT const SeeErrorClass*
see_error_class();


/**
 * \brief Create a new error without an initialized message.
 *
 * @param out[out] A pointer to a SeeError* object *out should be NULL.
 *
 * @return SEE_SUCCESS when successful and another see_error otherwise.
 */
SEE_EXPORT int
see_error_new(SeeError** out);


/**
 * \brief Create a new error that contains an initialized error message.
 *
 * @param out[out]      If this function is successful, *out will point
 *                      to the newly created SeeError instance.
 * @param message[in]   A message describing the error that has occurred.
 *
 * @return A newly created message with a descriptive error message.
 */
SEE_EXPORT int
see_error_new_msg(SeeError** out, const char* message);


/**
 * \brief obtain the message that belongs to the Error.
 *
 * @param error[in] The error from which we would like to obtain the message.
 *
 * @return A pointer to the message.
 */
SEE_EXPORT const char*
see_error_msg(const SeeError* error);

/**
 * \brief Set the error message.
 *
 * @param error[in,out] The error instance on which we would like to set the message
 * @param msg[in]       The message.
 */
SEE_EXPORT void
see_error_set_msg(SeeError* error, const char* msg);

/* **** class initialization functions **** */

/**
 * Initialize SeeError; make it ready for use.
 */
SEE_EXPORT
int see_error_init();

/**
 * Deinitialize SeeError, after SeeError has been deinitialized,
 * all functions in this header shouldn't be used anymore.
 */
SEE_EXPORT
void see_error_deinit();

#ifdef __cplusplus
}
#endif

#endif //ifndef SEE_ERROR_H
