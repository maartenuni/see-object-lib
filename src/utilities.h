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


#ifndef SEEUTILITIES_H
#define SEEUTILITIES_H

#include "see_export.h"
#include <stdint.h>
#include "Duration.h"
#include "TimePoint.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief reverses the order of the bytes in a 16 bit integer
 *
 * @param [in] val
 * @return val the byte order reversed.
 */
SEE_EXPORT uint16_t
see_swap_endianess16(uint16_t val);

/**
 * \brief reverses the order of the bytes in a 32 bit integer
 *
 * @param [in] val
 * @return val the byte order reversed.
 */
SEE_EXPORT uint32_t
see_swap_endianess32(uint32_t val);

/**
 * \brief reverses the order of the bytes in a 64 bit integer
 *
 * @param [in] val
 * @return val the byte order reversed.
 */
SEE_EXPORT uint64_t
see_swap_endianess64(uint64_t val);

/**
 * \brief if necessary, swap byteorder to match network byteorder
 *
 * On most platforms the byteorder is little endian, meaning the least
 * significant byte is stored first in an integer, contrasting with big
 * endian where the most significant byte is stored first.
 *
 * @param [in] val the integer that you want in network representation.
 * @return val in network representation.
 */
SEE_EXPORT uint16_t
see_host_to_network16(uint16_t val);

/**
 * \brief if necessary, swap byteorder to match network byteorder
 *
 * When you receive a uint16_t in network byte order you can use this function
 * to swap the bytes to the network order of the host.
 *
 * @param [in] val the integer that you want in network representation.
 * @return val in network representation.
 */
SEE_EXPORT uint16_t
see_network_to_host16(uint16_t val);

/**
 * \brief if necessary, swap byteorder to match network byteorder
 *
 * On most platforms the byteorder is little endian, meaning the least
 * significant byte is stored first in an integer, contrasting with big
 * endian where the most significant byte is stored first.
 *
 * @param [in] val the integer that you want in network representation.
 * @return val in network representation.
 */
SEE_EXPORT uint32_t
see_host_to_network32(uint32_t val);

/**
 * \brief if necessary, swap byteorder to match network byteorder
 *
 * When you receive a uint32_t in network byte order you can use this function
 * to swap the bytes to the network order of the host.
 *
 * @param [in] val the integer that you want in network representation.
 * @return val in network representation.
 */
SEE_EXPORT uint32_t
see_network_to_host32(uint32_t val);

/**
 * \brief if necessary, swap byteorder to match network byteorder.
 *
 * see the notes for see_host_to_network32()
 *
 * @param [in] val
 * @return val in network byte order.
 */
SEE_EXPORT uint64_t
see_host_to_network64(uint64_t val);

/**
 * \brief if necessary, swap byteorder to match network byteorder
 *
 * When you receive a uint64_t in network byte order you can use this function
 * to swap the bytes to the network order of the host.
 *
 * @param [in] val the integer that you want in network representation.
 * @return val in network representation.
 */
SEE_EXPORT uint64_t
see_network_to_host64(uint64_t val);

/**
 * \brief Sleep for a little while.
 *
 * This function makes suspends the current thread for a duration.
 * This function should sleep for at least the specified duration. The delay
 * might be affected by scheduling and other thread battling for resources.
 *
 * @param [in] dur The duration to suspend the current thread.
 *
 * @return SEE_SUCCESS, SEE_INVALID_ARGUMENT
 */
SEE_EXPORT int see_sleep(const SeeDuration* dur);


/**
 * \brief Sleep until a given timepoint.
 *
 * @param [in] tp A timepoint at which we should wake.
 *
 * @return SEE_SUCCESS, SEE_INVALID_ARGUMENT
 */
SEE_EXPORT int see_sleep_until(const SeeTimePoint* tp);


#ifdef __cplusplus
}
#endif

#endif //ifndef SEEUTILITIES_H
