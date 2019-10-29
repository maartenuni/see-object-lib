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

/**
 * \file see_functions.c implementations of some standard functions.
 *
 *
 */

#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include "SeeObject.h"
#include "see_functions.h"
#include "Error.h"

void* see_copy_by_ref(void* dest_seeobj, const void* src, size_t unused)
{
    (void) unused;
    memcpy(dest_seeobj, src, sizeof(SeeObject*));
    see_object_ref(* ((SeeObject**) dest_seeobj));
    return dest_seeobj;
}

void* see_copy_by_value(void* dest_seeobj, const void* src_seeobj, size_t unused)
{
    (void) unused;
    SeeObject*  out     = NULL;
    SeeError*   error   = NULL;
    const SeeObject* const* src_obj = src_seeobj;
    SeeObject** out_obj = dest_seeobj;

    int ret = see_object_copy(*src_obj, &out, &error);
    if (ret) {
        fprintf(stderr, "Oops: %s", see_error_msg(error));
        see_object_decref(SEE_OBJECT(error));
        return NULL;
    }
    *out_obj = out;
    return out;
}

int
see_init_memset(void* obj, size_t nbytes, void* byte)
{
    int byte_value = 0;
    if (byte)
        byte_value = *((uint8_t*) byte);
    memset(obj, byte_value, nbytes);
    return SEE_SUCCESS;
}

void see_free_see_object(void* object)
{
    see_object_decref(object);
}

