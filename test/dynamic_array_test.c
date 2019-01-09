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

#include <assert.h>
#include "test_macros.h"
#include "../src/DynamicArray.h"

static const char* SUITE_NAME = "Dynamic array test";

#define TEST_N 10

/* **** utility functions **** */

int two_int_arrays_equal(int* a, int* b, size_t n)
{
    for (size_t i = 0; i < n; i++)
        if (a[i] != b[i])
            return 0;

    return 1;
}

/* **** unit tests **** */

static void array_create(void)
{
    SeeDynamicArray* array = NULL;
    int ret;
    ret = see_dynamic_array_new(&array, sizeof(int), NULL, NULL, NULL);
    CU_ASSERT_EQUAL(ret, SEE_SUCCESS);
    if (ret)
        return;

    CU_ASSERT_EQUAL(0, see_dynamic_array_capacity(array));
    CU_ASSERT_EQUAL(0, see_dynamic_array_size(array));

    see_object_decref(SEE_OBJECT(array));
}

static void array_create_capacity(void)
{
    const size_t desired_capacity = 10;
    SeeDynamicArray* array = NULL;
    int ret;
    ret = see_dynamic_array_new_capacity(
        &array, sizeof(int), NULL, NULL, NULL, desired_capacity
        );

    CU_ASSERT_EQUAL(ret, SEE_SUCCESS);
    if (ret)
        return;

    CU_ASSERT_EQUAL(see_dynamic_array_capacity(array), desired_capacity);
    CU_ASSERT_EQUAL(see_dynamic_array_size(array), 0);

    for (size_t i = 0; i < desired_capacity; i++) {
        if (see_dynamic_array_add(array, &i) != SEE_SUCCESS) {
            // this shouldn't happen realistically.
            see_object_decref((SeeObject*) array);
            CU_ASSERT(CU_FALSE);
            return;
        }
    }
    CU_ASSERT_EQUAL(see_dynamic_array_capacity(array), desired_capacity);
    CU_ASSERT_EQUAL(
        see_dynamic_array_capacity(array), see_dynamic_array_size(array)
        );
    int value = 1;
    ret = see_dynamic_array_add(array, &value);
    CU_ASSERT_EQUAL(ret, SEE_SUCCESS);
    CU_ASSERT_EQUAL(see_dynamic_array_capacity(array), desired_capacity * 2);

    see_object_decref(SEE_OBJECT(array));
}


static void array_add(void)
{
    int input[TEST_N] = {0,1,2,3,4,5,6,7,8,9};
    SeeDynamicArray* array = NULL;
    int ret;
    ret = see_dynamic_array_new(&array, sizeof(int), NULL, NULL, NULL);
    CU_ASSERT_EQUAL(ret, SEE_SUCCESS);
    if (ret)
        return;

    for (size_t i = 0; i < TEST_N; i++) {
        ret = see_dynamic_array_add(array, &input[i]);
        if (ret)
            break;
    }

    int* elements_start = see_dynamic_array_get(array, 0);
    CU_ASSERT(two_int_arrays_equal(elements_start, input, TEST_N));

    CU_ASSERT_EQUAL(ret, SEE_SUCCESS);
    CU_ASSERT_EQUAL(TEST_N, see_dynamic_array_size(array));

    size_t power_of_two = 1;
    while (power_of_two < TEST_N)
        power_of_two =  power_of_two << 1;

    CU_ASSERT_EQUAL(power_of_two, see_dynamic_array_capacity(array));

    see_object_decref(SEE_OBJECT(array));
}

static void array_set(void)
{
    // This check should also be run with valgrind, the elements should
    // be freed when the array is destroyed, but also when a new
    // element is set on the position of an old value. So the array should
    // free all mallocs in this function.

    int input1[TEST_N/2] = {0,1,2,3,4};
    int input2[TEST_N/2] = {5,6,7,8,9};

    SeeDynamicArray* array = NULL;
    int ret;

    ret = see_dynamic_array_new(&array, sizeof(int*), NULL, NULL, free);
    CU_ASSERT_EQUAL(ret, SEE_SUCCESS);
    if (ret)
        return;

    for (size_t i = 0; i < TEST_N/2; i++) {
        int* val = malloc(sizeof(int));
        assert(val);
        *val = input1[i];
        see_dynamic_array_add(array, &val);
    }
    CU_ASSERT_EQUAL(see_dynamic_array_size(array), TEST_N/2);

    int matches = 1;
    const int** int_ptr_array = see_dynamic_array_get(array, 0);
    for (size_t i = 0; i < TEST_N/2 ;i++) {
        if (input1[i] != *int_ptr_array[i])
            matches = 0;
    }
    CU_ASSERT_EQUAL(matches, 1);

    for (size_t i = 0; i < TEST_N/2; i++) {
        int* val = malloc(sizeof(int));
        assert(val);
        *val = input2[i];
        ret = see_dynamic_array_set(array, i, &val);
        assert(ret == SEE_SUCCESS);
    }
    CU_ASSERT_EQUAL(see_dynamic_array_size(array), TEST_N/2);

    matches = 1;
    int_ptr_array = see_dynamic_array_get(array, 0);
    for (size_t i = 0; i < TEST_N/2 ;i++) {
        if (input2[i] != *int_ptr_array[i])
            matches = 0;
    }
    CU_ASSERT_EQUAL(matches, 1);
    CU_ASSERT_EQUAL(see_dynamic_array_size(array), TEST_N/2);

    see_object_decref(SEE_OBJECT(array));
}

static void array_capacity(void)
{
    const size_t CAPACITY = 100;
    const size_t SIZE     = 10;
    SeeDynamicArray* array = NULL;
    int ret;
    ret = see_dynamic_array_new(&array, sizeof(int), NULL, NULL, NULL);
    CU_ASSERT_EQUAL(ret, SEE_SUCCESS);
    if (ret)
        return;

    CU_ASSERT_EQUAL(see_dynamic_array_capacity(array), 0);
    see_dynamic_array_reserve(array, CAPACITY);
    for (size_t i = 0; i < SIZE; i++)
        see_dynamic_array_add(array, &ret);

    CU_ASSERT_EQUAL(see_dynamic_array_capacity(array), CAPACITY);
    CU_ASSERT_EQUAL(see_dynamic_array_size(array), SIZE);

    see_dynamic_array_shrink_to_fit(array);

    CU_ASSERT_EQUAL(
        see_dynamic_array_size(array), see_dynamic_array_capacity(array)
        );

    see_object_decref(SEE_OBJECT(array));
}

int add_dynamic_array_suite()
{
    SEE_UNIT_SUITE_CREATE(NULL, NULL);
    SEE_UNIT_TEST_CREATE(array_create);
    SEE_UNIT_TEST_CREATE(array_create_capacity);
    SEE_UNIT_TEST_CREATE(array_add);
    SEE_UNIT_TEST_CREATE(array_set);
    SEE_UNIT_TEST_CREATE(array_capacity);

    return 0;
}
