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
 * successfully, otherwise, the meta class will think initialization failed
 * and it will destroy the newly allocated class.
 */
typedef int (*see_class_init_func)(
    SeeObjectClass* new_class
    );

/**
 * Values that tell the metaclass->new function everything about the
 * the new class to instantiate.
 */
enum see_meta_new_args {
    /**
     * \brief Tells the meta class how many bytes to allocate for the
     * class instance.
     */
    SEE_META_NEW_CLASS_SIZE = SEE_OBJECT_INIT_SENTINAL,
    SEE_META_NEW_SENTINAL,
};

/**
 * Describes the init list Helps with initializing the new class.
 *
 * This uses SEE_META_NEW_SENTINAL as base, so there is no confusion between
 * the arguments see_meta_new_args and see_meta_init_args
 */
enum see_meta_init_args {
    /**
     * size of the instances the class is going to make.
     * Expects size_t argument
     */
     SEE_META_INIT_INSTANCE_SIZE = SEE_META_NEW_SENTINAL,

     /**
      * The init func will initialize the virtual functions and the functions
      * that extend the parent class. Expects a function pointer to the
      * function that is going to initialize the class.
      */
     SEE_META_INIT_CLS_INIT_FUNC,

     /**
      * This parameter is used to copy the derived function pointers from the
      * parent to the deriving class, expects a size_t argument.
      */
      SEE_META_INIT_PARENT_CLS_SIZE,

      /**
       * Next argument is a pointer to the parent class
       */
      SEE_META_INIT_PARENT,

      /**
       * Marks that the meta class init func should not expect any other
       * arguments and this one doesn't take any.
       */
       SEE_META_INIT_FINISHED,

      /**
       * Final item in the list
       */
      SEE_META_INIT_SENTINAL
};

struct _SeeMetaClass {
    SeeObjectClass cls;
};

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
