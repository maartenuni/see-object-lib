/*
 * This file is part of see-objects.
 *
 * see-objects is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * see-objects is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with see-objects.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdio.h>
#include <stdlib.h>
#include <CUnit/CUnit.h>
#include <CUnit/Basic.h>
#include <assert.h>

#include "suites.h"
#include "../src/see_init.h"
#include "../src/errors.h"

int add_suites()
{
    int res = add_see_object_suite();
    if (res)
        return res;

    res = add_meta_suite();
    if (res)
        return res;

    res = add_bst_suite();
    if (res)
        return res;

    res = add_dynamic_array_suite();
    if (res)
        return res;

    res = add_error_suite();
    if (res)
        return res;

    res = add_msg_buffer_suite();
    if (res)
        return res;

    res = add_random_suite();
    if (res)
        return res;

    res = add_serial_suite();
    if (res)
        return res;

    res = add_stack_suite();
    if (res)
        return res;
    
    res = add_time_suite();
    if (res)
        return res;
    
    res = add_utilities_suite();
    
    return res;
}

int main(int argc, char** argv) {
    
    (void) argc; (void) argv; // prevent compiler warnings.

    if (CU_initialize_registry() != CUE_SUCCESS)
        return EXIT_FAILURE;

    int ret = see_init();
    assert(ret == SEE_SUCCESS);
    if (ret) {
        fprintf(stderr, "Unable to init the see-objects library\n");
        return 1;
    }

    add_suites();

    if(CU_basic_run_tests() != CUE_SUCCESS) {
        fprintf(stderr, "Unable to run tests\n");
        return EXIT_FAILURE;
    }

    see_deinit();

    CU_cleanup_registry();

    return 0;
}
