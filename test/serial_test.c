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
#include "../src/Serial.h"
#include "../src/RuntimeError.h"

static const char* SUITE_NAME = "Serial Suite";

void serial_create(void)
{
    SeeSerial* serial   = NULL;
    SeeError*  error    = NULL;

    int ret = see_serial_new(&serial, &error);
    SEE_UNIT_HANDLE_ERROR()

    CU_ASSERT_EQUAL(ret, SEE_SUCCESS)
    CU_ASSERT_NOT_EQUAL(serial, NULL)
    CU_ASSERT_EQUAL(error, NULL)

fail:

    see_object_decref(SEE_OBJECT(serial));
    see_object_decref(SEE_OBJECT(error));
}

void serial_use_unopened(void)
{
    SeeSerial* serial   = NULL;
    SeeError*  error    = NULL;

    char buffer[] = "Hello serial world.";
    size_t bufsz = sizeof(buffer);

    int ret = see_serial_new(&serial, &error);
    SEE_UNIT_HANDLE_ERROR()

    // The unit should be closed after this call, so this should just work.
    ret = see_serial_close(serial, &error);
    SEE_UNIT_HANDLE_ERROR()
    CU_ASSERT_EQUAL(ret, SEE_SUCCESS);



    ret = see_serial_write(serial, buffer, &bufsz, &error);
    CU_ASSERT_EQUAL(ret, SEE_ERROR_RUNTIME);
    CU_ASSERT_EQUAL(
        see_runtime_error_class(),
        SEE_RUNTIME_ERROR_GET_CLASS(error)
        );
    if (ret) {
        const char* msg = see_error_msg(error);
        (void) msg;
        see_object_decref(SEE_OBJECT(error));
        error = NULL;
    }

    bufsz = sizeof(buffer);
    ret = see_serial_read(serial, buffer, &bufsz, &error);
    CU_ASSERT_EQUAL(ret, SEE_ERROR_RUNTIME);
    CU_ASSERT_EQUAL(
        see_runtime_error_class(),
        SEE_RUNTIME_ERROR_GET_CLASS(error)
        );
    if (ret) {
        const char* msg = see_error_msg(error);
        (void) msg;
        see_object_decref(SEE_OBJECT(error));
        error = NULL;
    }

fail:
    see_object_decref(SEE_OBJECT(serial));
    see_object_decref(SEE_OBJECT(error));
}

int add_serial_suite()
{
    SEE_UNIT_SUITE_CREATE(NULL, NULL);
    SEE_UNIT_TEST_CREATE(serial_create);
    SEE_UNIT_TEST_CREATE(serial_use_unopened);

    return 0;
}
