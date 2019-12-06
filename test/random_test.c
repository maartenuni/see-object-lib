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

#include <string.h>
#include "test_macros.h"
#include "../src/Error.h"
#include "../src/Random.h"

#define ASIZ  1024
#define ASIZ2 (ASIZ * ASIZ)

static const char* SUITE_NAME = "SeeRandom Suite";

static double
mean(const double* data, size_t n)
{
    double mean = 0.0;
    for (const double* p = data; p < data + n; ++p)
        mean += *p/n;
    return mean;
}

static double
standard_deviation(const double* data, size_t n)
{
    double avg = mean(data, n);
    double ss = 0.0;
    for (size_t i = 0; i < n; ++i) {
        ss += (data[i] - avg) * (data[i] - avg);
    }
    return sqrt(ss/(n-1));
}

static void
random_create(void)
{
    SeeError* error = NULL;
    SeeRandom* rgen = NULL;
    int ret;

    ret = see_random_new(&rgen, &error);
    SEE_UNIT_HANDLE_ERROR();

fail:
    SEE_OBJECT_DECREF(rgen);
    SEE_OBJECT_DECREF(error);
}

/**
 * \private
 * \brief test whether the seeding of the random devices works.
 *
 * In general the seeds of the different machines should be different.
 * Although there is a very small chance that they are the same.
 *
 * If 3 different generators are seeded with the same seed, they should
 * generate the same sequence.
 */
static void
random_seed(void)
{
    SeeError*  error = NULL;
    SeeRandom* rgen1 = NULL, *rgen2 = NULL;
    uint64_t seed1, seed2;
    uint64_t seed0; // from the global default random device.
    int ret;
    int rand0[ASIZ];
    int rand1[ASIZ];
    int rand2[ASIZ];
    int randstart[ASIZ]; // The second time we should generate the same
                         // as for the first time.
    int oneandtwoequal;  // are rand0 and rand1 equal
    int oneandthreequal; // are rand0 and rand2 equal
    // If both are equal, two and three are also equal.
    
    // Since other tests might have used the random device seed it once more.
    // The stored seed is the seed that was last used to seed the random
    // device. Hence once the generator is used it is not up to date anymore.
    uint64_t seed = see_random_uint64(NULL);
    see_random_seed(NULL, seed);
    
    ret = see_random_new(&rgen1, &error);
    SEE_UNIT_HANDLE_ERROR();

    ret = see_random_new(&rgen2, &error);
    SEE_UNIT_HANDLE_ERROR();

    ret = see_random_get_seed(rgen1, &seed1);
    SEE_UNIT_HANDLE_ERROR();
    ret = see_random_get_seed(rgen2, &seed2);
    SEE_UNIT_HANDLE_ERROR();
    ret = see_random_get_seed(NULL, &seed0);
    SEE_UNIT_HANDLE_ERROR();
    
    // Do we get different seeds to start with?
    CU_ASSERT_NOT_EQUAL(seed0, seed1);
    CU_ASSERT_NOT_EQUAL(seed0, seed2);

    for (size_t i = 0; i < ASIZ; i++) {
        rand0[i] = see_random_int32(NULL);
        randstart[i] = rand0[i]; // save the result from the first time.
        rand1[i] = see_random_int32(rgen1);
        rand2[i] = see_random_int32(rgen2);
    }

    oneandtwoequal = memcmp(rand0, rand1, sizeof(rand0));
    oneandthreequal= memcmp(rand0, rand2, sizeof(rand0));

    // Then we should get unequal results from the memcmp.
    CU_ASSERT_NOT_EQUAL(oneandtwoequal, 0);
    CU_ASSERT_NOT_EQUAL(oneandthreequal, 0);

    // Apply the same seeds to all generators.
    ret = see_random_seed(rgen1, seed0);
    SEE_UNIT_HANDLE_ERROR();
    ret = see_random_seed(rgen2, seed0);
    SEE_UNIT_HANDLE_ERROR();
    ret = see_random_seed(NULL, seed0);
    SEE_UNIT_HANDLE_ERROR();

    for (size_t i = 0; i < ASIZ; i++) {
        rand0[i] = see_random_int32(NULL);
        rand1[i] = see_random_int32(rgen1);
        rand2[i] = see_random_int32(rgen2);
    }

    oneandtwoequal = memcmp(rand0, rand1, sizeof(rand0));
    oneandthreequal= memcmp(rand0, rand2, sizeof(rand0));

    // And then now they should be equal.
    CU_ASSERT_EQUAL(oneandtwoequal, 0);
    CU_ASSERT_EQUAL(oneandthreequal, 0);
    CU_ASSERT_EQUAL(memcmp(randstart, rand0, sizeof(rand0)), 0);

fail:
    SEE_OBJECT_DECREF(error);
    SEE_OBJECT_DECREF(rgen1);
    SEE_OBJECT_DECREF(rgen2);
}

