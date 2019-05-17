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


#ifndef SEE_INDEX_ERROR_H
#define SEE_INDEX_ERROR_H

#include "Error.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _SeeIndexError SeeIndexError;
typedef struct _SeeIndexErrorClass SeeIndexErrorClass;

/**
 * @brief An instance of SeeIndexError,
 * \private
 */
struct _SeeIndexError {
    /**
     * @brief SeeIndexError derives from SeeError.
     * @private
     */
    SeeError parent_obj;

};

/**
 * @brief implements the SeeIndexErrorClass.
 * \private
 */
struct _SeeIndexErrorClass {

    /**
     * \brief The SeeIndexErrorClass derives from SeeErrorClass.
     * \private
     */
    SeeErrorClass parent_cls;

    /**
     * @brief initialize a SeeIndexError
     *
     * \private
     *
     * @param [in] index_error     A pointer to a newly allocated index_error
     * @param [in] index_error_cls A pointer to the class.
     * @param [in] index           The index that was invalid (out of range).
     * @return
     */
    int (*index_error_init)(
        SeeIndexError*            index_error,
        const SeeIndexErrorClass* index_error_cls,
        size_t                    index
        );
        
    /* expand SeeIndexError class with extra functions here.*/
};

/* **** function style macro casts **** */

/**
 * \brief cast a pointer from a SeeIndexError derived instance back to a
 *        pointer to SeeIndexError.
 */
#define SEE_INDEX_ERROR(obj)                      \
    ((SeeIndexError*) obj)

/**
 * \brief cast a pointer to pointer from a SeeIndexError derived instance back to a
 *        reference to SeeIndexError*.
 */
#define SEE_INDEX_ERROR_REF(ref)                      \
    ((SeeIndexError**) ref)

/**
 * \brief cast a pointer to SeeIndexErrorClass derived class back to a
 *        pointer to SeeIndexErrorClass.
 */
#define SEE_INDEX_ERROR_CLASS(cls)                      \
    ((const SeeIndexErrorClass*) cls)

/**
 * \brief obtain a pointer to SeeIndexErrorClass from a instance of
 *        derived from SeeIndexError. This macro is preferably
 *        used when obtaining the class of a instance. When this
 *        macro is used. Calling methods on the class will enable
 *        polymorphism, because you'll get the derived class.
 */
#define SEE_INDEX_ERROR_GET_CLASS(obj)                \
    (SEE_INDEX_ERROR_CLASS(see_object_get_class(SEE_OBJECT(obj)) )  )

/* **** public functions **** */

/**
 * Create a new index error for a given index.
 *
 * Index Errors are thrown from Array like objects when an index is out of
 * range/bounds.
 *
 * @param [out] error   The error is returned here. error must be a valid pointer
 *                      It should point to a SeeError* that point to NULL.
 * @param [in]  i       The index that caused this error.
 *
 * @return SEE_SUCCESS (or you are really out of luck).
 */
SEE_EXPORT int
see_index_error_create(SeeError** error, size_t i);


/**
 * Gets the pointer to the SeeIndexErrorClass table.
 */
SEE_EXPORT const SeeIndexErrorClass*
see_index_error_class();

/* Expand the class with public functions here, don't forget the SEE_EXPORT
 * macro, because otherwise you'll run into troubles when exporting function
 * in a windows dll.
 */

/* **** class initialization functions **** */

/**
 * Initialize SeeIndexError; make it ready for use.
 */
SEE_EXPORT
int see_index_error_init();

/**
 * Deinitialize SeeIndexError, after SeeIndexError has been deinitialized,
 * all functions in this header shouldn't be used anymore.
 */
SEE_EXPORT
void see_index_error_deinit();

#ifdef __cplusplus
}
#endif

#endif //ifndef SEE_INDEX_ERROR_H
