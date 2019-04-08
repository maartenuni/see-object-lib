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

#include <chrono>
#include <utility>
#include "Duration.hpp"

class TimePoint {
    typedef std::chrono::time_point<
        std::chrono::steady_clock
        > tp;
public:

    TimePoint()
        : m_tp()
    {
    }

    TimePoint(const TimePoint& other)
        : m_tp(other.m_tp)
    {
    }

    TimePoint(TimePoint&& other)
        : m_tp(std::move(other.m_tp))
    {
    }

    TimePoint(const tp& timep)
        : m_tp(timep)
    {
    }

    TimePoint(tp&& timep) noexcept
        : m_tp(timep)
    {
    }

    TimePoint& operator= (const TimePoint& rhs) noexcept
    {
        m_tp = rhs.m_tp;
        return *this;
    }

    TimePoint& operator=(const TimePoint&& rhs) noexcept
    {
        m_tp = std::move(rhs.m_tp);
        return *this;
    }

    TimePoint operator-(const Duration& dur) const noexcept
    {
        return TimePoint(this->m_tp - dur.m_dur);
    }

    Duration operator-(const TimePoint& other) const noexcept
    {
        return Duration(this->m_tp - other.m_tp);
    }

private:
     tp m_tp;
};