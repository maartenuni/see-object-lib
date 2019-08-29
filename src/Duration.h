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
 * \file Duration.h Export the api of the SeeDuration objects.
 *
 * \brief Durations are a aritmetic type that specify the amount of time
 * between two points in time.
 *
 * Duration is a data type that stores the amount of time between
 * two specific timepoints. Durations are objects that can be
 * added and subtracted from each other. Although one can construct
 * durations from seconds, ms, us and ns, the calculations are always
 * done with the precision of the C++11 std::chrono::steady_clock.
 * This is likely in nanoseconds.
 * The SeeDurationTimeClass implements SeeObjectClass->compare. This means
 * that durations can be checked on order from short to long and equality.
 */

#ifndef SEE_DURATION_H
#define SEE_DURATION_H

#include "SeeObject.h"
#include "Error.h"
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _SeeDuration SeeDuration;
typedef struct _SeeDurationClass SeeDurationClass;

/**
 * @brief A instance of a SeeDuration
 *
 * \private
 */
struct _SeeDuration {

    /**
     * \briefSeeDuration is also a SeeObject.
     * \private
     */
    SeeObject   parent_obj;

    /**
     * @brief the implementation of the duration
     * \private
     */
    void*       priv_dur;
};

/**
 * \brief The SeeDurationClass that specifies the operations on a duration.
 */
struct _SeeDurationClass {

    /**
     * \brief the SeeDuration class is an instance of SeeObjectClass.
     * \private
     */
    SeeObjectClass parent_cls;

    /**
     * \brief Initizalizes a new duration instance.
     *
     * \private
     *
     * @param [in] duration     A newly allocated duration
     * @param [in] duration_cls The class the duration belongs to
     * @param [in] nano_seconds The number of nanoseconds the duration should
     *                          be set to.
     * @param [out] error_out   If an error occurs it will be returned here.
     *
     * @return SEE_SUCCESS
     */
    int (*duration_init)(
        SeeDuration*            duration,
        const SeeDurationClass* duration_cls,
        int64_t                 nano_seconds,
        SeeError**              error_out
        );
        
    /* expand SeeDuration class with extra functions here.*/
};

/* **** function style macro casts **** */

/**
 * \brief cast a pointer from a SeeDuration derived instance back to a
 *        pointer to SeeDuration.
 */
#define SEE_DURATION(obj)                      \
    ((SeeDuration*) obj)

/**
 * \brief cast a pointer to pointer from a SeeDuration derived instance back to a
 *        reference to SeeDuration*.
 */
#define SEE_DURATION_REF(ref)                      \
    ((SeeDuration**) ref)

/**
 * \brief cast a pointer to SeeDurationClass derived class back to a
 *        pointer to SeeDurationClass.
 */
#define SEE_DURATION_CLASS(cls)                      \
    ((const SeeDurationClass*) cls)

/**
 * \brief obtain a pointer to SeeDurationClass from a instance of
 *        derived from SeeDuration. This macro is preferably
 *        used when obtaining the class of a instance. When this
 *        macro is used. Calling methods on the class will enable
 *        polymorphism, because you'll get the derived class.
 */
#define SEE_DURATION_GET_CLASS(obj)                \
    (SEE_DURATION_CLASS(see_object_get_class(SEE_OBJECT(obj)) )  )

/* **** public functions **** */

/**
 * @brief Create a new duration with a default value of 0
 *
 * @param [out] out The newly generated duration
 * @param [out] error_out If an error occurs it will be returned here
 * @return SEE_SUCCESS, SEE_INVALID_ARGUMENT
 */
SEE_EXPORT int
see_duration_new(SeeDuration** out, SeeError** error_out);

/**
 * @brief Create a new duration with a default value of in seconds
 *
 * @param [out] out The newly generated duration
 * @param [in]  s   The number of seconds the duration should be equal to
 * @param [out] error_out If an error occurs it will be returned here
 * @return SEE_SUCCESS, SEE_INVALID_ARGUMENT
 */
