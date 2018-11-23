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
#include <CUnit/CUnit.h>

#include "../src/SeeObject.h"

static const char* SUITE_NAME = "SeeObject suite";

static void create_see_object()
{
    SeeObject* obj = NULL;
    obj = see_object_create();
    CU_ASSERT_PTR_NOT_NULL(obj);
    see_object_decref(obj);
}

static void repr()
{
    SeeObject* obj = NULL;
    obj = see_object_create();
    char bufcmp[BUFSIZ] = {0};
    char buffer[BUFSIZ] = {0};
    snprintf(bufcmp, BUFSIZ, "See object at %p", (void*) obj);
    int n = see_object_repr(obj, buffer, BUFSIZ);
    CU_ASSERT_STRING_EQUAL(bufcmp, buffer);
    CU_ASSERT_EQUAL(n, (int)strlen(bufcmp));
    see_object_decref(obj);
}


int add_see_object_suite() {

    CU_pSuite suite = CU_add_suite(SUITE_NAME, NULL, NULL);
    if (!suite) {
        fprintf(stderr,
                "Unable to create %s:%s\n ",
                SUITE_NAME,
                CU_get_error_msg()
                );
        return CU_get_error();
    }

    CU_pTest test = CU_add_test(suite, "create", create_see_object);
    if (!test) {
        fprintf(stderr,
                "Unable to create test %s:%s\n ",
                SUITE_NAME,
                CU_get_error_msg()
                );
        return CU_get_error();
    }
    
    test = CU_add_test(suite, "repr", repr);
    if (!test) {
        fprintf(stderr,
                "Unable to create test %s:%s\n ",
                SUITE_NAME,
                CU_get_error_msg()
                );
        return CU_get_error();
    }

    return CU_get_error();
}
