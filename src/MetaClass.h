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
 * \file MetaClass.h This file exports the public interface to the MetaClass.
 *
 * The instance of meta class is responsible of generating new classes.
 * MetaClass, hence instance of MetaClass are classes themselves. You
 * The metaclass overrides SeeObjectClass.new. Therefore instances of
 * SeeMetaClass have a unknown instance size.
 */

#ifndef MetaClass_H
#define MetaClass_H

#include "SeeObject.h"

#ifdef __cplusplus
extern "C" {
#endif

struct SeeMetaClass;
typedef struct SeeMetaClass SeeMetaClass;

/**
 * \brief A function that is called when a new class is created in
 * order to override virtual functions and to initialize
 * other class members.
 *
 * When a new class is created, the user is allowed to override
 * the functions that are derived from its parent. This function
 * will be called to override those function pointers on the derived class.
 * When a new class is created the function pointers point to
 * the same functions as the parents "vtable", this function
 * allows to specialize the derived class.
 * Additionally this function should set the new function pointer that the
 * parent doesn't have.
 * The function should return SEE_SUCCESS when it initialized the class
 * successfully, otherwise, the meta class will think initialization failed
 * and it will destroy the newly allocated class.
 */
typedef int (*see_class_init_func)(
    SeeObjectClass* new_class
    );

/**
 * \brief SeeMetaClass contains only one function, to initialize new
 * classes.
 *
 * Next to the fact new classes can be initialized, the meta class overrides
 * the default SeeObject->new to generate new classes instance instead
 * of regular instances.
 */
struct SeeMetaClass {
    SeeObjectClass cls;

    int (*class_init)(
        SeeObjectClass*         new_cls,
        const SeeObjectClass*   meta_cls,
        size_t                  instance_size,
        const SeeObjectClass*   parent,
        size_t                  parent_cls_size,
        see_class_init_func     init_func
        );
};

/**
 * Cast a pointer to a SeeMeta derived object to a pointer of SeeMeta.
 *
 * Note make sure it is SeeMeta derived
 */
#define SEE_META(obj)\
    ((SeeMeta*)(obj))

/**
 * @brief Cast a pointer to pointer to an instance derived from SeeMeta
 * To a reference to a SeeMeta*
 */
#define SEE_META_REF(obj)\
    ((SeeMeta**)(obj))

/**
 * Cast a pointer to a SeeMetaClass derived class back to a const SeeMetaClass
 * instance.
 */
#define SEE_META_CLASS(cls)\
    ((const SeeMetaClass*) (cls))

/**
 * Get a const pointer a (derived) SeeMetaClass. This cast can be used
 * to call polymorphic functions.
 */
#define SEE_META_GET_CLASS(obj)\
    ( SEE_META_CLASS( see_object_get_class( SEE_OBJECT(obj) ) )  )

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
 * @brief Ask from the meta class to generate a new class.
 * @param [in]  meta                A valid pointer to the meta class
 *                                  (use eg see_meta_class_class())
 * @param [out] out                 A pointer to an uninitialized class in such way
 *                                  that *out should be NULL, but out isn't.
 * @param [in]  class_instance_size The size of the class.
 * @param [in]  instance_size       The size of the instance the class makes.
 * @param [in]  parent              A pointer to the Initialized parent class.
 * @param [in]  parent_cls_size     The size of the parent class.
 * @param [in]  init_func           A function that will initialize the
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
