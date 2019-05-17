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
 * \file Error.h
 * \brief Sometimes things don't work out as desired.
 * \author Maarten Duijndam
 *
 * This class is designed to be used when something goes horribly (hopefully not)
 * wrong. You can obtain a SeeError. The SeeError instance contains a useful
 * message, indicating what went wrong and perhaps it enables you at runtime
 * to choose a secondary strategy to achieve the same goal.
 *
 * for example:
 * @code
 * int ret;
 * SeeError* error = NULL;
 * ret = see_dynamic_array_reserve(array, 100000000000, &error);
 * if (ret != SEE_SUCCESS) {
 *    fprintf(stderr, "Oops couldn't reserve enough memory: %s\n"
 *            see_error_msg(error)
 *            );
 * }
 * @endcode
 */

#ifndef SEE_ERROR_H
#define SEE_ERROR_H

#include "SeeObject.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _SeeError SeeError;
typedef struct _SeeErrorClass SeeErrorClass;


/**
 * \brief The errors thrown when something goes to Oblivion.
 *
 * \private
 */
struct _SeeError {
    SeeObject parent_obj;

    /*expand SeeError data here*/

    /**
     * \brief holds the error message.
     * \private
     */
    char* msg;
};

/**
 * \brief The class definition of an error.
 *
 * \private
 */
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

/**
 * \brief Cast a pointer to a SeeError derived object to a pointer of SeeError.
 *
 * Note make sure it is SeeError derived
 */
#define SEE_ERROR(obj)\
    ((SeeError*)(obj))

/**
 * \brief Cast pointer to pointer to a reference of a SeeError*.
 */
#define SEE_ERROR_REF(ref)\
    ((SeeError**) ref)

/**
 * \brief Cast a pointer to a SeeErrorClass derived class back to a const SeeErrorClass
 * instance.
 */
#define SEE_ERROR_CLASS(cls)\
    ((const SeeErrorClass*) (cls))

/**
 * \brief Get a const pointer a (derived) SeeErrorClass. This cast can be used
 * to call polymorphic functions.
 */
#define SEE_ERROR_GET_CLASS(obj)\
    ( SEE_ERROR_CLASS( see_object_get_class( SEE_OBJECT(obj) ) )  )


/* **** public functions **** */

/**
 * \brief Gets the pointer to the SeeErrorClass table.
 */
SEE_EXPORT const SeeErrorClass*
see_error_class();


/**
 * \brief Create a new error without an initialized message.
 *
 * @param [out] out A pointer to a SeeError* object *out should be NULL.
 *
 * @return SEE_SUCCESS when successful and another see_error otherwise.
 */
SEE_EXPORT int
see_error_new(SeeError** out);


/**
 * \brief Create a new error that contains an initialized error message.
 *
 * @param [out] out       If this function is successful, *out will point
 *                        to the newly created SeeError instance.
 * @param [in]  message   A message describing the error that has occurred.
 *
 * @return A newly created message with a descriptive error message.
 */
SEE_EXPORT int
see_error_new_msg(SeeError** out, const char* message);


/**
 * \brief obtain the message that belongs to the Error.
 *
 * @param [in] error The error from which we would like to obtain the message.
 *
 * @return A pointer to the message.
 */
SEE_EXPORT const char*
see_error_msg(const SeeError* error);

/**
 * \brief Set the error message.
 *
 * @param [in,out] error The error instance on which we would like to set the message
 * @param [in]     msg   The message.
 */
SEE_EXPORT void
see_error_set_msg(SeeError* error, const char* msg);

/* **** class initialization functions **** */

/**
 * \brief Initialize SeeError; make it ready for use.
 */
SEE_EXPORT
int see_error_init();

/**
 * \brief Deinitialize SeeError, after SeeError has been deinitialized,
 * all functions in this header shouldn't be used anymore.
 */
SEE_EXPORT
void see_error_deinit();

#ifdef __cplusplus
}
#endif

#endif //ifndef SEE_ERROR_H