static void
random_int32_range(void)
{
    const int32_t min = -100;
    const int32_t max = 100;

    int32_t* array = malloc(ASIZ2 * sizeof(int32_t));
    CU_ASSERT_PTR_NOT_NULL(array);
    if (!array)
        goto fail;

    for (size_t i = 0; i < ASIZ2; i++)
        array[i] = see_random_int32_range(NULL, min, max);

    for (size_t i = 0; i < ASIZ2; i++) {
        int32_t rv = array[i];
        if (rv < min || rv > max) {
            CU_ASSERT(rv >= min && rv <= max);
            fprintf(stderr,
                    "failing value = %d not in range [%d,%d]\n",
                    rv,
                    min,
                    max
                    );
            goto fail;
        }
    }

fail:
    free(array);
}

static void
random_uint32_range(void)
{
    const uint32_t min = 100;
    const uint32_t max = 1000;

    uint32_t* array = malloc(ASIZ2 * sizeof(uint32_t));
    CU_ASSERT_PTR_NOT_NULL(array);
    if (!array)
        goto fail;

    for (size_t i = 0; i < ASIZ2; i++)
        array[i] = see_random_int32_range(NULL, min, max);

    for (size_t i = 0; i < ASIZ2; i++) {
        uint32_t rv = array[i];
        if (rv < min || rv > max) {
            CU_ASSERT(rv >= min && rv <= max);
            fprintf(stderr,
                    "failing value = %u not in range [%u,%u]\n",
                    rv,
                    min,
                    max
                    );
            goto fail;
        }
    }

fail:
    free(array);
}

static void
random_int64_range(void)
{
    const int64_t min = -100;
    const int64_t max = 100;

    int64_t* array = malloc(ASIZ2 * sizeof(int64_t));
    CU_ASSERT_PTR_NOT_NULL(array);
    if (!array)
        goto fail;

    for (size_t i = 0; i < ASIZ2; i++)
        array[i] = see_random_int64_range(NULL, min, max);

    for (size_t i = 0; i < ASIZ2; i++) {
        int64_t rv = array[i];
        if (rv < min || rv > max) {
            CU_ASSERT(rv >= min && rv <= max);
            fprintf(stderr,
                    "failing value = %ld not in range [%ld,%ld]\n",
                    rv,
                    min,
                    max
                    );
            goto fail;
        }
    }

fail:
    free(array);
}

static void
random_uint64_range(void)
{
    const uint64_t min = 100;
    const uint64_t max = 1000;

    uint64_t* array = malloc(ASIZ2 * sizeof(uint64_t));
    CU_ASSERT_PTR_NOT_NULL(array);
    if (!array)
        goto fail;

    for (size_t i = 0; i < ASIZ2; i++)
        array[i] = see_random_int64_range(NULL, min, max);

    for (size_t i = 0; i < ASIZ2; i++) {
        uint64_t rv = array[i];
        if (rv < min || rv > max) {
            CU_ASSERT(rv >= min && rv <= max);
            fprintf(stderr,
                    "failing value = %lu not in range [%lu,%lu]\n",
                    rv,
                    min,
                    max
                    );
            goto fail;
        }
    }

fail:
    free(array);
}


static void
random_float_range(void)
{
    const double min = 0.0;
    const double max = 1.0;

    double* array = malloc(ASIZ2 * sizeof(double));
    CU_ASSERT_PTR_NOT_NULL(array);
    if (!array)
        goto fail;

    for (size_t i = 0; i < ASIZ2; i++)
        array[i] = see_random_float_range(NULL, min, max);

    for (size_t i = 0; i < ASIZ2; i++) {
        double rv = array[i];
        if (rv < min || rv > max) {
            CU_ASSERT(rv >= min && rv <= max);
            fprintf(stderr,
                    "failing value = %f not in range [%f,%f]\n",
                    rv,
                    min,
                    max
                    );
            goto fail;
        }
    }

fail:
    free(array);
}

static void
random_normal(void)
{
    const double avg = 3.141592654;
    const double stdev = 1.0;

    double* array = malloc(ASIZ2 * sizeof(double));
    CU_ASSERT_PTR_NOT_NULL(array);
    if (!array)
        goto fail;

    for (size_t i = 0; i < ASIZ2; i++)
        array[i] = see_random_normal_float(NULL, avg, stdev);

    double found_mean = mean(array, ASIZ2);
    double found_stdev = standard_deviation(array, ASIZ2);

    CU_ASSERT_DOUBLE_EQUAL(found_mean, avg, 0.01);
    CU_ASSERT_DOUBLE_EQUAL(found_stdev, stdev, 0.01);

fail:
    free(array);
}

int
add_random_suite(void)
{
    SEE_UNIT_SUITE_CREATE(NULL, NULL);
    SEE_UNIT_TEST_CREATE(random_create);
    SEE_UNIT_TEST_CREATE(random_seed);
    SEE_UNIT_TEST_CREATE(random_int32_range);
    SEE_UNIT_TEST_CREATE(random_uint32_range);
    SEE_UNIT_TEST_CREATE(random_int64_range);
    SEE_UNIT_TEST_CREATE(random_uint64_range);
    SEE_UNIT_TEST_CREATE(random_float_range);
    SEE_UNIT_TEST_CREATE(random_normal);

    return 0;
}
