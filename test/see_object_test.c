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
#include <assert.h>

#include "../src/SeeObject.h"
#include "../src/TimePoint.h"

static const char* SUITE_NAME = "SeeObject suite";

static void create_see_object(void)
{
    SeeObject* obj = NULL;
    obj = see_object_create();
    CU_ASSERT_PTR_NOT_NULL(obj);
    see_object_decref(obj);
}

static void repr(void)
{
    SeeObject* obj = NULL;
    obj = see_object_create();
	const char* expected = "Instance of SeeObject at %p";
    char buffer[BUFSIZ] = {0};
	char* repr = NULL;

	snprintf(buffer, BUFSIZ, expected, obj);
    int n = see_object_repr(obj, &repr);

	CU_ASSERT_EQUAL(n, SEE_SUCCESS);
	if (n != SEE_SUCCESS) {
		assert(repr == NULL);
		return;
	}
	
    CU_ASSERT_STRING_EQUAL(repr, buffer);
	free(repr);
    see_object_decref(obj);
}

static void instance_of(void)
{
    SeeObject* obj = NULL;
    SeeError* error = NULL;
    const SeeObjectClass* see_obj_cls = see_object_class();
    const SeeObjectClass* see_dur_cls = SEE_OBJECT_CLASS(see_duration_class());
    const SeeObjectClass* see_tp_cls = SEE_OBJECT_CLASS(see_time_point_class());
    SeeDuration* dur    = NULL;
    SeeTimePoint* tp    = NULL;
    int instance_of, ret;

    ret = see_duration_new(&dur, &error);
    CU_ASSERT_EQUAL(ret, SEE_SUCCESS);
    if (ret != SEE_SUCCESS)
        goto fail;

    ret = see_time_point_new(&tp, &error);
    CU_ASSERT_EQUAL(ret, SEE_SUCCESS);
    if (ret != SEE_SUCCESS)
        goto fail;

    obj = see_object_create();
    CU_ASSERT_NOT_EQUAL(obj, NULL);
    if (!obj)
        goto fail;

    ret = see_object_is_instance_of(obj, see_obj_cls, &instance_of);
    CU_ASSERT_EQUAL(ret, SEE_SUCCESS);
    CU_ASSERT_EQUAL(instance_of, 1);
    if (ret  != SEE_SUCCESS)
        goto fail;

    ret = see_object_is_instance_of(
        SEE_OBJECT(dur), see_obj_cls, &instance_of
        );
    CU_ASSERT_EQUAL(ret, SEE_SUCCESS);
    CU_ASSERT_EQUAL(instance_of, 1);
    if (ret != SEE_SUCCESS)
        goto fail;

    ret = see_object_is_instance_of(
        SEE_OBJECT(tp), see_obj_cls, &instance_of
    );
    CU_ASSERT_EQUAL(ret, SEE_SUCCESS);
    CU_ASSERT_EQUAL(instance_of, 1);
    if (ret != SEE_SUCCESS)
        goto fail;

    ret = see_object_is_instance_of(
        SEE_OBJECT(tp), see_dur_cls, &instance_of
    );
    CU_ASSERT_EQUAL(ret, SEE_SUCCESS);
    CU_ASSERT_EQUAL(instance_of, 0);
    if (ret != SEE_SUCCESS)
        goto fail;

    ret = see_object_is_instance_of(
        SEE_OBJECT(dur), see_tp_cls, &instance_of
    );
    CU_ASSERT_EQUAL(ret, SEE_SUCCESS);
    CU_ASSERT_EQUAL(instance_of, 0);
    if (ret != SEE_SUCCESS)
        goto fail;

    ret = see_object_is_instance_of(
        SEE_OBJECT(dur), see_dur_cls, &instance_of
    );
    CU_ASSERT_EQUAL(ret, SEE_SUCCESS);
    CU_ASSERT_EQUAL(instance_of, 1);
    if (ret != SEE_SUCCESS)
        goto fail;

    ret = see_object_is_instance_of(
        SEE_OBJECT(tp), see_tp_cls, &instance_of
    );
    CU_ASSERT_EQUAL(ret, SEE_SUCCESS);
    CU_ASSERT_EQUAL(instance_of, 1);
    if (ret != SEE_SUCCESS)
        goto fail;

    ret = see_object_is_instance_of(
        NULL, see_obj_cls, &instance_of
    );
    CU_ASSERT_EQUAL(ret, SEE_SUCCESS);
    CU_ASSERT_EQUAL(instance_of, 0);
    if (ret != SEE_SUCCESS)
        goto fail;

fail:
    SEE_OBJECT_DECREF(obj);
    SEE_OBJECT_DECREF(dur);
    SEE_OBJECT_DECREF(tp);

}


int add_see_object_suite(void)
{
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

    test = CU_add_test(suite, "instance_of", instance_of);
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
