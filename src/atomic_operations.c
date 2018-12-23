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

#include "atomic_operations.h"

#if defined(_WIN32) || defined(WIN32)
#define WIN32_LEAN_AND_MEAN
#include <Windows.h> // for Interlocked Increment
#include <WinBase.h>

#endif

int see_atomic_increment(int* val)
{
    return see_atomic_increment_by(val, 1);
}

int see_atomic_decrement(int* val)
{
    return see_atomic_decrement_by(val, 1);
}

int see_atomic_increment_by(int* val, int n)
{
#if defined(SEE_HAVE_STDATOMIC_H)
    return sync_add_and_fetch(val, n);
#elif defined(__GNUC__)
    return __sync_add_and_fetch(val, n);
#elif defined(_WIN32) || defined(_WIN64)
	return InterlockedAdd(val, n);
#else
#error Unable to impletent atomic increment
#endif
}

int see_atomic_decrement_by(int* val, int n)
{
#if defined(SEE_HAVE_STDATOMIC_H)
    return sync_sub_and_fetch(val, n);
#elif defined(__GNUC__)
    return __sync_sub_and_fetch(val, n);
#elif defined(_WIN32) || defined(_WIN64)
	return InterlockedAdd(val, -n);
#else
#error Unable to impletent atomic decrement
#endif
}
