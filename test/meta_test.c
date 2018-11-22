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
#include "../src/MetaClass.h"

static const char* SUITE_NAME = "SeeMetaClass suite";

typedef struct _SeeCustomRepr SeeCustomRepr;
typedef struct _SeeCustomReprClass SeeCustomReprClass;

struct _SeeCustomRepr {
    SeeObject   obj;
    const char* repr;
};

typedef void(*set_repr_func)(SeeCustomRepr* obj, const char* repr);

typedef struct _SeeCustomReprClass {
    SeeObjectClass  parent;
    set_repr_func   set_repr;
}SeeCustomReprClass;

SeeCustomReprClass* g_custom_class_instance = NULL;

static int custom_repr(const SeeObject* obj, char* out, size_t size)
{
    SeeCustomRepr* r = (SeeCustomRepr*) obj;
    return snprintf(out, size, "%s", r->repr);
}

static void
set_repr(SeeCustomRepr* obj, const char* repr)
{
    obj->repr = repr;
}

static int post_init_class(SeeObjectClass* cls)
{
    cls->repr = custom_repr;
    SeeCustomReprClass* custom_cls = (SeeCustomReprClass*) cls;
    custom_cls->set_repr = set_repr;
    return SEE_SUCCESS;
}

void set_custom_repr_set_repr(SeeCustomRepr* obj, const char* repr)
{
    const SeeCustomReprClass* cls =
        (const SeeCustomReprClass*)see_object_get_class((SeeObject*) obj);

    cls->set_repr(obj, repr);
}

static void meta_create()
{
    int ret = see_meta_class_new_class(
        see_meta_class_class(),
        (SeeObjectClass**) &g_custom_class_instance,
        sizeof(SeeCustomReprClass),
        sizeof(SeeCustomRepr),
        see_object_class(),
        sizeof(SeeObjectClass),
        post_init_class
        );
    CU_ASSERT_EQUAL(ret, SEE_SUCCESS);
    CU_ASSERT_NOT_EQUAL(g_custom_class_instance, NULL);

    CU_ASSERT_EQUAL(g_custom_class_instance->set_repr, &custom_repr);

}

static void meta_use()
{

}

static void meta_destroy()
{

}

int add_meta_suite()
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

    CU_pTest test = CU_add_test(suite, "create", meta_create);
    if (!test) {
        fprintf(stderr, "Unable to create test %s:%s:%s",
            SUITE_NAME,
            "create",
            CU_get_error_msg()
            );
        return CU_get_error();
    }

    test = CU_add_test(suite, "use", meta_use);
    if (!test) {
        fprintf(stderr, "Unable to create test %s:%s:%s",
            SUITE_NAME,
            "use",
            CU_get_error_msg()
            );
        return CU_get_error();
    }

    test = CU_add_test(suite, "use", meta_destroy);
    if (!test) {
        fprintf(stderr, "Unable to create test %s:%s:%s",
            SUITE_NAME,
            "destroy",
            CU_get_error_msg()
            );
        return CU_get_error();
    }

    return 0;
}