SEE_EXPORT int
see_duration_new_s(SeeDuration** out, int64_t s, SeeError** error_out);

/**
 * @brief Create a new duration with a default value of 0
 *
 * @param [out] out The newly generated duration
 * @param [in]  ms  The number of milliseconds the duration should be equal to
 * @param [out] error_out If an error occurs it will be returned here
 *
 * @return SEE_SUCCESS, SEE_INVALID_ARGUMENT
 */
SEE_EXPORT int
see_duration_new_ms(SeeDuration** out, int64_t ms, SeeError** error_out);

/**
 * @brief Create a new duration with a default value of 0
 *
 * @param [out] out The newly generated duration
 * @param [in]  us  The number of microseconds the duration should be equal to
 * @param [out] error_out If an error occurs it will be returned here
 * @return SEE_SUCCESS, SEE_INVALID_ARGUMENT
 */
SEE_EXPORT int
see_duration_new_us(SeeDuration** out, int64_t us, SeeError** error_out);

/**
 * @brief Create a new duration with a default value of 0
 *
 * @param [out] out The newly generated duration
 * @param [in]  ns  The number of nano seconds the duration should be equal to
 * @param [out] error_out If an error occurs it will be returned here
 * @return SEE_SUCCESS, SEE_INVALID_ARGUMENT
 */
SEE_EXPORT int
see_duration_new_ns(SeeDuration** out, int64_t ns, SeeError** error_out);

/**
 * @brief Set the current duration equal to another duration
 *
 * @param [in]  self
 * @param [in]  other
 * @param [out] error_out
 *
 * @return SEE_SUCCES, SEE_INVALID_ARGUMENT, SEE_ERROR_RUNTIME
 */
SEE_EXPORT int
see_duration_set(
    SeeDuration* self,
    SeeDuration* other,
    SeeError** error_out
    );

/**
 * @brief Add two durations
 *
 * Adding self + other (communicative)
 *
 * @param [in] self may not be null
 * @param [in] other may not be null
 * @param [in, out] result may not be NULL, if *result is NULL, a new result
 *                  will be returned, otherwise the old one will be used to
 *                  store the result of self + other.
 * @param error_out If an error occurs it will be returned here.
 * @return SEE_SUCCESS, SEE_INVALID_ARUGMENT, SEE_ERROR_RUNTIME
 */
SEE_EXPORT int
see_duration_add(
    const SeeDuration*  self,
    const SeeDuration*  other,
    SeeDuration**       result,
    SeeError**          error_out
    );

/**
 * @brief Subtract two durations
 *
 * subtract other from self. self - other (not communicative)
 *
 * @param [in] self may not be null
 * @param [in] other may not be null
 * @param [in, out] result may not be NULL, if *result is NULL, a new result
 *                  will be returned, otherwise the old one will be used to
 *                  store the result of self - other.
 * @param error_out If an error occurs it will be returned here.
 * @return SEE_SUCCESS, SEE_INVALID_ARUGMENT, SEE_ERROR_RUNTIME
 */

SEE_EXPORT int
see_duration_sub(
    const SeeDuration*  self,
    const SeeDuration*  other,
    SeeDuration**       result,
    SeeError**          error_out
    );

/**
 * \brief Multiply a duration by a scalar.
 *
 * \note Multiplication might lead to overflows.
 *
 * @param [in] self May not be NULL, the duration to multiply.
 * @param [in] scalar The scalar to multiply the duration with.
 * @param [out] result The result will be returned here.
 * @param [out] error_out If an error occurs it might be returned here.
 *
 * @return SEE_SUCCESS, SEE_INVALID_ARGUMENT
 */
SEE_EXPORT int
see_duration_multiply(
    const SeeDuration* self,
    int64_t            scalar,
    SeeDuration**      result,
    SeeError**         error_out
    );

