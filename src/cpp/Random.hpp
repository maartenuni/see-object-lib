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
 * \file Random.hpp
 * \brief Provides the private random number generator
 * \private
 */

#ifndef SEE_RANDOM_HPP
#define SEE_RANDOM_HPP

#include <random>

class Random {

    public:

        Random()
            : m_seed(0)
        {
            std::random_device rd;
            seed_rand(std::random_device()());
        }

        void seed_rand(uint64_t seed)
        {
            m_seed = seed;
            gen.seed(m_seed);
        }

        uint64_t get_seed() const
        {
            return m_seed;
        }

        /**
         * \brief return a value from the range [min, max].
         */
        uint32_t uniform_uint32_range(uint32_t min, uint32_t max)
        {
            std::uniform_int_distribution<uint32_t>dist(min, max);
            return dist(gen);
        }

        /**
         * \brief return a random 32bit integer
         */
        uint32_t uniform_uint32()
        {
            constexpr uint32_t min = std::numeric_limits<uint32_t>::min();
            constexpr uint32_t max = std::numeric_limits<uint32_t>::max();
            return uniform_uint32_range(min, max);
        }

        /**
         * \brief return a value from the range [min, max].
         */
        int32_t uniform_int32_range(int32_t min, int32_t max)
        {
            std::uniform_int_distribution<int32_t>dist(min, max);
            return dist(gen);
        }

        /**
         * \brief return a random 32bit integer
         * @return
         */
        int uniform_int32()
        {
            constexpr auto min = std::numeric_limits<int32_t>::min();
            constexpr auto max = std::numeric_limits<int32_t>::max();
            return uniform_uint32_range(min, max);
        }

        /**
         * \brief return a value from the range [min, max].
         */
        uint64_t uniform_uint64_range(uint64_t min, uint64_t max)
        {
            std::uniform_int_distribution<uint64_t>dist(min, max);
            return dist(gen);
        }

        /**
         * \brief return a random 64bit integer
         */
        uint64_t uniform_uint64()
        {
            constexpr uint64_t min = std::numeric_limits<uint64_t>::min();
            constexpr uint64_t max = std::numeric_limits<uint64_t>::max();
            return uniform_uint64_range(min, max);
        }

        /**
         * \brief return a value from the range [min, max].
         */
        int64_t uniform_int64_range(int64_t min, int64_t max)
        {
            std::uniform_int_distribution<int64_t>dist(min, max);
            return dist(gen);
        }

        /**
         * \brief return a random 64bit integer
         * @return
         */
        int uniform_int64()
        {
            constexpr auto min = std::numeric_limits<int64_t>::min();
            constexpr auto max = std::numeric_limits<int64_t>::max();
            return uniform_uint64_range(min, max);
        }

        /**
         * \brief generate floats in the range [min, max]
         */
        double uniform_float_range(double min, double max)
        {
            std::uniform_real_distribution<double>dist(min, max);
            return dist(gen);
        }

        /**
         * \brief return a number in the range[0.0, 1.0]
         * @return value between and including 0.0  and 1.0
         */
        double uniform_float()
        {
            return uniform_float_range(0.0, 1.0);
        }

        /**
         * \brief return a number from the range [0.0, 1.0)
         * \note in contrast to uniform_float, this will not generate 1.0
         */
        double canonical_float()
        {
            return std::generate_canonical<
                double,
                std::numeric_limits<double>::digits
                >(gen);
        }

        /**
         * \brief Draw a number from a normal distribution with a mean and
         *        standard deviation.
         */
        double normal_float(double mean, double std)
        {
            std::normal_distribution<double> dist{mean, std};
            return dist(gen);
        }

    private:

        std::mt19937_64 gen;

        unsigned m_seed;
};

#endif //ifndef SEE_RANDOM_HPP
