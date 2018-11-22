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

#include "suites.h"
#include "../src/init.h"

int add_suites()
{
    int res = add_see_object_suite();
    if (res)
        return res;

    res = add_meta_suite();
    if (res)
        return res;
    
    return res;
}

int main(int argc, char** argv) {
    
    (void) argc; (void) argv; // prevent compiler warnings.

    if (CU_initialize_registry() != CUE_SUCCESS)
        return EXIT_FAILURE;

    see_init();

    add_suites();

    if(CU_basic_run_tests() != CUE_SUCCESS) {
        fprintf(stderr, "Unable to run tests\n");
        return EXIT_FAILURE;
    }

    see_deinit();

    CU_cleanup_registry();

    return 0;


}