/**
 * \brief Multiply a duration by a scalar.
 *
 * \note Multiplication might lead to overflows.
 *
 * @param [in] self May not be NULL, the duration to multiply.
 * @param [in] scalar The scalar to multiply the duration with.
 * @param [out] result The result will be returned here.
 * @param [out] error_out If an error occurs it might be returned here.
 *
 * @return SEE_SUCCESS, SEE_INVALID_ARGUMENT
 */
SEE_EXPORT int
see_duration_multiply_f(
    const SeeDuration* self,
    double             scalar,
    SeeDuration**      result,
    SeeError**         error_out
    );

/**
 * \brief Divide a duration by a scalar.
 *
 * \note Multiplication might lead to overflows.
 *
 * @param [in] self May not be NULL, the duration to divide.
 * @param [in] scalar The scalar to divide the duration with.
 * @param [out] result The result will be returned here.
 * @param [out] error_out If an error occurs it might be returned here.
 *
 * @return SEE_SUCCESS, SEE_INVALID_ARGUMENT
 */
SEE_EXPORT int
see_duration_divide(
    const SeeDuration* self,
    int64_t            scalar,
    SeeDuration**      result,
    SeeError**         error_out
);

/**
 * \brief Divide a duration by a scalar.
 *
 * \note Multiplication might lead to overflows.
 *
 * @param [in] self May not be NULL, the duration to divide.
 * @param [in] scalar The scalar to divide the duration with.
 * @param [out] result The result will be returned here.
 * @param [out] error_out If an error occurs it might be returned here.
 *
 * @return SEE_SUCCESS, SEE_INVALID_ARGUMENT
 */

SEE_EXPORT int
see_duration_divide_f(
    const SeeDuration* self,
    double             scalar,
    SeeDuration**      result,
    SeeError**         error_out
);

/**
 * @brief Obtain the duration in seconds.
 *
 * @param self
 * @return a floating point number of the duration in seconds.
 */
SEE_EXPORT double
see_duration_seconds_f(const SeeDuration* self);

/**
 * @brief obtain the number of seconds a duration is. The result is floored.
 * if the duration is 1.0001 or 1.99999, 1 will be returned.
 * @param self
 * @return the floored number of seconds of the duration
 */
SEE_EXPORT int64_t
see_duration_seconds(const SeeDuration* self);

/**
 * @brief obtain the number of milli seconds a duration is. The result is floored.
 * if the duration is 1.0001 or 1.99999, 1 will be returned.
 * @param self
 * @return the floored number of milli seconds of the duration
 */
SEE_EXPORT int64_t
see_duration_millis(const SeeDuration* self);

/**
 * @brief obtain the number of micro seconds a duration is. The result is floored.
 * if the duration is 1.0001 or 1.99999, 1 will be returned.
 * @param self
 * @return the floored number of micro seconds of the duration
 */
SEE_EXPORT int64_t
see_duration_micros(const SeeDuration* self);

/**
 * @brief obtain the number of nano seconds a duration is.
 *
 * This is the precision that SeeDuration object use to do calculations.
 *
 * @param self
 * @return The number of nanoseconds the duration is.
 */
SEE_EXPORT int64_t
see_duration_nanos(const SeeDuration* self);


/**
 * Gets the pointer to the SeeDurationClass table.
 */
SEE_EXPORT const SeeDurationClass*
see_duration_class();

/* Expand the class with public functions here, don't forget the SEE_EXPORT
 * macro, because otherwise you'll run into troubles when exporting function
 * in a windows dll.
 */

/* **** class initialization functions **** */

/**
 * Initialize SeeDuration; make it ready for use.
 */
SEE_EXPORT
int see_duration_init();

/**
 * Deinitialize SeeDuration, after SeeDuration has been deinitialized,
 * all functions in this header shouldn't be used anymore.
 */
SEE_EXPORT
void see_duration_deinit();

#ifdef __cplusplus
}
#endif

#endif //ifndef SEE_DURATION_H
