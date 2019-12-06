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


#ifndef SEE_BALANCED_TREE_H
#define SEE_BALANCED_TREE_H

#include <stdint.h>

#include "BST.h"
#include "Error.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct SeeBalancedTree SeeBalancedTree;
typedef struct SeeBalancedTreeClass SeeBalancedTreeClass;
typedef struct SeeBalancedNode SeeBalancedNode;

struct SeeBalancedNode {
    SeeBSTNode bst_node;
    uint8_t    is_red;
};

struct SeeBalancedTree {
    SeeBST parent_obj;
    /*expand SeeBalancedTree data here*/
        
};

struct SeeBalancedTreeClass {
    SeeBSTClass parent_cls;
    
    int (*balanced_tree_init)(
        SeeBalancedTree*            balanced_tree,
        const SeeBalancedTreeClass* balanced_tree_cls
        /* Put instance specific arguments here and remove this comment. */
        );
        
    /* expand SeeBalancedTree class with extra functions here.*/
};

/* **** function style macro casts **** */

/**
 * \brief cast a pointer from a SeeBalancedTree derived instance back to a
 *        pointer to SeeBalancedTree.
 */
#define SEE_BALANCED_TREE(obj)                      \
    ((SeeBalancedTree*) obj)

/**
 * \brief cast a pointer to pointer from a SeeBalancedTree derived instance back to a
 *        reference to SeeBalancedTree*.
 */
#define SEE_BALANCED_TREE_REF(ref)                      \
    ((SeeBalancedTree**) ref)

/**
 * \brief cast a pointer to SeeBalancedTreeClass derived class back to a
 *        pointer to SeeBalancedTreeClass.
 */
#define SEE_BALANCED_TREE_CLASS(cls)                      \
    ((const SeeBalancedTreeClass*) cls)

/**
 * \brief obtain a pointer to SeeBalancedTreeClass from a instance of
 *        derived from SeeBalancedTree. This macro is preferably
 *        used when obtaining the class of a instance. When this
 *        macro is used. Calling methods on the class will enable
 *        polymorphism, because you'll get the derived class.
 */
#define SEE_BALANCED_TREE_GET_CLASS(obj)                \
    (SEE_BALANCED_TREE_CLASS(see_object_get_class(SEE_OBJECT(obj)) )  )

/**
 * \brief cast a pointer from a in SeeObject style derived instance of
 *        SeeBalancedNode. Clients for a bst node need to derive from a SeeBalancedNode
 *        and add a key and value type. The functions however, expect a
 *        SeeBalancedNode*, this helps with casting a derived pointer back to its
 *        SeeBalancedNode* signature.
 */
#define SEE_BALANCED_NODE(obj)\
    ((SeeBalancedNode*) obj)

/**
 * \brief Cast a pointer to pointer to a SeeBalancedNode derived instance to a
 *        SeeBalancedNode** (a reference to a node).
 */
#define SEE_BALANCED_NODE_REF(obj)\
    ((SeeBalancedNode**) obj)


/* **** public functions **** */

/*
 * Carefully examine whether BETWEEN obj_out and error_out should be another
 * parameter. Eg for SeeError that might be a const char* msg. This
 * should also be added in the .c file.
 *
 * Remove this comment and add useful documentation
 */
SEE_EXPORT int
see_balanced_tree_new(
        SeeBalancedTree**       out,
        see_cmp_func            cmp_node,
        see_free_func           free_node,
        see_node_stringfy_func  stringify_node,
        SeeError**              error_out
        );


/**
 * Gets the pointer to the SeeBalancedTreeClass table.
 */
SEE_EXPORT const SeeBalancedTreeClass*
see_balanced_tree_class();

/* Expand the class with public functions here, don't forget the SEE_EXPORT
 * macro, because otherwise you'll run into troubles when exporting function
 * in a windows dll.
 */

/* **** class initialization functions **** */

/**
 * Initialize SeeBalancedTree; make it ready for use.
 */
SEE_EXPORT
int see_balanced_tree_init();

/**
 * Deinitialize SeeBalancedTree, after SeeBalancedTree has been deinitialized,
 * all functions in this header shouldn't be used anymore.
 */
SEE_EXPORT
void see_balanced_tree_deinit();

#ifdef __cplusplus
}
#endif

#endif //ifndef SEE_BALANCED_TREE_H
