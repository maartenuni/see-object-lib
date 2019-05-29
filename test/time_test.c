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

#include "test_macros.h"
#include "../src/Clock.h"
#include "../src/Duration.h"
#include "../src/TimePoint.h"
#include "../src/utilities.h"

static const char* SUITE_NAME = "Time test";

void clock_create(void)
{
    int ret;
    SeeClock* clk = NULL;
    SeeError* error = NULL;

    ret = see_clock_new(&clk, &error);
    CU_ASSERT_EQUAL(ret, SEE_SUCCESS);
    if (ret) {
        if (error) {
            fprintf(stderr, "Unable to create clock: %s",
                see_error_msg(error)
                );
            see_object_decref(SEE_OBJECT(error));
        }
        return;
    }
    CU_ASSERT_PTR_NULL(error);
    CU_ASSERT_PTR_NOT_NULL(clk);
    see_object_decref(SEE_OBJECT(clk));
}

void duration_create(void)
{
    int ret;
    SeeDuration* dur = NULL;
    SeeError* error = NULL;

    ret = see_duration_new(&dur, &error);
    CU_ASSERT_EQUAL(ret, SEE_SUCCESS);
    if (ret) {
        if (error) {
            fprintf(stderr, "Unable to create duration: %s",
                see_error_msg(error)
            );
            see_object_decref(SEE_OBJECT(error));
        }
        return;
    }
    CU_ASSERT_PTR_NULL(error);
    CU_ASSERT_PTR_NOT_NULL(dur);
    see_object_decref(SEE_OBJECT(dur));
}

void time_point_create(void)
{
    int ret;
    SeeTimePoint* tp = NULL;
    SeeError* error = NULL;

    ret = see_time_point_new(&tp, &error);
    CU_ASSERT_EQUAL(ret, SEE_SUCCESS);
    if (ret) {
        if (error) {
            fprintf(stderr, "Unable to create duration: %s",
                see_error_msg(error)
            );
            see_object_decref(SEE_OBJECT(error));
        }
        return;
    }
    CU_ASSERT_PTR_NULL(error);
    CU_ASSERT_PTR_NOT_NULL(tp);
    see_object_decref(SEE_OBJECT(tp));
}

void clock_use(void)
{
    SeeClock* clk       = NULL;
    SeeError* error     = NULL;
    SeeTimePoint* tstart= NULL, *tend = NULL, *t1 = NULL, *t2 = NULL;
    SeeDuration* dur    = NULL;

    int ret = see_clock_new(&clk, &error);
    CU_ASSERT_EQUAL(ret, SEE_SUCCESS);
    if (ret) {
        fprintf(stderr, "Unable to create a clock\n");
        see_object_decref(SEE_OBJECT(error));
        return;
    }
    
    ret = see_clock_time(clk, &tstart, &error);
    CU_ASSERT_EQUAL(ret, SEE_SUCCESS);
    if (ret) {
        fprintf(stderr, "Unable to retrieve time: %s\n",
            see_error_msg(error)
            );
        goto fail;
    }
     
    ret = see_clock_time(clk, &t1, &error);
    CU_ASSERT_EQUAL(ret, SEE_SUCCESS);
    
    int monotonic = 1;
    for (size_t i = 0; i < 1000; i++) {

        ret = see_clock_time(clk, &t2, &error);
        if (ret) {
            CU_ASSERT(ret == SEE_SUCCESS);
            goto fail;
        }

        ret = see_time_point_sub(t2, t1, &dur, &error);
        if (ret) {
            CU_ASSERT(ret == SEE_SUCCESS);
            goto fail;
        }

        int64_t nanos = see_duration_nanos(dur);
        if (nanos < 0)
            monotonic = 0;

        ret = see_time_point_set(t1, t2, &error);
        if (ret) {
            CU_ASSERT(ret == SEE_SUCCESS);
            goto fail;
        }
    }
    CU_ASSERT_TRUE(monotonic);

    ret = see_clock_time(clk, &tend, &error);
    CU_ASSERT(ret == SEE_SUCCESS);
    if (ret)
        goto fail;

    ret = see_time_point_sub(tend, tstart, &dur, &error);
    CU_ASSERT(ret == SEE_SUCCESS);
    if (ret)
        goto fail;

    volatile int64_t nanos = see_duration_nanos(dur);
    CU_ASSERT(nanos >= 0);
    
fail:
    see_object_decref(SEE_OBJECT(clk));
    see_object_decref(SEE_OBJECT(error));
    see_object_decref(SEE_OBJECT(t1));
    see_object_decref(SEE_OBJECT(t2));
    see_object_decref(SEE_OBJECT(tstart));
    see_object_decref(SEE_OBJECT(tend));
    see_object_decref(SEE_OBJECT(dur));
}

