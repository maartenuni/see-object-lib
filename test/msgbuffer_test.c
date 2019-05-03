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
    int ret;

    ret = see_msg_buffer_new(&buffer, &error);
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

int add_msg_buffer_suite()
{
    SEE_UNIT_SUITE_CREATE(NULL,NULL);
    SEE_UNIT_TEST_CREATE(msg_buffer_create);
    SEE_UNIT_TEST_CREATE(msg_part_create);

    return 0;
}