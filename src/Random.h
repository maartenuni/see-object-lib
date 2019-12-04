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


#ifndef SEE_RANDOM_H
#define SEE_RANDOM_H

#include "SeeObject.h"
#include "Error.h"
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct SeeRandom SeeRandom;
typedef struct SeeRandomClass SeeRandomClass;

struct SeeRandom {
    SeeObject parent_obj;
    void* priv;            
};

struct SeeRandomClass {
    SeeObjectClass parent_cls;
    
    int (*random_init)(
        SeeRandom*            random,
        const SeeRandomClass* random_cls
        );
};

/* **** function style macro casts **** */

/**
 * \brief cast a pointer from a SeeRandom derived instance back to a
 *        pointer to SeeRandom.
 */
#define SEE_RANDOM(obj)                      \
    ((SeeRandom*) obj)

/**
 * \brief cast a pointer to pointer from a SeeRandom derived instance back to a
 *        reference to SeeRandom*.
 */
#define SEE_RANDOM_REF(ref)                      \
    ((SeeRandom**) ref)

/**
 * \brief cast a pointer to SeeRandomClass derived class back to a
 *        pointer to SeeRandomClass.
 */
#define SEE_RANDOM_CLASS(cls)                      \
    ((const SeeRandomClass*) cls)

/**
 * \brief obtain a pointer to SeeRandomClass from a instance of
 *        derived from SeeRandom. This macro is preferably
 *        used when obtaining the class of a instance. When this
 *        macro is used. Calling methods on the class will enable
 *        polymorphism, because you'll get the derived class.
 */
#define SEE_RANDOM_GET_CLASS(obj)                \
    (SEE_RANDOM_CLASS(see_object_get_class(SEE_OBJECT(obj)) )  )

/* **** public functions **** */

/**
 * \brief Create a new random generator.
 *
 * @param [out] rand_out
 * @param [out] error_out
 *
 * @return SEE_SUCCESS if successful.
 */
SEE_EXPORT int
see_random_new(SeeRandom** rand_out, SeeError** error_out);

/**
 * \brief Set the random seed of the generator it should produce identical
 * results when given the same seed.
 *
 * @param [in] random
 * @param [in] seed
 *
 * @return SEE_SUCCESS or SEE_INVALID_ARGUMENT
 */
SEE_EXPORT int
see_random_seed(SeeRandom* random, unsigned seed);


/**
 * \brief Get the random seed of the generator.
 *
 * @param [in] random
 * @param [out]seed
 *
 * @return SEE_SUCCESS or SEE_INVALID_ARGUMENT
 */
SEE_EXPORT int
see_random_get_seed(SeeRandom* random, unsigned* seed);

/**
 * \brief return a random int32_t.
 *
 * @param [in] random may be NULL, then a global default will be used,
 *                    note this isn't thread safe.
 * @return a random value between and including the maximal and minimal
 *         32 bit integer value.
 */
SEE_EXPORT int32_t
see_random_int32(SeeRandom* random);

/**
 * \brief return a random value in the range [min, max]
 *
 * @param [in] random A random device. This may be null, then a global device is
 *                    used, note that this isn't thread safe.
 * @param [in] min    The minimal value (inclusive)
 * @param [in] max    The maximal value (inclusive)
 *
 * @return a uniformly drawn number in the range [min, max]
 */
SEE_EXPORT int32_t
see_random_int32_range(SeeRandom* random, int32_t min, int32_t max);
/**
 * \brief return a random int32_t.
 *
 * @param [in] random may be NULL, then a global default will be used,
 *                    note this isn't thread safe.
 * @return a random value between and including the maximal and minimal
 *         32 bit integer value.
 */

SEE_EXPORT uint32_t
see_random_uint32(SeeRandom* random);

/**
 * \brief return a random value in the range [min, max]
 *
 * @param [in] random A random device. This may be null, then a global device is
 *                    used, note that this isn't thread safe.
 * @param [in] min    The minimal value (inclusive)
 * @param [in] max    The maximal value (inclusive)
 *
 * @return a uniformly drawn number in the range [min, max]
 */
SEE_EXPORT int32_t
see_random_int32_range(SeeRandom* random, int32_t min, int32_t max);

/**
 * \brief return a double precision floating point number
 *        in the range [0.0, 1.0]
 *
 * @param [in] random A random device. This may be null, then a global device is
 *                    used, note that this isn't thread safe.
 */
SEE_EXPORT double
see_random_float(SeeRandom* random);

/**
 * \brief return a double precision floating point number
 *        in the range [0.0, 1.0]
 *
 * @param [in] random A random device. This may be null, then a global device is
 *                    used, note that this isn't thread safe.
 */
SEE_EXPORT double
see_random_float_range(SeeRandom* random, double min, double max);

/**
 * \brief return a value from a normal distribution in with
 * a mean and a given standard deviation.
 */
SEE_EXPORT double
see_random_normal_float(SeeRandom* random, double mean, double std);

/**
 * Gets the pointer to the SeeRandomClass table.
 */
SEE_EXPORT const SeeRandomClass*
see_random_class();

/* Expand the class with public functions here, don't forget the SEE_EXPORT
 * macro, because otherwise you'll run into troubles when exporting function
 * in a windows dll.
 */

/* **** class initialization functions **** */

/**
 * Initialize SeeRandom; make it ready for use.
 */
SEE_EXPORT
int see_random_init();

/**
 * Deinitialize SeeRandom, after SeeRandom has been deinitialized,
 * all functions in this header shouldn't be used anymore.
 */
SEE_EXPORT
void see_random_deinit();

#ifdef __cplusplus
}
#endif

#endif //ifndef SEE_RANDOM_H
