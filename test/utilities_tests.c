#include "test_macros.h"

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

#include <stdlib.h>
#include "test_macros.h"
#include "../src/utilities.h"
#include <see_object_config.h>
#include <stdint.h>

static const char* SUITE_NAME = "test_utilities";

void swap_endianness32(void)
{
    uint32_t input_order        = 0x12345678;
    uint32_t expected_result    = 0x78563412;

    uint32_t swapped = see_swap_endianess32(input_order);
    CU_ASSERT_EQUAL(swapped, expected_result);
}

void swap_endianness64(void)
{
    uint64_t input_order        = 0x0123456789abcdef;
    uint64_t expected_result    = 0xefcdab8967452301;

    uint64_t swapped = see_swap_endianess64(input_order);
    CU_ASSERT_EQUAL(swapped, expected_result);
}

void network_to_host32(void)
{
    int32_t result;
    int32_t network = 0xdeadbeef;
#if SEE_LITTLE_ENDIAN
    int32_t host    = 0xefbeadde;
#elif SEE_BIG_ENDIAN
    int32_t host    = 0xdeadbeef;
#else
#warning "unknown endianness";
#endif

    result = see_network_to_host32(network);
    CU_ASSERT_EQUAL(result, host);
}

void host_to_network32(void)
{
    int32_t result;
    int32_t host = 0xdeadbeef;
#if SEE_LITTLE_ENDIAN
    int32_t network = 0xefbeadde;
#elif SEE_BIG_ENDIAN
    int32_t network = 0xdeadbeef;
#else
#warning "unknown endianness";
#endif

    result = see_host_to_network32(host);
    CU_ASSERT_EQUAL(result, network);
}

void network_to_host64(void)
{
    int64_t result;
    int64_t network = 0x0123456789abcdef;
#if SEE_LITTLE_ENDIAN
    int64_t host    = 0xefcdab8967452301;
#elif SEE_BIG_ENDIAN
    int64_t host    = 0x0123456789abcdef;
#else
#warning "unknown endianness";
#endif

    result = see_network_to_host64(network);
    CU_ASSERT_EQUAL(result, host);
}

void host_to_network64(void)
{
    int64_t result;
    int64_t host    = 0x0123456789abcdef;
#if SEE_LITTLE_ENDIAN
    int64_t network = 0xefcdab8967452301;
#elif SEE_BIG_ENDIAN
    int64_t network = 0x0123456789abcdef;
#else
#warning "unknown endianness";
#endif

    result = see_host_to_network64(host);
    CU_ASSERT_EQUAL(result, network);
}

int add_utilities_suite() {

    SEE_UNIT_SUITE_CREATE(NULL, NULL);

    SEE_UNIT_TEST_CREATE(swap_endianness32);
    SEE_UNIT_TEST_CREATE(swap_endianness64);
    SEE_UNIT_TEST_CREATE(network_to_host32);
    SEE_UNIT_TEST_CREATE(host_to_network32);
    SEE_UNIT_TEST_CREATE(network_to_host64);
    SEE_UNIT_TEST_CREATE(host_to_network64);

    return 0;
}

