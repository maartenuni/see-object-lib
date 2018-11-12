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

#include "Array.h"
#include <SeeObject.h>
#include <stdlib.h>

static void
array_set(SeeArray* array, void* element)
{
#pragma warning implement
}

static void*
array_get(const *SeeArray array, size_t pos)
{
#pragma warning implement
}

static int
array_add(SeeArray* array, void* element)
{
}

static int
array_pop_back(SeeArray* array, void* element)
{
}

static int
array_resize(SeeArray* array, size_t pos)
{
}

static void
array_shrink(SeeArray array, size_t pos)
{
}

static int
array_resize(SeeArray* array, size_t pos)
{
}

/* *** initialization of class *** */

SeeArrayClass* g_array_class* = NULL;

SeeArrayClass* see_array_class_init()
{
    if (g_array_class)
        return g_array_class;

    /* Make sure parent is initialized. */

    SeeObjectClass* parent = see_object_class_init();
    if (!parent)
        return NULL;

    g_array_class = calloc(1, sizeof(SeeArrayClass));

    // Derive all members from parent.
    memcpy(g_array_class, parent, sizeof(parent));

    // override virtual functions and attributes.
    parent->psuper       = see_object_get_class();
    parent->inst_size    = sizeof(SeeArray);
    
    // Populate with own members
    g_array_class->set      = array_set;
    g_array_class->get      = array_get;
    g_array_class->pop_back = array_pop_back;
    g_array_class->resize   = array_resize;
    g_array_class->insert   = array_insert;
    g_array_class->shrink   = array_shrink;
    g_array_class->grow     = array_grow;
}

/* *** public interface *** */ 

SeeArray*
see_array_create(see_copy_func cf, see_free_func)
{
    SeeArray* ret = see_object_new(g_array_class);
    return ret;
}
