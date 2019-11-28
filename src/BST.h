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

/**
 * \brief In essence a tree consist of nodes that point to other nodes
 *
 * If one would like to use the binary search tree, one would derive from
 * SeeBSTNode
 */
struct SeeBSTNode {
    SeeBSTNode* left;
    SeeBSTNode* right;
};

struct SeeBST {
    SeeObject parent_obj;
    /*expand SeeBST data here*/
    SeeBSTNode*     root;
    see_cmp_func    cmp_nodes;
    see_free_func   free_func;
};

struct SeeBSTClass {
    SeeObjectClass parent_cls;
    
    int (*bst_init)(
        SeeBST*             bst,
        const SeeBSTClass*  bst_cls,
        see_cmp_func        node_cmp_func,
        see_free_func       free_func,
        SeeError**          error_out
        );

    /**
     * \brief Insert a new node in the tree, if a node with equal key exists, it
     * will be deleted from the tree.
     * \private
     */
    void* (*insert)(SeeBSTNode* node);
    /**
     * \brief Remove a node from the tree.
     * \private
     */
    void* (*delete)(SeeBSTNode* node);

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

/**
 * Find a node with similar key in the tree.
 */
SEE_EXPORT void*
see_bst_find(SeeBST* tree, const SeeBSTNode* node, SeeError);

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
