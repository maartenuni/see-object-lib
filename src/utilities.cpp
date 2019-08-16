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
 * \file utilities.cpp
 * \brief implement some useful utilities that require C++
 * @private
 */

#include <thread>

#include "utilities.h"
#include "cpp/Duration.hpp"
#include "cpp/TimePoint.hpp"
#include "TimePoint.h"


int see_sleep(const SeeDuration* dur)
{
    int64_t nanos = see_duration_nanos(dur);
    std::this_thread::sleep_for(Duration::ns(nanos));
    return SEE_SUCCESS;
}

int see_sleep_until(const SeeTimePoint* tp)
{
    const TimePoint* priv_time = static_cast<const TimePoint*>(tp->priv_time);
    std::this_thread::sleep_until(priv_time->get_time());
    return SEE_SUCCESS;
}

void see_yield()
{
    std::this_thread::yield();
}

