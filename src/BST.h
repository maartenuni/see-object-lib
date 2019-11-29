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
typedef char*(*see_node_stringfy_func)(const SeeBSTNode* node);

struct SeeBSTNode {
    SeeBSTNode* node_left;
    SeeBSTNode* node_right;
};

struct SeeBST {
    SeeObject       parent_obj;

    /**
     * \brief The root of the tree.
     * \private
     */
    SeeBSTNode*     root;

    /**
     * \brief Compares two nodes in the tree to achieve a total ordering of the
     * nodes in the tree.
     * \private
     */
    see_cmp_func    cmp_node;

    /**
     * \brief Frees a node when a node is removed from the tree.
     */
    see_free_func   free_node;

    /**
     * \brief a function that stringifies a key from a node.
     *
     * When a key isn't found, this function is used to give a descriptive
     * key error because the key isn't found.
     * \private
     */
    see_node_stringfy_func stringify_node;
};

struct SeeBSTClass {
    SeeObjectClass parent_cls;
    
    int (*bst_init)(
        SeeBST*                 bst,
        const SeeBSTClass*      bst_cls,
        see_cmp_func            bst_cmp_node,
        see_free_func           bst_free_node,
        see_node_stringfy_func  bst_node_stringify,
        SeeError**              error_out
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

/**
 * \brief cast a pointer from a in SeeObject style derived instance of
 *        SeeBSTNode. Clients for a bst node need to derive from a SeeBSTNode
 *        and add a key and value type. The functions however, expect a
 *        SeeBSTNode*, this helps with casting a derived pointer back to its
 *        SeeBSTNode* signature.
 */
#define SEE_BST_NODE(obj)\
    ((SeeBSTNode*) obj)

/**
 * \brief Cast a pointer to pointer to a SeeBSTNode derived instance to a
 *        SeeBSTNode** (a reference to a node).
 */
#define SEE_BST_NODE_REF(obj)\
    ((SeeBSTNode**) obj)

/* **** public functions **** */

/**
 * \brief Create a new binary search tree.
 *
 * @param [out] obj_out The newly generated binary search tree.
 * @param [in]  bst_cmp_node The function that compares two of the node
 *                           the function should take care of a total ordering.
 * @param [in]  bst_free_node If a node is freed from the tree this function
 *                           will release all resources.
 * @param [out] error_out    If an error occurs it will be returned here.
 * @return SEE_SUCCESS or another function when not successful.
 */
SEE_EXPORT int
see_bst_new(
    SeeBST**        obj_out,
    see_cmp_func    bst_cmp_node,
    see_free_func   bst_free_node,
    SeeError**      error_out
    );

/**
 * \brief Insert a new key value pair into the tree
 *
 * If there already is a node with the same key, the existing node will be
 * popped from the tree using the free function specified when creating
 * the tree.
 *
 * @param [in, out]tree The tree in which the key should be inserted.
 * @param [in]     node The node (key-value pair) to insert in the tree.
 * @return SEE_SUCCESS or SEE_INVALID_ARGUMENT
 */
SEE_EXPORT int
see_bst_insert(SeeBST* tree, SeeBSTNode* node);

/**
 * \brief Find and return a node in the tree.
 *
 * @param [in] tree  The tree to search for a key.
 * @param [in] key   A node whose key is set for comparison. It depends on how
 *                   the compare_function is implemented, but in theory they
 *                   should only compare the key, hence setting the key should
 *                   be enough.
 * @param [out]out   If a node with similar key is in the tree,
 *                   a pointer to the node is returned here.
 * @param error_out  If an error occurs it is returned here.
 *
 * @return  SEE_SUCCESS, SEE_INVALID_ARGUMENT, SEE_ERROR_KEY
 */
SEE_EXPORT int
see_bst_find(
    SeeBST*             tree,
    const SeeBSTNode*   key,
    SeeBSTNode**        out,
    SeeError**          error_out
    );

/**
 * \brief Compute the depth of the tree
 *
 * This function visits every node in the tree. The returned size will be
 * the length from the root to the deepest layer in the tree.
 *
 * @param [in] tree The tree whose depth you would like to know.
 * @param [out] size_out The depth of the tree will be returned here.
 * @return SEE_SUCCESS or SEE_INVALID_ARGUMENT
 */
SEE_EXPORT int
see_bst_depth(const SeeBST* tree, size_t* size_out);

/**
 * \brief Compute the size of the tree
 *
 * This function visits every node in the tree. The returned size will be
 * the length from the root to the deepest layer in the tree.
 *
 * @param [in] tree The tree whose size you would like to know.
 * @param [out] size_out The size of the tree will be returned here.
 * @return SEE_SUCCESS or SEE_INVALID_ARGUMENT
 */
SEE_EXPORT int
see_bst_size(const SeeBST* tree, size_t* size_out);

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
