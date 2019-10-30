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
#include "../src/Error.h"
#include "../src/Stack.h"
#include "../src/IndexError.h"

#define ARRAY_SIZE 1024

static const char* SUITE_NAME = "SeeStack Suite";

void
stack_create(void)
{
    SeeError* error = NULL;
    SeeStack* stack = NULL;
    int ret;

    ret = see_stack_new(&stack, sizeof(int), NULL, NULL, NULL, &error);
    SEE_UNIT_HANDLE_ERROR();
    CU_ASSERT_EQUAL(ret, SEE_SUCCESS);

fail:
    see_object_decref(SEE_OBJECT(error));
    see_object_decref(SEE_OBJECT(stack));
}

void
stack_push_pop(void)
{
    SeeError* error = NULL;
    SeeStack* stack = NULL;
    int ret;
    int* array_in = calloc(ARRAY_SIZE, sizeof(int));
    int* array_out = calloc(ARRAY_SIZE, sizeof(int));
    int* array_reversed = calloc(ARRAY_SIZE, sizeof(int));

    for (size_t i = 0; i < ARRAY_SIZE; ++i) {
        array_in[i] = i;
        array_reversed[i] = ARRAY_SIZE - (i + 1);
    }

    ret = see_stack_new(&stack, sizeof(int), NULL, NULL, NULL, &error);
    SEE_UNIT_HANDLE_ERROR();

    for (size_t i = 0; i < ARRAY_SIZE; i++) {
        ret = see_stack_push(stack, &array_in[i], &error);
        if (ret) {
            // only encounter this error when something went wrong.
            SEE_UNIT_HANDLE_ERROR();
        }
    }
    SEE_UNIT_HANDLE_ERROR();

    for (size_t i = 0; i < ARRAY_SIZE; i++) {
        ret = see_stack_top(stack, &array_out[i], &error);
        if (ret) {
            // only encounter this error when something went wrong.
            SEE_UNIT_HANDLE_ERROR();
        }
        ret = see_stack_pop(stack, &error);
        if (ret) {
            // only encounter this error when something went wrong.
            SEE_UNIT_HANDLE_ERROR();
        }
    }
    SEE_UNIT_HANDLE_ERROR();
    CU_ASSERT_EQUAL(ret, SEE_SUCCESS);

    ret = memcmp(array_out, array_reversed, sizeof(int) * ARRAY_SIZE);
    CU_ASSERT_EQUAL(ret, 0);

    fail:
    see_object_decref(SEE_OBJECT(error));
    see_object_decref(SEE_OBJECT(stack));
    free(array_in);
    free(array_out);
    free(array_reversed);
}

void
stack_error(void)
{
    SeeError* error = NULL;
    SeeStack* stack = NULL;
    int ret;
    int isinstance = 0;
    int val = 9;

    ret = see_stack_new(&stack, sizeof(int), NULL, NULL, NULL, &error);
    SEE_UNIT_HANDLE_ERROR();

    ret = see_stack_top(stack, &val, &error);
    CU_ASSERT_EQUAL(ret, SEE_ERROR_INDEX);
    CU_ASSERT_NOT_EQUAL(error, NULL);
    see_object_is_instance_of(
        SEE_OBJECT(error),
        SEE_OBJECT_CLASS(see_index_error_class()),
        &isinstance
    );
    CU_ASSERT_NOT_EQUAL(isinstance, 0);
    CU_ASSERT_EQUAL(val, 9);

    //cleanup top
    see_object_decref(SEE_OBJECT(error));
    error = NULL;
    isinstance = 0;

    ret = see_stack_pop(stack, &error);
    CU_ASSERT_EQUAL(ret, SEE_ERROR_INDEX);
    CU_ASSERT_NOT_EQUAL(error, NULL);
    see_object_is_instance_of(
        SEE_OBJECT(error),
        SEE_OBJECT_CLASS(see_index_error_class()),
        &isinstance
    );
    CU_ASSERT_NOT_EQUAL(isinstance, 0);

fail:
    see_object_decref(SEE_OBJECT(error));
    see_object_decref(SEE_OBJECT(stack));
}

int
add_stack_suite(void)
{
    SEE_UNIT_SUITE_CREATE(NULL, NULL);
    SEE_UNIT_TEST_CREATE(stack_create);
    SEE_UNIT_TEST_CREATE(stack_push_pop);
    SEE_UNIT_TEST_CREATE(stack_error);

    return 0;
}