#define UNIT_HANDLE_ERROR()                                 \
    if (ret != SEE_SUCCESS) {                               \
        CU_ASSERT(ret == SEE_SUCCESS);                      \
        if (error){                                         \
            fprintf(stderr, "%s:%d: oops \"%s\"\n",         \
                __FILE__, __LINE__, see_error_msg(error)    \
                );                                          \
        }                                                   \
        else {                                              \
            fprintf(stderr, "%s:%d: oops...\n",             \
                __FILE__, __LINE__);                        \
        }                                                   \
        goto fail;                                          \
    }\

void dur_init(void)
{
    SeeDuration *dur1 = NULL;
    SeeError* error = NULL;

    const int64_t zero      = 0;
    const int64_t thousend  = 1000;
    const int64_t million   = 1000000;
    const int64_t billion   = 1000000000;

    int ret = see_duration_new(&dur1, &error);
    UNIT_HANDLE_ERROR();
    CU_ASSERT_EQUAL(see_duration_nanos(dur1), zero);
    see_object_decref(SEE_OBJECT(dur1)); dur1 = NULL;

    ret = see_duration_new_ns(&dur1, 1, &error);
    UNIT_HANDLE_ERROR();
    CU_ASSERT_EQUAL(see_duration_nanos(dur1), 1);
    see_object_decref(SEE_OBJECT(dur1)); dur1 = NULL;

    ret = see_duration_new_us(&dur1, 1, &error);
    UNIT_HANDLE_ERROR();
    CU_ASSERT_EQUAL(see_duration_nanos(dur1), thousend);
    CU_ASSERT_EQUAL(see_duration_micros(dur1), 1);
    see_object_decref(SEE_OBJECT(dur1)); dur1 = NULL;

    ret = see_duration_new_ms(&dur1, 1, &error);
    UNIT_HANDLE_ERROR();
    CU_ASSERT_EQUAL(see_duration_nanos(dur1), million);
    CU_ASSERT_EQUAL(see_duration_millis(dur1), 1);
    see_object_decref(SEE_OBJECT(dur1)); dur1 = NULL;

    ret = see_duration_new_s(&dur1, 1, &error);
    UNIT_HANDLE_ERROR();
    CU_ASSERT_EQUAL(see_duration_nanos(dur1), billion);
    CU_ASSERT_EQUAL(see_duration_seconds(dur1), 1);
    see_object_decref(SEE_OBJECT(dur1)); dur1 = NULL;
    
fail:

    see_object_decref(SEE_OBJECT(dur1));
    see_object_decref(SEE_OBJECT(error));
}

