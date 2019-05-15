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
#include "../src/MsgBuffer.h"
#include "test_macros.h"

static const char* SUITE_NAME = "SeeMsgBuffer suite";

static void
msg_buffer_create(void)
{
    SeeError*       error   = NULL;
    SeeMsgBuffer*   buffer  = NULL;
    uint16_t id = 0;
    int ret;

    ret = see_msg_buffer_new(&buffer, id, &error);
    SEE_UNIT_HANDLE_ERROR();

    fail:
    see_object_decref(SEE_OBJECT(error));
    see_object_decref(SEE_OBJECT(buffer));
}

static void
msg_part_create(void)
{
    SeeError*   error = NULL;
    SeeMsgPart* part  = NULL;
    int ret;

    ret = see_msg_part_new(&part, &error);
    SEE_UNIT_HANDLE_ERROR();

    fail:
    see_object_decref(SEE_OBJECT(error));
    see_object_decref(SEE_OBJECT(part));
}

static void
msg_part_get_set_equal(void)
{
    SeeError*   error = NULL;
    SeeMsgPart* part  = NULL;
    int32_t in32 = 10245, out32;
    uint32_t generates_error;
    int ret;

    ret = see_msg_part_new(&part, &error);
    SEE_UNIT_HANDLE_ERROR();

    ret = see_msg_part_write_int32(part, in32, &error);
    SEE_UNIT_HANDLE_ERROR();

    ret = see_msg_part_get_int32(part, &out32, &error);
    SEE_UNIT_HANDLE_ERROR();

    ret = see_msg_part_get_uint32(part, &generates_error, &error);
    CU_ASSERT_EQUAL(ret, SEE_ERROR_MSG_PART_TYPE);

    CU_ASSERT_EQUAL(in32, out32);

fail:
    see_object_decref(SEE_OBJECT(error));
    see_object_decref(SEE_OBJECT(part));
}

static void
msg_part_get_set_string(void)
{
    SeeError*   error   = NULL;
    SeeMsgPart* part    = NULL;
    const char* input   = "Hello World";
    char* output        = NULL;
    int ret;

    ret = see_msg_part_new(&part, &error);
    SEE_UNIT_HANDLE_ERROR();

    ret = see_msg_part_write_string(part, input, strlen(input), &error);
    SEE_UNIT_HANDLE_ERROR();

    ret = see_msg_part_write_string(part, input, strlen(input), &error);
    SEE_UNIT_HANDLE_ERROR();

    ret = see_msg_part_get_string(part, &output, &error);
    SEE_UNIT_HANDLE_ERROR();

    CU_ASSERT_STRING_EQUAL(input, output);

fail:
    see_object_decref(SEE_OBJECT(error));
    see_object_decref(SEE_OBJECT(part));
    free(output);
}

static void
msg_part_get_set_float(void)
{
    SeeError*   error   = NULL;
    SeeMsgPart* part    = NULL;
    float       input   = M_E;
    float       output;
    int ret;

    ret = see_msg_part_new(&part, &error);
    SEE_UNIT_HANDLE_ERROR();

    ret = see_msg_part_write_float(part, input, &error);
    SEE_UNIT_HANDLE_ERROR();

    ret = see_msg_part_get_float(part, &output, &error);
    SEE_UNIT_HANDLE_ERROR();

    CU_ASSERT_EQUAL(input, output);

fail:
    see_object_decref(SEE_OBJECT(error));
    see_object_decref(SEE_OBJECT(part));
}

static void
msg_part_get_set_double(void)
{
    SeeError*   error   = NULL;
    SeeMsgPart* part    = NULL;
    double      input   = M_PI;
    double      output;
    int ret;

    ret = see_msg_part_new(&part, &error);
    SEE_UNIT_HANDLE_ERROR();

    ret = see_msg_part_write_double(part, input, &error);
    SEE_UNIT_HANDLE_ERROR();

    ret = see_msg_part_get_double(part, &output, &error);
    SEE_UNIT_HANDLE_ERROR();

    CU_ASSERT_EQUAL(input, output);

fail:
    see_object_decref(SEE_OBJECT(error));
    see_object_decref(SEE_OBJECT(part));
}

