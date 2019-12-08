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

#define SIZE_MASK (~((size_t) 0) >> 1u)
#define COLOR_MASK ~SIZE_MASK

typedef enum {
    RED,
    BLACK
} rb_color;

static rb_color
node_color(const SeeBSTNode* n)
{
    if (n && n->size & COLOR_MASK)
        return RED;
    return BLACK;
}

static void
set_color(SeeBSTNode* node, rb_color color)
{
    assert(node != NULL);
    if (color == RED)
        node->size |= COLOR_MASK;
    else
        node->size &= ~COLOR_MASK;
}

static void
rotate_left(SeeBSTNode** node_ref)
{
    SeeBSTNode* x = (*node_ref)->right;
    (*node_ref)->right = x->left;
    x->left = (*node_ref);
    set_color(x, node_color(*node_ref));
    *node_ref = x;
    x->size = (*node_ref)->size;
    (*node_ref)->size =
        bst_size((*node_ref)->left) +
        bst_size((*node_ref)->right) + 1u;
}

static int
is_red(SeeBSTNode* node)
{
    return node_color(node) == RED;
}

/* **** functions that implement SeeBalancedTree or override SeeBST **** */

static int
balanced_insert(SeeBST* tree, SeeBSTNode* node)
{
    //TODO
    int ret = SEE_SUCCESS;
    int cmp = tree->cmp_node();
    return ret;
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

