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


#ifndef SEE_CLOCK_H
#define SEE_CLOCK_H

#include "SeeObject.h"
#include "Error.h"
#include "TimePoint.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _SeeClock SeeClock;
typedef struct _SeeClockClass SeeClockClass;

struct _SeeClock {
    SeeObject parent_obj;
    void*     priv_clk;
};

struct _SeeClockClass {
    SeeObjectClass parent_cls;
    
    int (*clock_init)(
        SeeClock*            clock,
        const SeeClockClass* clock_cls,
        SeeError**           error_out
        );

    int (*time) (
        const SeeClock*     clock,
        SeeTimePoint**      out,
        SeeError**          error_out
        );
        
    /* expand SeeClock class with extra functions here.*/
};

/* **** function style macro casts **** */

/**
 * \brief cast a pointer from a SeeClock derived instance back to a
 *        pointer to SeeClock.
 */
#define SEE_CLOCK(obj)                      \
    ((SeeClock*) obj)

/**
 * \brief cast a pointer to pointer from a SeeClock derived instance back to a
 *        reference to SeeClock*.
 */
#define SEE_CLOCK_REF(ref)                      \
    ((SeeClock**) ref)

/**
 * \brief cast a pointer to SeeClockClass derived class back to a
 *        pointer to SeeClockClass.
 */
#define SEE_CLOCK_CLASS(cls)                      \
    ((const SeeClockClass*) cls)

/**
 * \brief obtain a pointer to SeeClockClass from a instance of
 *        derived from SeeClock. This macro is preferably
 *        used when obtaining the class of a instance. When this
 *        macro is used. Calling methods on the class will enable
 *        polymorphism, because you'll get the derived class.
 */
#define SEE_CLOCK_GET_CLASS(obj)                \
    (SEE_CLOCK_CLASS(see_object_get_class(SEE_OBJECT(obj)) )  )

/* **** public functions **** */

/**
 * @brief create a new clock object;
 */
SEE_EXPORT int
see_clock_new(SeeClock** out, SeeError** error_out);

/**
 * @brief Obtain time from the clock
 *
 * @param [in] in   the clock from which we would like to the time.
 * @param [in,out]  tp The time is returned here. If inout already
 *                        contains a time it will be reinitialized with a
 *                        new time. If *tp == NULL, a new instance will be
 *                        allocated.
 * @param [out]     error_out, if an error occurs the error
 *                  The error will be returned here.
 * @return SEE_SUCCESS, SEE_INVALID_ARGUMENT, SEE_ERROR_RUNTIME
 */
SEE_EXPORT int
see_clock_time(const SeeClock* in, SeeTimePoint** tp, SeeError** error_out);

/**
 * @brief Obtain the resolution of the clock.
 *
 * The resolution of a clock is finite. This value represents the smallest
 * units in which the clock can be incremented, Although the clock will probably
 * return timepoints specified in nanoseconds, it might easily be that
 * the smallest increment of the clock is multiple of a nanosecond.
 *
 * @param [in]      clk the clock for which you would like to know the
 *                  resolution.
 * @param [in, out] resolution *resolution may be NULL, if not NULL
 *                  the duration will be reset to the resolution of the clock.
 * @param [out]     error_out if an error occurres it will be returned here
 *
 * @return SEE_SUCCESS, SEE_ERROR_RUNTIME
 */
SEE_EXPORT int
see_clock_resolution(
    const SeeClock* clk,
    SeeDuration** resolution,
    SeeError** error_out
    );

/**
 * Gets the pointer to the SeeClockClass table.
 */
SEE_EXPORT const SeeClockClass*
see_clock_class();

/* Expand the class with public functions here, don't forget the SEE_EXPORT
 * macro, because otherwise you'll run into troubles when exporting function
 * in a windows dll.
 */

/* **** class initialization functions **** */

/**
 * Initialize SeeClock; make it ready for use.
 */
SEE_EXPORT
int see_clock_init();

/**
 * Deinitialize SeeClock, after SeeClock has been deinitialized,
 * all functions in this header shouldn't be used anymore.
 */
SEE_EXPORT
void see_clock_deinit();

#ifdef __cplusplus
}
#endif

#endif //ifndef SEE_CLOCK_H
