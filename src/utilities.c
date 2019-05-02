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


#include "see_object_config.h"
#include "utilities.h"

#if defined(HAVE_ARPA_INET_H)
#include <arpa/inet.h>
#endif

uint32_t
see_swap_endianess32(uint32_t input)
{
    uint32_t out = 0;

    out = ((input & 0xff000000u) >> 24u) |
          ((input & 0x00ff0000u) >>  8u) |
          ((input & 0x0000ff00u) <<  8u) |
          ((input & 0x000000ffu) << 24u) ;

    return out;
}

uint64_t
see_swap_endianess64(uint64_t input)
{
    uint64_t out = 0;

    out = ((input & 0xff00000000000000ul) >> 56u) |
          ((input & 0x00ff000000000000ul) >> 40u) |
          ((input & 0x0000ff0000000000ul) >> 24u) |
          ((input & 0x000000ff00000000ul) >>  8u) |
          ((input & 0x00000000ff000000ul) <<  8u) |
          ((input & 0x0000000000ff0000ul) << 24u) |
          ((input & 0x000000000000ff00ul) << 40u) |
          ((input & 0x00000000000000fful) << 56u) ;

    return out;
}

uint32_t
see_host_to_network32(uint32_t host_val)
{
    uint32_t net_val;
#if SEE_BIG_ENDIAN
    net_val= hostval;
#else
    net_val = see_swap_endianess32(host_val);
#endif
    return net_val;
}

uint64_t
see_host_to_network64(uint64_t host_val)
{
    uint64_t net_val;
#if SEE_BIG_ENDIAN
    net_val= hostval;
#else
    net_val = see_swap_endianess64(host_val);
#endif
    return net_val;
}

uint32_t
see_network_to_host32(uint32_t host_val)
{
    uint32_t net_val;
#if SEE_BIG_ENDIAN
    net_val= hostval;
#else
    net_val = see_swap_endianess32(host_val);
#endif
    return net_val;
}

uint64_t
see_network_to_host64(uint64_t host_val)
{
    uint64_t net_val;
#if SEE_BIG_ENDIAN
    net_val= hostval;
#else
    net_val = see_swap_endianess64(host_val);
#endif
    return net_val;
}

