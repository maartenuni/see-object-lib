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


#ifndef errors_H
#define errors_H

#ifdef __cplusplus
extern "C" {
#endif

enum see_error {
    SEE_SUCCESS = 0,         /**< Everything looks dandy! */
    SEE_INVALID_ARGUMENT,    /**< Inspect arguments given to function*/
    SEE_RUNTIME_ERROR,       /**< Inspect errno for clue.*/
};

#ifdef __cplusplus
}
#endif

#endif //ifndef errors_H
