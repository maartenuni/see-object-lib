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
        
    int (*seed) (SeeRandom* random, unsigned i);
    
    int (*get_seed) (SeeRandom* random, unsigned* i);
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

/*
 * Carefully examine whether BETWEEN obj_out and error_out should be another
 * parameter. Eg for SeeError that might be a const char* msg. This
 * should also be added in the .c file.
 *
 * Remove this comment and add useful documentation
 */
SEE_EXPORT int
see_random_new(SeeRandom** obj_out, SeeError** error_out);

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
