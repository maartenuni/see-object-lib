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

#ifndef SEE_DURATION_HPP
#define SEE_DURATION_HPP

#include <chrono>
#include <cstdint>
#include <cerrno>

class TimePoint;

class Duration {

public:

    typedef std::chrono::steady_clock::duration dur;
    typedef std::chrono::seconds        s;
    typedef std::chrono::milliseconds   ms;
    typedef std::chrono::microseconds   us;
    typedef std::chrono::nanoseconds    ns;

    friend TimePoint;

    Duration()
        : m_dur()
    {
    }

    Duration(const dur& d)
        : m_dur(d)
    {
    }

    /* Arithmetic operator overloading */

    Duration operator + (const Duration& other) const
    {
        return this->m_dur + other.m_dur;
    }

    Duration& operator += (const Duration& other)
    {
        this->m_dur += other.m_dur;
        return *this;
    }

    Duration operator-(const Duration& other) const
    {
        return this->m_dur - other.m_dur;
    }

    Duration& operator-=(const Duration& other)
    {
        this->m_dur -= other.m_dur;
        return *this;
    }

    Duration operator *(int64_t n) const
    {
        return this->m_dur * n;
    }

    Duration operator *(double n) const
    {
        Duration temp (*this);
        temp *= n;
        return temp;
    }

    Duration& operator*= (int64_t n)
    {
        this->m_dur *= n;
        return *this;
    }

    Duration& operator*= (double n)
    {
        this->m_dur *= n;
        return *this;
    }

    Duration operator/ (int64_t n) const
    {
        return this->m_dur / n;
    }

    Duration operator/ (double n) const
    {
        Duration temp (*this);
        temp /= n;
        return temp;
    }

    Duration& operator/= (int64_t n)
    {
        this->m_dur /= n;
        return *this;
    }

    Duration& operator/= (double n)
    {
        this->m_dur /= n;
        return *this;
    }

    /* comparison operator overloading */

    bool operator < (const Duration& other) const
    {
        return this->m_dur < other.m_dur;
    }

    bool operator <= (const Duration& other) const
    {
        return this->m_dur <= other.m_dur;
    }

    bool operator ==(const Duration& other) const
    {
        return this->m_dur == other.m_dur;
    }

    bool operator >= (const Duration& other) const
    {
        return this->m_dur >= other.m_dur;
    }

    bool operator > (const Duration& other) const
    {
        return this->m_dur > other.m_dur;
    }

    double seconds_f() const
    {
        std::chrono::duration<double> res = m_dur;
        return res.count();
    }

    int64_t seconds() const
    {
        s d = std::chrono::duration_cast<s>(m_dur);
        return d.count();
    }

    int64_t millis() const
    {
        ms d = std::chrono::duration_cast<ms>(m_dur);
        return d.count();
    }

    int64_t micros() const
    {
        us d = std::chrono::duration_cast<us>(m_dur);
        return d.count();
    }
    
    int64_t nanos() const
    {
        ns d = std::chrono::duration_cast<ns>(m_dur);
        return d.count();
    }

    static Duration from_seconds(int64_t s)
    {
        return Duration(
            std::chrono::duration_cast<dur>(
                std::chrono::seconds(s)
                )
        );
    }

    static Duration from_ms(int64_t ms)
    {
        return Duration(
            std::chrono::duration_cast<dur>(
                std::chrono::milliseconds(ms)
                )
        );
    }

    static Duration from_us(int64_t us)
    {
        return Duration(
            std::chrono::duration_cast<dur>(
                std::chrono::microseconds(us)
                )
            );
    }

    static Duration from_ns(int64_t ns)
    {
        return Duration(
            std::chrono::duration_cast<dur> (
                std::chrono::nanoseconds(ns)
                )
            );
    }

private:

    dur m_dur;
};

#endif
