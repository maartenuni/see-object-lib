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
 * @file TimePoint.h Implements the API of SeeTimePoint
 *
 * @brief TimePoints are points in time for which some arithmetic
 * operations are defined.
 *
 * Typically two time points can be subtracted from each other in order to
 * obtain the duration between the two points. Additionally a Duration can
 * be added or subtracted from a time point in order to get a new
 * time point that is precisely that duration away.
 * Two time points are comparable, so that one is able to see which occurred
 * first. The SeeTimePointClass implements the compare interface, This
 * means that it is valid to call
 * see_object_compare(SEE_OBJECT(tp1), SEE_OBJECT(tp2)); on them
 * if tp1 and tp2 are a valid SeeTimePoint. This also means that you can call
 * see_object_less(), ... see_object_equal(), ... see_object_greater() on
 * them.
 */

#ifndef SEE_TIME_POINT_H
#define SEE_TIME_POINT_H

#include "SeeObject.h"
#include "Error.h"
#include "Duration.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct SeeTimePoint SeeTimePoint;
typedef struct SeeTimePointClass SeeTimePointClass;
/**
 * \brief An instance of SeeTimePoint
 */
struct SeeTimePoint {
    SeeObject parent_obj;
    /*expand SeeTimePoint data here*/

    /**
     * @brief the implementation of the timepoint
     * \private
     */
    void*     priv_time;
        
};

/**
 * @brief Describes the SeeTimePoint class.
 * \private
 */
struct SeeTimePointClass {
    SeeObjectClass parent_cls;
    
    int (*time_point_init)(
        SeeTimePoint*            time_point,
        const SeeTimePointClass* time_point_cls,
        SeeError**               error_out
        );
        
    /* expand SeeTimePoint class with extra functions here.*/

    /**
     * @brief Turn the timepoint to a human readable format.
     *
     * TODO (remove) Not implemented yet, since it aint no wall time.
     * perhaps is is possible to obtain the difference (a duration) with the
     * wall clock time and then compute a meaning full date/time etc.
     */
    int (*to_string)(const SeeTimePoint*, char** out);

    /**
     * @brief Add duration to this time point and return a new SeeTimePoint.
     */
    int(*add_dur)(
        const SeeTimePoint* self,
        const SeeDuration*  duration,
        SeeTimePoint**      inout,
        SeeError**          error_out
        );

    /**
     * @brief Subtract a duration from a timepoint in order to obtain a
     * new timepoint
     */
    int(*sub_dur)(
        const SeeTimePoint* self,
        const SeeDuration*  duration,
        SeeTimePoint**      inout,
        SeeError**          error_out
        );

    /**
     * @brief Subtract another timepoint from this one, that the duration
     * difference between the two timepoints can be calculated.
     */
    int (*sub)(
        const SeeTimePoint* self,
        const SeeTimePoint* other,
        SeeDuration**       out,
        SeeError**          error_out
        );

};

/* **** function style macro casts **** */

/**
 * \brief cast a pointer from a SeeTimePoint derived instance back to a
 *        pointer to SeeTimePoint.
 */
#define SEE_TIME_POINT(obj)                      \
    ((SeeTimePoint*) obj)

/**
 * \brief cast a pointer to pointer from a SeeTimePoint derived instance back to a
 *        reference to SeeTimePoint*.
 */
#define SEE_TIME_POINT_REF(ref)                      \
    ((SeeTimePoint**) ref)

/**
 * \brief cast a pointer to SeeTimePointClass derived class back to a
 *        pointer to SeeTimePointClass.
 */
#define SEE_TIME_POINT_CLASS(cls)                      \
    ((const SeeTimePointClass*) cls)

/**
 * \brief obtain a pointer to SeeTimePointClass from a instance of
 *        derived from SeeTimePoint. This macro is preferably
 *        used when obtaining the class of a instance. When this
 *        macro is used. Calling methods on the class will enable
 *        polymorphism, because you'll get the derived class.
 */
#define SEE_TIME_POINT_GET_CLASS(obj)                \
    (SEE_TIME_POINT_CLASS(see_object_get_class(SEE_OBJECT(obj)) )  )

/* **** public functions **** */

/**
 * @brief Create a newly generated timepoint
 *
 * returns SEE_SUCCESS or another error value.
 */
SEE_EXPORT int
see_time_point_new(SeeTimePoint** out, SeeError** error_out);

