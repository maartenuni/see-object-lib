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
 * \file Clock.h Exports the api of the SeeClock class and object.
 * 
 * \brief A clock can return Specific TimePoints.
 * 
 * The Clock used is the std::chrono::steady_clock. This clock is
 * a clock that typically has its start at boot time of the computer.
 * Since that epoch, it will always continues to run and it won't be
 * adjusted in contrast with a wallclock, that can be adjusted (think summer,
 * winter time or leap seconds). 
 * 
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

/**
 * Implementation of the clock.
 */
struct _SeeClock {
    SeeObject parent_obj;
    /**
     * \private
     * The implementation of the clock.
     */
    void*           priv_clk;
    SeeTimePoint*   base_time;
};

/**
 * @brief the SeeClockClass the class defines some of the 
 * virtual operation that can be done on the clock.
 */
struct _SeeClockClass {
    SeeObjectClass parent_cls;
    
    /**
     * @brief Initialize a SeeClock object
     * @param clock the clock to be initialized.
     * @param clock_cls A pointer to the class that implements the
     *                  virtual operations
     * @param error_out A pointer to a SeeError where occasional errors 
     *                  can be returned.
     * @return 
     */
    int (*clock_init)(
        SeeClock*            clock,
        const SeeClockClass* clock_cls,
        SeeError**           error_out
        );

    /**
     * @brief Return the current time (since boot) that the clock has been
     *        running.
     * @param clock 
     * @param out 
     * @param error_out 
     * @return 
     */
    int (*time) (
        const SeeClock*     clock,
        SeeTimePoint**      out,
        SeeError**          error_out
        );


    /**
     * @brief Get the duration since the base time.
     *
     * The clock stores an internal timepoint. This is by default the time
     * since clock started, which is most likely since the pc has booted.
     * The base time can be set by the user via the set_base_time() function.
     *
     * @param [in]  clock
     * @param [out] out  The duration since the basetime
     * @param [out] error_out
     * @return SEE_SUCCESS, SEE_INVALID_ARGUMENT
     */
    int (*duration) (
            const SeeClock* clock,
            SeeDuration**   out,
            SeeError**      error_out
            );

    /**
     * @brief Reset the base time to a new value.
     *
     * @param [in] clock
     * @param [in] base_time May be null, then the clock will set the basetime
     *                       to the current time of the clock. Otherwise,
     *                       it will be set to base_time.
     * @param [out] If an error occurs it will be returned here.
     *
     * @return SEE_SUCCESS, SEE_INVALID_ARGUMENT
     */
    int (*set_base_time)(
            SeeClock*           clock,
            const SeeTimePoint* base_time,
            SeeError**          error_out
            );

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
 * @brief Create a new clock object.
 * @param [out] clk  clk may not be NULL, whereas *clk should.
 * @param [out] error_out 
 * @return SEE_SUCCESS, SEE_INVALID_ARGUMENT, SEE_RUNTIME_ERROR
 */
SEE_EXPORT int
see_clock_new(SeeClock** clk, SeeError** error_out);

/**
 * @brief Obtain time from the clock
 *
 * @param [in]      in the clock from which we would like to the time.
 * @param [in,out]  tp The time is returned here. If inout already
 *                        contains a time it will be reinitialized with a
 *                        new time. If *tp == NULL, a new instance will be
 *                        allocated.
 * @param [out]     error_out if an error occurs the error will be returned
 *                      here.
 *
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
 * @brief Get the duration since the base time of the clock.
 * @param [in]  clk         The pointer to the clock
 * @param [out] dur_out     A Reference to a SeeDuration, *dur_out may be NULL,
 *                          then a new duration will be returned.
 * @param [out] error_out   If an error occurs it will be returned here.
 * @return SEE_SUCCESS, SEE_INVALID_ARGUMENT, SEE_ERROR_RUNTIME
 */
SEE_EXPORT int
see_clock_duration(
    const SeeClock* clk,
    SeeDuration**   dur_out,
    SeeError**      error_out
    );

/**
 * @brief Set a new basetime for the clock.
 * @param [in]  clk        The clock whose basetime we would like to set.
 * @param [in]  tp         The new timepoint for the clock or null to take
 *                         the current time as base time.
 * @param [out] error_out  If an error occurs it might be returned here.
 * @return
 */
SEE_EXPORT int
see_clock_set_base_time(
    SeeClock*       clk,
    SeeTimePoint*   tp,
    SeeError**      error_out
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
