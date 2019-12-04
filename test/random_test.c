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
#include "../src/Error.h"
#include "../src/Random.h"


static const char* SUITE_NAME = "SeeRandom Suite";

static void
random_create(void)
{
    SeeError* error = NULL;
    SeeRandom* rgen = NULL;
    int ret;

    ret = see_random_new(&rgen, &error);
    SEE_UNIT_HANDLE_ERROR();

fail:
    SEE_OBJECT_DECREF(rgen);
    SEE_OBJECT_DECREF(error);
}


int
add_random_suite(void)
{
    SEE_UNIT_SUITE_CREATE(NULL, NULL);
    SEE_UNIT_TEST_CREATE(random_create);

    return 0;
}
