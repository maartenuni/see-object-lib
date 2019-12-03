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


#ifndef SEE_RANDOM_HPP
#define SEE_RANDOM_HPP

#include <random>

class Random {

    public:

        Random()
        {
            std::random_device rd;
            seed_rand(m_seed);
        }

        void seed_rand(unsigned seed)
        {
            m_seed = seed;
            gen.seed(m_seed);
        }

        unsigned get_seed()
        {
            return m_seed;
        }

        /**
         * \brief return a value from the range [min, max].
         */
        unsigned uniform_uint(unsigned min, unsigned max)
        {
            std::uniform_int_distribution<unsigned>dist(min, max);
            return dist(gen);
        }

    private:
        std::mt19937 gen;
        unsigned m_seed;
};

#endif //ifndef SEE_RANDOM_H
