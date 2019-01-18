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
 * \file see_init.h
 * \brief Initialize the seeobject library.
 *
 *
 * This header contains the two functions that are needed to intialize
 * and un-/deintialize the seeobject library. One can initialize all
 * other classes/instance from their respectived headers, but these
 * function enable the entire library.
 */

#ifndef init_H
#define init_H

#include "see_export.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief Initialize seeobject library.
 *
 * This function enables all classes that are supported in the current version
 * of the library. If you are really tight on memory, you could skip this
 * functions. Intialize the metaclass by calling see_meta_init(), and
 * then only initialize the classes that you need.
 * For example when you only need SeeDynamicArray, you can intialize the meta-
 * class, and then call see_dynamic_array_init() It might be quite
 * possible that a class depends on other classes being initialized...
 *
 * @return SEE_SUCCES or another value indicating an error.
 */
SEE_EXPORT int see_init();

/**
 * \brief Free the resources used by the seeobject library.
 *
 * After you are done with see_objects, you can deinitialize the library.
 */
SEE_EXPORT void see_deinit();

#ifdef __cplusplus
}
#endif

#endif //ifndef init_H
