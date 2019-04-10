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

struct _SeeDuration {
    SeeObject   parent_obj;
    void*       priv_dur;
};

struct _SeeDurationClass {
    SeeObjectClass parent_cls;
    
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

SEE_EXPORT int
see_duration_new(SeeDuration** out, SeeError** error_out);

SEE_EXPORT int
see_duration_new_s(SeeDuration** out, int64_t s, SeeError** error_out);

SEE_EXPORT int
see_duration_new_ms(SeeDuration** out, int64_t ms, SeeError** error_out);

SEE_EXPORT int
see_duration_new_us(SeeDuration** out, int64_t us, SeeError** error_out);

SEE_EXPORT int
see_duration_new_ns(SeeDuration** out, int64_t ns, SeeError** error_out);

SEE_EXPORT int
see_duration_set(
    SeeDuration* self,
    SeeDuration* other,
    SeeError** error_out
    );

SEE_EXPORT int
see_duration_add(
    const SeeDuration*  self,
    const SeeDuration*  other,
    SeeDuration**       result,
    SeeError**          error_out
    );

SEE_EXPORT int
see_duration_sub(
    const SeeDuration*  self,
    const SeeDuration*  other,
    SeeDuration**       result,
    SeeError**          error_out
    );

/**
* @brief evaluates self < rhs
*
* @param[out] result non zero value when self smaller then rhs.
* @return SEE_SUCCESS or SEE_INVALID_ARGUMENT
*/
SEE_EXPORT int
see_duration_lt(
    const SeeDuration* self,
    const SeeDuration* rhs,
    int* result
    );

/**
* @brief evaluates self <= rhs
*
* @param [out] result non zero value when self <= rhs.
* @return SEE_SUCCESS or SEE_INVALID_ARGUMENT
*/
SEE_EXPORT int
see_duration_lte(
    const SeeDuration* self,
    const SeeDuration* rhs,
    int* result
    );

/**
* @brief evaluates self == rhs
*
* @return non zero value when self == rhs.
* @result SEE_SUCCESS or SEE_INVALID_ARGUMENT
*/
SEE_EXPORT int
see_duration_eq(
    const SeeDuration* self,
    const SeeDuration* rhs,
    int* result
    );

/**
 * @brief evaluates self >= rhs
 *
 * @return non zero value when self >= rhs.
 * @return SEE_SUCCESS or SEE_INVALID_ARGUMENT
 */
SEE_EXPORT int
see_duration_gte(
    const SeeDuration* self,
    const SeeDuration* rhs,
    int* result
    );

/**
 * @brief evaluates self > rhs
 *
 * @param result non zero value when self > rhs.
 * @return SEE_SUCCESS or SEE_INVALID_ARGUMENT
 */
SEE_EXPORT int
see_duration_gt(
    const SeeDuration* self,
    const SeeDuration* rhs,
    int* result
    );


SEE_EXPORT double
see_duration_seconds_f(const SeeDuration* self);

SEE_EXPORT int64_t
see_duration_seconds(const SeeDuration* self);

SEE_EXPORT int64_t
see_duration_millis(const SeeDuration* self);

SEE_EXPORT int64_t
see_duration_micros(const SeeDuration* self);

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