void time_comparison(void)
{
    SeeTimePoint* t1    = NULL, *t2 = NULL, *t3 = NULL, *te = NULL;
    SeeDuration *d1     = NULL, *d2 = NULL, *d3 = NULL, *de = NULL;
    SeeError* error     = NULL;
    SeeClock* clk       = NULL;
    SeeDuration *sleep_dur = NULL;

    int ret, result_true, result_false, greater, smaller, equal;

    ret = see_duration_new_ms(&sleep_dur, 1, &error);
    UNIT_HANDLE_ERROR();

    // Comparing the times
    ret = see_clock_new(&clk, &error);
    UNIT_HANDLE_ERROR();
    ret = see_clock_time(clk, &t1, &error);
    see_sleep(sleep_dur);
    UNIT_HANDLE_ERROR();
    ret = see_clock_time(clk, &t2, &error);
    UNIT_HANDLE_ERROR();
    see_sleep(sleep_dur);
    ret = see_clock_time(clk, &t3, &error);
    UNIT_HANDLE_ERROR();
    ret = see_time_point_sub(t2, t1, &d1, &error);
    UNIT_HANDLE_ERROR();
    ret = see_time_point_sub(t3, t1, &d2, &error);
    UNIT_HANDLE_ERROR();
    ret = see_duration_add(d2, d1, &d3, &error);
    UNIT_HANDLE_ERROR();

    see_time_point_new(&te, &error);
    see_time_point_set(te, t2, &error);
    see_duration_new(&de, &error);
    see_duration_set(de, d2, &error);

    ret = see_time_point_lt(t1, t2, &result_true);
    UNIT_HANDLE_ERROR();
    ret = see_time_point_lt(t2, t1, &result_false);
    UNIT_HANDLE_ERROR();
    CU_ASSERT_TRUE(result_true);
    CU_ASSERT_FALSE(result_false);

    ret = see_time_point_lte(t1, t2, &result_true);
    UNIT_HANDLE_ERROR();
    ret = see_time_point_lte(t2, t1, &result_false);
    UNIT_HANDLE_ERROR();
    CU_ASSERT_TRUE(result_true);
    CU_ASSERT_FALSE(result_false);
    ret = see_time_point_lte(t2, te, &result_true);
    UNIT_HANDLE_ERROR();
    ret = see_time_point_lte(t3, t2, &result_false);
    UNIT_HANDLE_ERROR();
    CU_ASSERT_NOT_EQUAL(result_true, 0);
    CU_ASSERT_EQUAL(result_false, 0);

    ret = see_time_point_eq(t2, te, &result_true);
    UNIT_HANDLE_ERROR();
    ret = see_time_point_eq(t2, t3, &result_false);
    UNIT_HANDLE_ERROR();
    CU_ASSERT_FALSE(result_false);
    CU_ASSERT_TRUE(result_true);

    ret = see_time_point_gte(t3, t2, &result_true);
    UNIT_HANDLE_ERROR();
    ret = see_time_point_gte(t2, t3, &result_false);
    UNIT_HANDLE_ERROR();
    CU_ASSERT_TRUE(result_true);
    CU_ASSERT_FALSE(result_false);
    ret = see_time_point_gte(t2, te, &result_true);
    UNIT_HANDLE_ERROR();
    ret = see_time_point_gte(t1, t3, &result_false);
    UNIT_HANDLE_ERROR();
    CU_ASSERT_NOT_EQUAL(result_true, 0);
    CU_ASSERT_EQUAL(result_false, 0);

    ret = see_time_point_gt(t3, t2, &result_true);
    UNIT_HANDLE_ERROR();
    ret = see_time_point_gt(t1, t3, &result_false);
    UNIT_HANDLE_ERROR();
    CU_ASSERT_TRUE(result_true);
    CU_ASSERT_FALSE(result_false);

    // comparing the durations
 
    ret = see_object_less(
        SEE_OBJECT(d1),
        SEE_OBJECT(d2),
        &result_true,
        &error
        );
    UNIT_HANDLE_ERROR();
    ret = see_object_less(
        SEE_OBJECT(d2),
        SEE_OBJECT(d1),
        &result_false,
        &error
        );
    UNIT_HANDLE_ERROR();
    CU_ASSERT_TRUE(result_true);
    CU_ASSERT_FALSE(result_false);

    ret = see_object_less_equal(
        SEE_OBJECT(d1),
        SEE_OBJECT(d2),
        &result_true,
        &error
        );
    UNIT_HANDLE_ERROR();
    ret = see_object_less_equal(
        SEE_OBJECT(d2),
        SEE_OBJECT(d1),
        &result_false,
        &error
        );
    UNIT_HANDLE_ERROR();
    CU_ASSERT_TRUE(result_true);
    CU_ASSERT_FALSE(result_false);
    ret = see_object_less_equal(
        SEE_OBJECT(d2),
        SEE_OBJECT(de),
        &result_true,
        &error
        );
    UNIT_HANDLE_ERROR();
    ret = see_object_less_equal(
        SEE_OBJECT(d3),
        SEE_OBJECT(d2),
        &result_false,
        &error
        );
    UNIT_HANDLE_ERROR();
    CU_ASSERT_NOT_EQUAL(result_true, 0);
    CU_ASSERT_EQUAL(result_false, 0);

    ret = see_object_equal(
        SEE_OBJECT(d2),
        SEE_OBJECT(de),
        &result_true,
        &error
        );
    UNIT_HANDLE_ERROR();
    ret = see_object_equal(
        SEE_OBJECT(d2),
        SEE_OBJECT(d3),
        &result_false,
        &error
        );
    UNIT_HANDLE_ERROR();
    CU_ASSERT_FALSE(result_false);
    CU_ASSERT_TRUE(result_true);

    ret = see_object_greater_equal(
        SEE_OBJECT(d3),
        SEE_OBJECT(d2),
        &result_true,
        &error
        );
    UNIT_HANDLE_ERROR();
    ret = see_object_greater_equal(
        SEE_OBJECT(d2),
        SEE_OBJECT(d3),
        &result_false,
        &error
        );
    UNIT_HANDLE_ERROR();
    CU_ASSERT_TRUE(result_true);
    CU_ASSERT_FALSE(result_false);

    ret = see_object_greater_equal(
        SEE_OBJECT(d2),
        SEE_OBJECT(de),
        &result_true,
        &error
        );
    UNIT_HANDLE_ERROR();
    ret = see_object_greater_equal(
        SEE_OBJECT(d1),
        SEE_OBJECT(d3),
        &result_false,
        &error
        );
    UNIT_HANDLE_ERROR();
    CU_ASSERT_NOT_EQUAL(result_true, 0);
    CU_ASSERT_EQUAL(result_false, 0);

    ret = see_object_greater(
        SEE_OBJECT(d3),
        SEE_OBJECT(d2),
        &result_true,
        &error
        );
    UNIT_HANDLE_ERROR();
    ret = see_object_greater(
        SEE_OBJECT(d1),
        SEE_OBJECT(d3),
        &result_false,
        &error
        );
    UNIT_HANDLE_ERROR();
    CU_ASSERT_TRUE(result_true);
    CU_ASSERT_FALSE(result_false);

    ret = see_clock_time(clk, &t1, &error);
    UNIT_HANDLE_ERROR();
    ret = see_sleep(sleep_dur);
    CU_ASSERT_EQUAL(ret, SEE_SUCCESS);

    ret = see_clock_time(clk, &t2, &error);
    UNIT_HANDLE_ERROR();
    ret = see_sleep(sleep_dur);
    CU_ASSERT_EQUAL(ret, SEE_SUCCESS);

    ret = see_clock_time(clk, &t3, &error);
    UNIT_HANDLE_ERROR();

    ret = see_object_greater(
        SEE_OBJECT(t3),
        SEE_OBJECT(t2),
        &greater,
        &error
    );
    UNIT_HANDLE_ERROR();
    CU_ASSERT_TRUE(greater);

    ret = see_object_less(
        SEE_OBJECT(t2),
        SEE_OBJECT(t3),
        &smaller,
        &error
        );
    UNIT_HANDLE_ERROR();

    CU_ASSERT(smaller);

    ret = see_object_less(
        SEE_OBJECT(t3),
        SEE_OBJECT(t2),
        &smaller,
        &error
    );
    UNIT_HANDLE_ERROR();
    CU_ASSERT_FALSE(smaller);

    ret = see_object_equal(
        SEE_OBJECT(t3),
        SEE_OBJECT(t3),
        &equal,
        &error
        );
    UNIT_HANDLE_ERROR();

    CU_ASSERT(equal);
    ret = see_object_equal(
        SEE_OBJECT(t3),
        SEE_OBJECT(t1),
        &equal,
        &error
    );
    UNIT_HANDLE_ERROR();
    CU_ASSERT_FALSE(equal);

fail:
    see_object_decref(SEE_OBJECT(t1));
    see_object_decref(SEE_OBJECT(t2));
    see_object_decref(SEE_OBJECT(t3));
    see_object_decref(SEE_OBJECT(te));
    see_object_decref(SEE_OBJECT(d1));
    see_object_decref(SEE_OBJECT(d2));
    see_object_decref(SEE_OBJECT(d3));
    see_object_decref(SEE_OBJECT(de));
    see_object_decref(SEE_OBJECT(clk));
    see_object_decref(SEE_OBJECT(sleep_dur));
    see_object_decref(SEE_OBJECT(error));
}

