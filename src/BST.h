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


#ifndef SEE_BST_H
#define SEE_BST_H

#include "SeeObject.h"
#include "Error.h"
#include "see_functions.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct SeeBST SeeBST;
typedef struct SeeBSTClass SeeBSTClass;
typedef struct SeeBSTNode SeeBSTNode;

struct SeeBSTNode {
    SeeBSTNode* node_left;
    SeeBSTNode* node_right;
};

struct SeeBST {
    SeeObject       parent_obj;
    SeeBSTNode*     root;
    see_cmp_func    cmp_node;
    see_free_func   free_node;
};

struct SeeBSTClass {
    SeeObjectClass parent_cls;
    
    int (*bst_init)(
        SeeBST*             bst,
        const SeeBSTClass*  bst_cls,
        see_cmp_func        bst_cmp_node,
        see_free_func       bst_free_node,
        SeeError**          error_out
        );

    int (*insert)   (SeeBST* tree, SeeBSTNode* node);

    int (*find)     (SeeBST* tree,
                     const SeeBSTNode* key,
                     SeeBSTNode** out,
                     SeeError** error_out
                     );

    int (*delete)   (SeeBST* tree,
                     const SeeBSTNode* key,
                     SeeError** error_out
                     );

};

/* **** function style macro casts **** */

/**
 * \brief cast a pointer from a SeeBST derived instance back to a
 *        pointer to SeeBST.
 */
#define SEE_BST(obj)                      \
    ((SeeBST*) obj)

/**
 * \brief cast a pointer to pointer from a SeeBST derived instance back to a
 *        reference to SeeBST*.
 */
#define SEE_BST_REF(ref)                      \
    ((SeeBST**) ref)

/**
 * \brief cast a pointer to SeeBSTClass derived class back to a
 *        pointer to SeeBSTClass.
 */
#define SEE_BST_CLASS(cls)                      \
    ((const SeeBSTClass*) cls)

/**
 * \brief obtain a pointer to SeeBSTClass from a instance of
 *        derived from SeeBST. This macro is preferably
 *        used when obtaining the class of a instance. When this
 *        macro is used. Calling methods on the class will enable
 *        polymorphism, because you'll get the derived class.
 */
#define SEE_BST_GET_CLASS(obj)                \
    (SEE_BST_CLASS(see_object_get_class(SEE_OBJECT(obj)) )  )

/* **** public functions **** */

/*
 * Carefully examine whether BETWEEN obj_out and error_out should be another
 * parameter. Eg for SeeError that might be a const char* msg. This
 * should also be added in the .c file.
 *
 * Remove this comment and add useful documentation
 */
SEE_EXPORT int
see_bst_new(SeeBST** obj_out, SeeError** error_out);

/**
 * Gets the pointer to the SeeBSTClass table.
 */
SEE_EXPORT const SeeBSTClass*
see_bst_class();

/* Expand the class with public functions here, don't forget the SEE_EXPORT
 * macro, because otherwise you'll run into troubles when exporting function
 * in a windows dll.
 */

/* **** class initialization functions **** */

/**
 * Initialize SeeBST; make it ready for use.
 */
SEE_EXPORT
int see_bst_init();

/**
 * Deinitialize SeeBST, after SeeBST has been deinitialized,
 * all functions in this header shouldn't be used anymore.
 */
SEE_EXPORT
void see_bst_deinit();

#ifdef __cplusplus
}
#endif

#endif //ifndef SEE_BST_H
