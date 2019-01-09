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

#include <stdio.h>
#include <CUnit/CUnit.h>

#include "../src/Error.h"

static const char* SUITE_NAME = "SeeErrorClass suite";

static void error_create(void)
{
    int ret;
    SeeError* error = NULL;
    ret = see_error_new(&error);
    CU_ASSERT_EQUAL(ret, SEE_SUCCESS);
    if (ret) {
        CU_ASSERT_EQUAL(error, NULL);
        return;
    }

    CU_ASSERT_STRING_EQUAL("No error", see_error_msg(error));
    see_object_decref(SEE_OBJECT(error));
}


static void error_create_msg(void)
{
    int ret;
    SeeError* error = NULL;
    const char* msg = "Hello, I'm an error!";

    ret = see_error_new_msg(&error, msg);
    CU_ASSERT_EQUAL(ret, SEE_SUCCESS);
    if (ret) {
        CU_ASSERT_EQUAL(error, NULL);
        return;
    }

    CU_ASSERT_STRING_EQUAL(msg, see_error_msg(error));
    see_object_decref(SEE_OBJECT(error));
}


static void error_set_msg(void)
{
    int ret;
    const char* msg = "Hello, World!";
    SeeError* error = NULL;

    ret = see_error_new(&error);
    if (ret)
        return;

    see_error_set_msg(error, msg);
    CU_ASSERT_STRING_EQUAL(msg, see_error_msg(error));
    see_object_decref(SEE_OBJECT(error));
}

static void error_expected_failures(void)
{
    int ret;
    SeeError* error = (SeeError*) (long) 0xdeadbeef;
    const char* msg = "blahh";

    ret = see_error_new(NULL);
    CU_ASSERT_EQUAL(ret, SEE_INVALID_ARGUMENT);

    ret = see_error_new(&error);
    CU_ASSERT_EQUAL(ret, SEE_INVALID_ARGUMENT);

    error = (SeeError*) (long) 0xdeadbeef;
    ret = see_error_new_msg(NULL, msg);
    CU_ASSERT_EQUAL(ret, SEE_INVALID_ARGUMENT);

    ret = see_error_new_msg(&error, msg);
    CU_ASSERT_EQUAL(ret, SEE_INVALID_ARGUMENT);

    error = NULL;
    ret = see_error_new_msg(&error, NULL);
    CU_ASSERT_EQUAL(ret, SEE_INVALID_ARGUMENT);
    CU_ASSERT_EQUAL(error, NULL)
}


int add_error_suite(void)
{
    CU_pSuite suite = CU_add_suite(SUITE_NAME, NULL, NULL);
    if (!suite) {
        fprintf(
            stderr, "Unable to create %s:%s\n",
            SUITE_NAME,
            CU_get_error_msg()
            );
        return CU_get_error();
    }

    CU_pTest test = CU_add_test(suite, "error_create", error_create);
    if (!test) {
        fprintf(stderr, "Unable to create test %s:%s:%s",
            SUITE_NAME,
            "create",
            CU_get_error_msg()
            );
        return CU_get_error();
    }

    test = CU_add_test(suite, "error_create_msg", error_create_msg);
    if (!test) {
        fprintf(stderr, "Unable to create test %s:%s:%s",
                SUITE_NAME,
                "error_create_msg",
                CU_get_error_msg()
        );
        return CU_get_error();
    }

    test = CU_add_test(suite, "error_set_msg", error_set_msg);
    if (!test) {
        fprintf(stderr, "Unable to set test %s:%s:%s",
                SUITE_NAME,
                "error_set_msg",
                CU_get_error_msg()
        );
        return CU_get_error();
    }

    test = CU_add_test(
        suite,
        "error_expected_failures",
        error_expected_failures
        );
    if (!test) {
        fprintf(stderr, "Unable to set test %s:%s:%s",
                SUITE_NAME,
                "error_expected_failures",
                CU_get_error_msg()
        );
        return CU_get_error();
    }

    return 0;
}