/**
 * @brief Set the timepoint of self equal to another.
 * @param [in, out] self
 * @param [in]      other
 * @param [out]     error_out
 * @return SEE_SUCCESS, SEE_ERROR_RUNTIME, SEE_INVALID_ARGUMENT
 */
SEE_EXPORT int
see_time_point_set(
    SeeTimePoint* self,
    const SeeTimePoint* other,
    SeeError** error_out
    );

/**
 * @brief Subtract two timepoints
 *
 * @param [in] self  The operation is the result self - other
 * @param [in] other t2
 * @param [in, out]  inout The result of self - other
 *
 * @return SEE_SUCCESS, SEE_RUNTIME_ERROR
 */
SEE_EXPORT int
see_time_point_sub(
    const SeeTimePoint* self,
    const SeeTimePoint* other,
    SeeDuration**       inout,
    SeeError**          error_out
    );

/**
 * @brief Add a duration to a timepoint in order to obtain a new
 *        timepoint.
 *
 * @param [in] self  The operation is the result self - other
 * @param [in] other t2
 * @param [in, out]  inout The result of self - other
 *
 * @return SEE_SUCCESS, SEE_RUNTIME_ERROR
 */
SEE_EXPORT int
see_time_point_add_dur(
    const SeeTimePoint* self,
    const SeeDuration*  dur,
    SeeTimePoint**      result,
    SeeError**          out
    );

/**
 * @brief Subtract a duration from a timepoint in order to obtain a new
 *        timepoint.
 *
 * @param [in] self  The operation is the result self - other
 * @param [in] other t2
 * @param [in, out]  inout The result of self - other
 *
 * @return SEE_SUCCESS, SEE_RUNTIME_ERROR
 */
SEE_EXPORT int
see_time_point_sub_dur(
    const SeeTimePoint* self,
    const SeeDuration*  dur,
    SeeTimePoint**      result,
    SeeError**          out
    );

/**
 * @brief evaluates self < rhs
 *
 * @param[out] result non zero value when self smaller then rhs.
 * @return SEE_SUCCESS or SEE_INVALID_ARGUMENT
 */
SEE_EXPORT int
see_time_point_lt(
    const SeeTimePoint* self,
    const SeeTimePoint* rhs,
    int* result
    );

/**
 * @brief evaluates self <= rhs
 *
 * @param [out] result non zero value when self <= rhs.
 * @return SEE_SUCCESS or SEE_INVALID_ARGUMENT
 */
SEE_EXPORT int
see_time_point_lte(
    const SeeTimePoint* self,
    const SeeTimePoint* rhs,
    int* result
    );

/**
* @brief evaluates self == rhs
*
* @return non zero value when self == rhs.
* @result SEE_SUCCESS or SEE_INVALID_ARGUMENT
*/
SEE_EXPORT int
see_time_point_eq(
    const SeeTimePoint* self,
    const SeeTimePoint* rhs,
    int* result
    );

/**
 * @brief evaluates self >= rhs
 *
 * @return non zero value when self >= rhs.
 * @return SEE_SUCCESS or SEE_INVALID_ARGUMENT
 */
SEE_EXPORT int
see_time_point_gte(
    const SeeTimePoint* self,
    const SeeTimePoint* rhs,
    int* result
    );

/**
 * @brief evaluates self > rhs
 *
 * @param result non zero value when self > rhs.
 * @return SEE_SUCCESS or SEE_INVALID_ARGUMENT
 */
SEE_EXPORT int
see_time_point_gt(
    const SeeTimePoint* self,
    const SeeTimePoint* rhs,
    int* result
    );

/**
 * Gets the pointer to the SeeTimePointClass table.
 */
SEE_EXPORT const SeeTimePointClass*
see_time_point_class();

/* Expand the class with public functions here, don't forget the SEE_EXPORT
 * macro, because otherwise you'll run into troubles when exporting function
 * in a windows dll.
 */

/* **** class initialization functions **** */

/**
 * Initialize SeeTimePoint; make it ready for use.
 */
SEE_EXPORT int
see_time_point_init();

/**
 * Deinitialize SeeTimePoint, after SeeTimePoint has been deinitialized,
 * all functions in this header shouldn't be used anymore.
 */
SEE_EXPORT
void see_time_point_deinit();

#ifdef __cplusplus
}
#endif

#endif //ifndef SEE_TIME_POINT_H
