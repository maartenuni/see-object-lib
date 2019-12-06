/*
 * This file is part of see-objects.
 *
 * see-objects is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * see-objects is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with see-objects.  If not, see <http://www.gnu.org/licenses/>.
 */


#ifndef suites_H
#define suites_H

#ifdef __cplusplus
extern "C" {
#endif

int add_see_object_suite();
int add_meta_suite();
int add_bst_suite();
int add_dynamic_array_suite();
int add_error_suite();
int add_msg_buffer_suite();
int add_random_suite();
int add_serial_suite();
int add_stack_suite();
int add_time_suite();
int add_utilities_suite();

#ifdef __cplusplus
}
#endif

#endif //ifndef suites_H