static void
msg_part_buffer_numeric(void)
{
    SeeMsgPart* part_uint32 = NULL;
    SeeMsgPart* part_uint64 = NULL;
    SeeMsgPart* part_int32  = NULL;
    SeeMsgPart* part_int64  = NULL;
    SeeMsgPart* part_double = NULL;
    SeeMsgPart* part_float= NULL;
    SeeError*   error       = NULL;

    uint32_t    data_length = 0;
    uint32_t    buf_length  = 0;
    int ret;

    ret = see_msg_part_new(&part_int32, &error);
    SEE_UNIT_HANDLE_ERROR();

    ret = see_msg_part_write_int32(part_int32, 12345, &error);
    SEE_UNIT_HANDLE_ERROR();

    ret = see_msg_part_length(part_int32, &data_length, &error);
    SEE_UNIT_HANDLE_ERROR();
    ret = see_msg_part_buffer_length(part_int32, &buf_length, &error);
    SEE_UNIT_HANDLE_ERROR();

    CU_ASSERT_EQUAL(data_length, sizeof(part_int32->value.uint32_val));
    CU_ASSERT_EQUAL(buf_length,
        sizeof(part_int32->value.int32_val) +
        sizeof(part_int32->value_type)
        );

    ret = see_msg_part_new(&part_int64, &error);
    SEE_UNIT_HANDLE_ERROR();

    ret = see_msg_part_write_int64(part_int64, 12345, &error);
    SEE_UNIT_HANDLE_ERROR();

    ret = see_msg_part_length(part_int64, &data_length, &error);
    SEE_UNIT_HANDLE_ERROR();
    ret = see_msg_part_buffer_length(part_int64, &buf_length, &error);
    SEE_UNIT_HANDLE_ERROR();

    CU_ASSERT_EQUAL(data_length, sizeof(part_int64->value.int64_val));
    CU_ASSERT_EQUAL(buf_length,
                    sizeof(part_int64->value.int64_val) +
                    sizeof(part_int64->value_type)
    );

    ret = see_msg_part_new(&part_uint32, &error);
    SEE_UNIT_HANDLE_ERROR();

    ret = see_msg_part_write_uint32(part_uint32, 12345, &error);
    SEE_UNIT_HANDLE_ERROR();

    ret = see_msg_part_length(part_uint32, &data_length, &error);
    SEE_UNIT_HANDLE_ERROR();
    ret = see_msg_part_buffer_length(part_uint32, &buf_length, &error);
    SEE_UNIT_HANDLE_ERROR();

    CU_ASSERT_EQUAL(data_length, sizeof(part_uint32->value.uint32_val));
    CU_ASSERT_EQUAL(buf_length,
                    sizeof(part_uint32->value.uint32_val) +
                    sizeof(part_uint32->value_type)
    );

    ret = see_msg_part_new(&part_uint64, &error);
    SEE_UNIT_HANDLE_ERROR();

    ret = see_msg_part_write_uint64(part_uint64, 12345, &error);
    SEE_UNIT_HANDLE_ERROR();

    ret = see_msg_part_length(part_uint64, &data_length, &error);
    SEE_UNIT_HANDLE_ERROR();
    ret = see_msg_part_buffer_length(part_uint64, &buf_length, &error);
    SEE_UNIT_HANDLE_ERROR();

    CU_ASSERT_EQUAL(data_length, sizeof(part_uint64->value.uint64_val));
    CU_ASSERT_EQUAL(buf_length,
                    sizeof(part_uint64->value.uint64_val) +
                    sizeof(part_uint64->value_type)
    );

    ret = see_msg_part_new(&part_double, &error);
    SEE_UNIT_HANDLE_ERROR();

    ret = see_msg_part_write_double(part_double, 12345, &error);
    SEE_UNIT_HANDLE_ERROR();

    ret = see_msg_part_length(part_double, &data_length, &error);
    SEE_UNIT_HANDLE_ERROR();
    ret = see_msg_part_buffer_length(part_double, &buf_length, &error);
    SEE_UNIT_HANDLE_ERROR();

    CU_ASSERT_EQUAL(data_length, sizeof(part_double->value.double_val));
    CU_ASSERT_EQUAL(buf_length,
                    sizeof(part_double->value.double_val) +
                    sizeof(part_double->value_type)
    );

    ret = see_msg_part_new(&part_float, &error);
    SEE_UNIT_HANDLE_ERROR();

    ret = see_msg_part_write_float(part_float, 12345, &error);
    SEE_UNIT_HANDLE_ERROR();

    ret = see_msg_part_length(part_float, &data_length, &error);
    SEE_UNIT_HANDLE_ERROR();
    ret = see_msg_part_buffer_length(part_float, &buf_length, &error);
    SEE_UNIT_HANDLE_ERROR();

    CU_ASSERT_EQUAL(data_length, sizeof(part_float->value.float_val));
    CU_ASSERT_EQUAL(buf_length,
                    sizeof(part_float->value.float_val) +
                    sizeof(part_float->value_type)
    );

fail:

    see_object_decref(SEE_OBJECT(part_int32));
    see_object_decref(SEE_OBJECT(part_int64));
    see_object_decref(SEE_OBJECT(part_uint32));
    see_object_decref(SEE_OBJECT(part_uint64));
    see_object_decref(SEE_OBJECT(part_float));
    see_object_decref(SEE_OBJECT(part_double));
    see_object_decref(SEE_OBJECT(error));
}

static void
msg_part_buffer_string(void)
{
    SeeError* error   = NULL;
    SeeMsgPart* part  = NULL;
    const char* hello = "Hello, World!";
    uint32_t data_length, buffer_length;

    size_t len = strlen(hello);

    int ret = see_msg_part_new(&part, &error);
    SEE_UNIT_HANDLE_ERROR();

    ret = see_msg_part_write_string(part, hello, len, &error);
    SEE_UNIT_HANDLE_ERROR();

    ret = see_msg_part_length(part, &data_length, &error);
    SEE_UNIT_HANDLE_ERROR();

    ret = see_msg_part_buffer_length(part, &buffer_length, &error);
    SEE_UNIT_HANDLE_ERROR();

    CU_ASSERT_EQUAL(buffer_length,
        sizeof(part->value_type) + sizeof(part->length) + len);

    CU_ASSERT_EQUAL(data_length, len);

fail:

    see_object_decref(SEE_OBJECT(error));
    see_object_decref(SEE_OBJECT(part));

}

int add_msg_buffer_suite()
{
    SEE_UNIT_SUITE_CREATE(NULL, NULL);
    SEE_UNIT_TEST_CREATE(msg_buffer_create);
    SEE_UNIT_TEST_CREATE(msg_part_create);
    SEE_UNIT_TEST_CREATE(msg_part_get_set_equal);
    SEE_UNIT_TEST_CREATE(msg_part_get_set_string);
    SEE_UNIT_TEST_CREATE(msg_part_get_set_float);
    SEE_UNIT_TEST_CREATE(msg_part_get_set_double);
    SEE_UNIT_TEST_CREATE(msg_part_buffer_numeric);
    SEE_UNIT_TEST_CREATE(msg_part_buffer_string);

    return 0;
}