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


#ifndef MetaClass_H
#define MetaClass_H

#include "SeeObject.h"

#ifdef __cplusplus
extern "C" {
#endif

struct _SeeMetaClass;
typedef struct _SeeMetaClass SeeMetaClass;

/**
 * \brief A function that is called when a new class is created in
 * order to override virtual functions and to initialize
 * other class members.
 *
 * When a new class is created, the user is allowed to override
 * the functions that are derived from its parent. This function
 * will be called to override those function pointers on the parent.
 * When a new class is created the function pointers point to
 * the same functions as the parents "vtable", this function
 * allows to specialize the derived class.
 * The function should return SEE_SUCCESS when it initialized the class
 * succesfully, otherwise, the meta class will think initialization failed
 * and it will destroy the newly allocated class.
 */
typedef int (*see_class_init_func)(
    SeeObjectClass* new_class
    );

/**
 * \brief allocate and initialize a new class instance.
 */
typedef int (*see_new_class_instance_func) (
    const SeeMetaClass*     meta,
    SeeObjectClass**        out,
    size_t                  class_instance_size,
    size_t                  instance_size,
    const SeeObjectClass*   parent,
    size_t                  parent_cls_size,
    see_class_init_func     init_func
    );

/**
 * \brief An instance of the metaclass. This class instance is used to generate
 * new classes that derive from another class.
 */
struct _SeeMetaClass {
    SeeObjectClass  cls;
    see_new_class_instance_func new_cls_instance;
};


/**
 * @brief Ask from the meta class to generate a new class.
 * @param [in]  meta                A valid pointer to the meta class
 *                                  (use eg see_meta_class_class())
 * @param [out] out                 A pointer to an uninitialized class in such way
 *                                  that *out should be NULL, but out isn't.
 * @param [in]  class_instance_size The size of the class.
 * @param [in]  instance_size       The size of the instance the class makes.
 * @param [in]  parent              A pointer to the Initialized parent class.
 * @param [in]  parent_cls_size     The size of the parent class.
 * @param init_func                 A function that will initialize the
 *                                  virtual functions/members of the parent.
 *                                  Also new members not part of the parent
 *                                  should be initialized by this function. This
 *                                  function specializes the newly generated
 *                                  class.
 *
 * @return SEE_SUCCESS or another values indicating an error that has occurred.
 */
SEE_EXPORT int
see_meta_class_new_class(
    const SeeMetaClass*     meta,
    SeeObjectClass**        out,
    size_t                  class_instance_size,
    size_t                  instance_size,
    const SeeObjectClass*   parent,
    size_t                  parent_cls_size,
    see_class_init_func     init_func
    );

/**
 * \brief initialize the meta class. Generally this function is already called
 * by see_init.
 * @return SEE_SUCCESS or another error indicating some status.
 */
SEE_EXPORT int
see_meta_class_init();

/**
 * \brief Deinitialize the meta class, after this call no new classes can
 * be made anymore.
 */
SEE_EXPORT void
see_meta_class_deinit();

/**
 * @brief get the meta class' class instance.
 * @return the instance to the SeeMetaClass instance.
 */
SEE_EXPORT const SeeMetaClass*
see_meta_class_class();

#ifdef __cplusplus
}
#endif

#endif //ifndef MetaClass_H