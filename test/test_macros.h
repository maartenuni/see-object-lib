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

#ifndef SEE_TEST_MACRO_S_H
#define SEE_TEST_MACRO_S_H

#include <stdio.h>
#include <CUnit/CUnit.h>

/**
 * \brief initialize the test suite.
 *
 * This macro assumes you have a static const char* called SUITE_NAME in the
 * same compilation unit that contains a descriptive name for the suite.
 *
 * It declares and defines a CU_pSuite called suite and registers the suite
 * with the CU_UNIT registry. Additionally it declares a CU_pTest called test.
 *
 * Further will any errors be printed to stderr and a error will be returned.
 * That can inform about what went wrong.
 *
 * @param init_func a function that sets up the resources for the suite.
 * @param finalize_func
 *
 * \private
 */
#define SEE_UNIT_SUITE_CREATE(init_func, finalize_func)                     \
    CU_pTest test;                                                          \
    CU_pSuite suite = CU_add_suite(SUITE_NAME, init_func, finalize_func);   \
    if (!suite) {                                                           \
        fprintf(stderr, "Unable to create suite %s:%s.\n",                  \
            SUITE_NAME,                                                     \
            CU_get_error_msg()                                              \
            );                                                              \
        return CU_get_error();                                              \
    }

/**
 * \brief add a test to the current test suite as created by the
 *        SEE_UNIT_SUITE_CREATE macro.
 * \param test_name, the test name must be the name of a function that does
 *        runs the actual test.
 */
#define SEE_UNIT_TEST_CREATE(test_name)                                     \
    test = CU_add_test(suite, #test_name, test_name);                       \
    if (!test){                                                             \
        fprintf(stderr, "Unable to create test %s:%s:%s",                   \
            SUITE_NAME,                                                     \
            #test_name,                                                     \
            CU_get_error_msg()                                              \
            );                                                              \
        return CU_get_error();                                              \
    }


#endif
