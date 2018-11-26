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


#include <string.h>
#include "SeeObject.h"
#include "see_functions.h"

void* see_copy_by_ref(void* dest_seeobj, const void* src, size_t unused)
{
    memcpy(dest_seeobj, src, sizeof(SeeObject*));
    see_object_ref(dest_seeobj);
    return dest_seeobj;
}