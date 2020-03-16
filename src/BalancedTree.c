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


#include "MetaClass.h"
#include "BST.h"
#include "BalancedTree.h"
#include <stdbool.h>
#include <assert.h>

// It might be an idea to use the most significant bit of the size to store
// the tree nodes' color. That might save a int/char to store that is stored
// separately.
//
//#define SIZE_MASK (~((size_t) 0) >> 1u)
//#define COLOR_MASK ~SIZE_MASK

typedef enum {
    RED,
    BLACK
} rb_color;

/**
 * \brief Determines the color of a node.
 *
 * This function determines the color of a node. If the node == NULL, its
 * considered a BLACK node. Otherwise the node->color is examined.
 *
 * \private
 *
 * @param n
 * @return
 */
static rb_color
node_color(const SeeBSTNode* n)
{
    return n && n->color == RED ? RED : BLACK;
}

/**
 * \brief Set the color of a node
 *
 * @param [out] node  The node whose color you want to set. May NOT be NULL.
 * @param [in]  color The desired color of the node.
 *
 * \private
 */
static void
set_color(SeeBSTNode* node, rb_color color)
{
    assert(node != NULL);
    if (color == RED)
        node->color = RED;
    else
        node->color = BLACK;
}

static void
rotate_left(SeeBSTNode** node_ref)
{
    SeeBSTNode* x = (*node_ref)->right;
    (*node_ref)->right = x->left;
    x->left = (*node_ref);
    set_color(x, node_color(*node_ref));
    set_color(*node_ref, RED);
    x->size = see_bst_node_size(*node_ref);
    (*node_ref)->size = 1 + see_bst_node_size((*node_ref)->left) +
                            see_bst_node_size((*node_ref)->right);
    *node_ref = x;
}

static void
rotate_right(SeeBSTNode** node_ref)
{
    SeeBSTNode* x = (*node_ref)->left;
    (*node_ref)->left = x->right;
    x->right = (*node_ref);
    set_color(x, node_color(*node_ref));
    set_color(*node_ref, RED);
    x->size = see_bst_node_size(*node_ref);
    (*node_ref)->size = 1 + see_bst_node_size((*node_ref)->left) +
                            see_bst_node_size((*node_ref)->right);
    *node_ref = x;
}

static void
flip_colors(SeeBSTNode* n)
{
    assert(n != NULL);
    assert(node_color(n->left) == RED);
    assert(node_color(n->right) == RED);
    set_color(n, RED);
    set_color(n->left, BLACK);
    set_color(n->right, BLACK);
}

/**
 * Determine whether the node is red or not.
 *
 * @param [in] node May be NULL, but then its black, for other nodes the node
 *                  will be examined which color it is.
 *
 * @return 1 if it is red, 0 if it is black.
 * @private
 */
static int
is_red(SeeBSTNode* node)
{
    return node_color(node) == RED;
}

/* **** functions that implement SeeBalancedTree or override SeeBST **** */

static SeeBSTNode*
tree_insert(
   SeeBST*      tree,
   SeeBSTNode*  tree_node,
   SeeBSTNode*  new_node
   )
{
    assert(new_node->left == NULL);
    assert(new_node->right == NULL);

    if (!tree_node) {
        new_node->size = 1;
        return new_node;
    }

    int ret = tree->cmp_node(tree_node, new_node);
    if (ret > 0) // new node is smaller
        tree_node->left = tree_insert(tree, tree_node->left, new_node);
    else if(ret < 0) // new node is larger
        tree_node->right = tree_insert(tree, tree_node->right, new_node);
    else { // they are equal
        new_node->right = tree_node->right;
        new_node->left =  tree_node->left;
        tree->free_node(tree_node);
        tree_node = new_node;
    }

    if (is_red(tree_node->right) && !is_red(tree_node->left))
        rotate_left(&tree_node);
    if (is_red(tree_node->left) && is_red(tree_node->left->left))
        rotate_right(&tree_node);
    if (is_red(tree_node->left) && is_red(tree_node->right))
        flip_colors(tree_node);

    tree_node->size = 1 + see_bst_node_size(tree_node->left) +
                          see_bst_node_size(tree_node->right);

    return tree_node;
}