void time_calculations(void)
{
    SeeDuration *d1     = NULL, *d2 = NULL, *dres = NULL;
    SeeTimePoint *t1    = NULL, *tres = NULL;
    SeeError* error     = NULL;
    SeeClock* clk       = NULL;
    int result;
    int ret;

    ret = see_duration_new_ns(&d1, 250, &error);
    UNIT_HANDLE_ERROR();
    ret = see_duration_new_us(&d2, 1, &error);
    UNIT_HANDLE_ERROR();
    ret = see_duration_add(d1, d2, &dres, &error);
    UNIT_HANDLE_ERROR();
    CU_ASSERT_EQUAL(see_duration_nanos(dres), 1250);

    ret = see_duration_add(dres, d1, &dres, &error);
    UNIT_HANDLE_ERROR();
    CU_ASSERT_EQUAL(see_duration_nanos(dres), 1500);
    ret = see_duration_add(dres, d1, &dres, &error);
    UNIT_HANDLE_ERROR();
    CU_ASSERT_EQUAL(see_duration_nanos(dres), 1750);
    CU_ASSERT_EQUAL(see_duration_micros(dres), 1);

    // Cleanup stuff
    see_object_decref(SEE_OBJECT(d1));
    see_object_decref(SEE_OBJECT(d2));
    see_object_decref(SEE_OBJECT(dres));
    d1 = d2 = dres = NULL;

    ret = see_duration_new_ns(&d1, 250, &error);
    UNIT_HANDLE_ERROR();
    ret = see_duration_new_ns(&d2, 500, &error);
    UNIT_HANDLE_ERROR();
    ret = see_duration_sub(d1, d2, &dres, &error);
    UNIT_HANDLE_ERROR();
    CU_ASSERT_EQUAL(see_duration_nanos(dres), -250);

    ret = see_clock_new(&clk, &error);
    UNIT_HANDLE_ERROR();
    ret = see_clock_time(clk, &t1, &error);
    UNIT_HANDLE_ERROR();
    ret = see_time_point_add_dur(t1, d1, &tres, &error);
    UNIT_HANDLE_ERROR();
    ret = see_time_point_sub(tres, t1, &dres, &error);
    UNIT_HANDLE_ERROR();
    ret = see_object_equal(
        SEE_OBJECT(dres),
        SEE_OBJECT(d1),
        &result,
        &error
        );
    UNIT_HANDLE_ERROR();
    CU_ASSERT_NOT_EQUAL(result, 0);

fail:

    see_object_decref(SEE_OBJECT(error));
    see_object_decref(SEE_OBJECT(clk));
    see_object_decref(SEE_OBJECT(d1));
    see_object_decref(SEE_OBJECT(d2));
    see_object_decref(SEE_OBJECT(dres));
    see_object_decref(SEE_OBJECT(t1));
    see_object_decref(SEE_OBJECT(tres));
}

