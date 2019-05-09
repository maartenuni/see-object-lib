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
 * @file errors.h
 * \brief this file export the constants used when a function returns
 * (un-)successfully.
 * @author Maarten Duijndam
 */


#ifndef errors_H
#define errors_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \public
 * \brief This enumeration exports the errors that are returned when a
 * see_object_do_something(object, param1, ... paramn) is unsuccessful.
 */
enum see_error {
    SEE_SUCCESS = 0,           /**< Everything looks dandy! */
    SEE_ERROR_INTERNAL,        /**< Something inside the library is broken */
    SEE_INVALID_ARGUMENT,      /**< Inspect arguments given to function*/
    SEE_ERROR_RUNTIME,         /**< Inspect errno for clue.*/
    SEE_NOT_INITIALIZED,       /**< making use of an uninitialized class/function.*/
    SEE_ERROR_INDEX,           /**< Index is out of the valid range. */
    SEE_ERROR_MSG_PART_TYPE    /**< Incorrect usage of the msg type */
};

#ifdef __cplusplus
}
#endif

#endif //ifndef errors_H