static int
balanced_insert (
    SeeBST* tree,
    SeeBSTNode* new_node
    )
{
    // A little housekeeping
    new_node->size = 0;
    set_color(new_node, RED);

    tree->root = tree_insert(tree, tree->root, new_node);

    // The root should be kept black.
    set_color(tree->root, BLACK);
    return SEE_SUCCESS;
}

static int
balanced_delete(
        SeeBST*             tree,
        SeeBSTNode**        root ,
        const SeeBSTNode*   key,
        SeeError**          error_out
        )
{
    //TODO
    int ret = SEE_SUCCESS;
    (void) tree;
    (void) root;
    (void) key;
    (void) error_out;
    return ret;
}

/* **** implementation of the public API **** */

/*
 * Carefully examine whether BETWEEN obj_out and error_out should be another
 * parameter. Eg for SeeError that might be a const char* msg. This
 * should also be added in the header file.
 */
int see_balanced_tree_new(
        SeeBalancedTree**      obj_out,
        see_cmp_func           cmp_func,
        see_free_func          free_func,
        see_node_stringfy_func key_stringify,
        SeeError**             error_out
        )
{
    const SeeObjectClass* cls = SEE_OBJECT_CLASS(
        see_balanced_tree_class()
        );

    if (!cls)
        return SEE_NOT_INITIALIZED;

    if (!obj_out || !error_out || *obj_out || *error_out)
        return SEE_INVALID_ARGUMENT;

    return cls->new_obj(
            cls,
            0,
            SEE_OBJECT_REF(obj_out),
            cmp_func,
            free_func,
            key_stringify,
            error_out
            );
}

/* **** initialization of the class **** */

SeeBalancedTreeClass* g_SeeBalancedTreeClass = NULL;

static int balanced_tree_class_init(SeeObjectClass* new_cls)
{
    int ret = SEE_SUCCESS;

    /* Override the functions on the SeeObject here */
    // 
    // new_cls->init = init;

    // Every class should have a unique name.
    new_cls->name = "SeeBalancedTree";

    // These can be optionally overwritten
    // Then you need to create static function above with the proper
    // signature.
    // new_cls->compare        = balancedtree_compare;
    // new_cls->less           = balancedtree_less;
    // new_cls->less_equal     = balancedtree_less_equal;
    // new_cls->equal          = balancedtree_equal;
    // new_cls->not_equal      = balancedtree_not_equal;
    // new_cls->greater_equal  = balancedtree_greater_equal;
    // new_cls->greater        = balancedtree_greater;
    // new_cls->copy           = balancedtree_copy;


    // Overwrite functions of the parent here
    SeeBSTClass* parent_cls = (SeeBSTClass*)new_cls;
    parent_cls->insert = balanced_insert;
    parent_cls->delete = balanced_delete;
    
    return ret;
}

/**
 * \private
 * \brief this class initializes SeeBalancedTree(Class).
 *
 * You might want to call this from the library initialization func.
 */
int
see_balanced_tree_init()
{
    int ret;
    const SeeMetaClass* meta = see_meta_class_class();

    ret = see_bst_init();
    if (ret)
        return ret;

    ret = see_meta_class_new_class(
        meta,
        (SeeObjectClass**) &g_SeeBalancedTreeClass,
        sizeof(SeeBalancedTreeClass),
        sizeof(SeeBalancedTree),
        SEE_OBJECT_CLASS(see_bst_class()),
        sizeof(SeeBSTClass),
        balanced_tree_class_init
        );

    return ret;
}

void
see_balanced_tree_deinit()
{
    if(!g_SeeBalancedTreeClass)
        return;

    see_object_decref(SEE_OBJECT(g_SeeBalancedTreeClass));
    g_SeeBalancedTreeClass = NULL;
}

const SeeBalancedTreeClass*
see_balanced_tree_class()
{
    return g_SeeBalancedTreeClass;
}

