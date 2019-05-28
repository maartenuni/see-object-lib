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
 * \file see_init.c the code that does the initialization of the library.
 *
 * \private
 */

#include "see_object_config.h"
#include "MetaClass.h"
#include "see_init.h"
#include "Clock.h"
#include "Duration.h"
#include "DynamicArray.h"
#include "Error.h"
#include "atomic_operations.h"
#include "IndexError.h"
#include "RuntimeError.h"
#include "TimePoint.h"
#include "Serial.h"
#include "MsgBuffer.h"
#include "TimeoutError.h"
#include "IncomparableError.h"

#if HAVE_WINDOWS_H
#include "windows/WindowsRuntimeError.h"
#elif HAVE_UNISTD_H
#include "posix/PosixSerial.h"

#endif

int g_init_count = 0;
int g_is_init = 0;

#if HAVE_WINDOWS_H
static int windows_init()
{
    int ret;
    ret = see_windows_runtime_error_init();
    if (ret)
        return ret;

    return ret;
}
#endif

#if HAVE_UNISTD_H
static int posix_init()
{
    int ret = SEE_SUCCESS;

    // posix_serial_is initialized from init_serial();
//
//    ret = see_posix_serial_init();
//    if (ret)
//        return ret;

    return ret;
}
#endif

static int
initialize() {
    // The meta class must be initialized first, otherwise the rest
    // will fail.
    int ret = see_meta_class_init();
    if (ret)
        return ret;

    // Initialize the other objects.
    ret = see_clock_init();
    if (ret)
        return ret;

    ret = see_duration_init();
    if (ret)
        return ret;

    ret = see_dynamic_array_init();
    if (ret)
        return ret;

    ret = see_error_init();
    if (ret)
        return ret;

    ret = see_incomparable_error_init();
    if (ret)
        return ret;

    ret = see_index_error_init();
    if (ret)
        return ret;

    ret = see_msg_buffer_init();
    if (ret)
        return ret;

    ret = see_msg_invalid_error_init();
    if (ret)
        return ret;

    ret = see_msg_part_init();
    if (ret)
        return ret;

    ret = see_msg_part_type_error_init();
    if (ret)
        return ret;

    ret = see_runtime_error_init();
    if (ret)
        return ret;

    ret = see_serial_init();
    if (ret)
        return ret;

    ret = see_timeout_error_init();
    if (ret)
        return ret;

    ret = see_time_point_init();
    if (ret)
        return ret;

#if HAVE_WINDOWS_H
    ret = windows_init();
    if (ret)
        return ret;
#elif HAVE_UNISTD_H
    ret = posix_init();
    if (ret)
        return ret;
#endif
    g_is_init = 1;

    return ret;
}

static void
deinit()
{
    see_clock_deinit();
    see_duration_deinit();
    see_dynamic_array_deinit();
    see_error_deinit();
    see_incomparable_error_deinit();
    see_index_error_deinit();
    see_msg_buffer_deinit();
    see_msg_invalid_error_deinit();
    see_msg_part_deinit();
    see_msg_part_type_error_deinit();
    see_runtime_error_deinit();
    see_serial_deinit();
    see_timeout_error_deinit();
    see_time_point_deinit();

    see_meta_class_deinit();

    g_is_init = 0;
}


int see_init()
{
    int ret = SEE_SUCCESS;

    int count = see_atomic_increment(&g_init_count);

    if (count == 1) {
        ret = initialize();
    }
    else {
        while (!g_is_init)
            ;
    }

    return ret;
}

void see_deinit()
{
    int count;
    count = see_atomic_decrement(&g_init_count);
    if (count == 0) {
        deinit();
    }
}