void clock_duration(void)
{
    int ret, result;
    SeeClock*       clk     = NULL;
    SeeError*       error   = NULL;
    SeeDuration*    d1      = NULL;
    SeeDuration*    d2      = NULL;

    ret = see_clock_new(&clk, &error);
    UNIT_HANDLE_ERROR();
    ret = see_clock_duration(clk, &d1, &error);
    UNIT_HANDLE_ERROR();
    ret = see_clock_duration(clk, &d2, &error);
    UNIT_HANDLE_ERROR();

    int64_t s1 = see_duration_seconds(d1);
    int64_t s2 = see_duration_seconds(d2);
    (void) s1; // Just to see in a debugger how large d1 and d2 can be.
    (void) s2;
    see_object_less(
        SEE_OBJECT(d1),
        SEE_OBJECT(d2),
        &result,
        &error
        );
    CU_ASSERT_TRUE(result);
    ret = see_clock_set_base_time(clk, NULL, &error);
    UNIT_HANDLE_ERROR();
    ret = see_clock_duration(clk, &d2, &error);
    UNIT_HANDLE_ERROR();
    see_object_greater(
        SEE_OBJECT(d1),
        SEE_OBJECT(d2),
        &result,
        &error
        );
    CU_ASSERT_TRUE(result);
    s1 = see_duration_seconds(d1);
    s2 = see_duration_seconds(d2);

    fail:

    see_object_decref(SEE_OBJECT(clk));
    see_object_decref(SEE_OBJECT(error));
    see_object_decref(SEE_OBJECT(d1));
    see_object_decref(SEE_OBJECT(d2));
}

int add_time_suite()
{
    SEE_UNIT_SUITE_CREATE(NULL, NULL);
    SEE_UNIT_TEST_CREATE(clock_create);
    SEE_UNIT_TEST_CREATE(duration_create);
    SEE_UNIT_TEST_CREATE(time_point_create);
    SEE_UNIT_TEST_CREATE(clock_use);
    SEE_UNIT_TEST_CREATE(dur_init);
    SEE_UNIT_TEST_CREATE(time_comparison);
    SEE_UNIT_TEST_CREATE(time_calculations);
    SEE_UNIT_TEST_CREATE(clock_duration);

    return 0;
}

