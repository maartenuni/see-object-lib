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


#include <assert.h>
#include "MetaClass.h"
#include "BST.h"
#include "see_functions.h"

/* **** functions that implement SeeBST or override SeeObject **** */

static int
bst_init(
    SeeBST*             bst,
    const SeeBSTClass*  bst_cls,
    see_cmp_func        bst_cmp_node,
    see_free_func       bst_free_node,
    SeeError**          error_out
    )
{
    int ret = SEE_SUCCESS;
    (void) error_out;
    const SeeObjectClass* parent_cls = SEE_OBJECT_GET_CLASS(
        bst
        );
        
    /* Because every class has its own members to initialize, you have
     * to check how your parent is initialized and pass through all relevant
     * parameters here. Typically, this should be parent_name_init, where
     * parent_name is the name of the parent and it should be the first function
     * that extends the parent above its parent.
     * Check how the parent is initialized and pass through the right parameters.
     */
    parent_cls->object_init(
            SEE_OBJECT(bst),
            SEE_OBJECT_CLASS(bst_cls)
            );

    bst->cmp_node = bst_cmp_node;
    bst->free_node= bst_free_node;
    
    return ret;
}

static int
init(const SeeObjectClass* cls, SeeObject* obj, va_list args)
{
    const SeeBSTClass* bst_cls = SEE_BST_CLASS(cls);
    SeeBST* bst = SEE_BST(obj);
    
    /*Extract parameters here from va_list args here.*/
    see_cmp_func cmp_func = va_arg(args, see_cmp_func);
    see_free_func free_func = va_arg(args,see_free_func);
    SeeError** error = va_arg(args, SeeError**);
    
    return bst_cls->bst_init(
        bst,
        bst_cls,
        cmp_func,
        free_func,
        error
        );
}

static SeeBSTNode* tree_insert(
    SeeBST* tree,
    SeeBSTNode* tree_node,
    SeeBSTNode* new_node
    )
{
    assert(new_node->node_left == NULL);
    assert(new_node->node_right == NULL);

    if (!tree_node)
        return new_node;

    int ret = tree->cmp_node(tree_node, new_node);
    if (ret > 0) // new node is smaller
        tree_node->node_left = tree_insert(tree, tree_node, new_node);
    else if(ret < 0) // new node is larger
        tree_node->node_right = tree_insert(tree, tree_node, new_node);
    else { // they are equal
        new_node->node_right = tree_node->node_right;
        new_node->node_left =  tree_node->node_left;
        tree->free_node(tree_node);
        tree_node = new_node;
    }
    return tree_node;
}

static int
bst_insert(SeeBST* tree, SeeBSTNode* new_node)
{
    tree->root = tree_insert(tree, tree->root, new_node);
    return SEE_SUCCESS;
}

static int
tree_find(
    SeeBST*             tree,
    SeeBSTNode*         root,
    const SeeBSTNode*   key,
    SeeBSTNode**        out,
    SeeError**          error_out
    )
{
    if (!root) {
        // TODO new_error_here;
        return SEE_ERROR_KEY;
    }

    int cmp = tree->cmp_node(root, key);
    if (cmp == 0) {
        *out = root;
        return SEE_SUCCESS;
    }
    else if (cmp > 0)
        return tree_find(tree, root->node_right, key, out, error_out);
    else
        return tree_find(tree, root->node_left, key, out, error_out);
}

static int
bst_find(
    SeeBST* tree, const SeeBSTNode* key, SeeBSTNode** out, SeeError** error_out
)
{
    return tree_find(tree, tree->root, key, out, error_out);
}

/* **** implementation of the public API **** */

/*
 * Carefully examine whether BETWEEN obj_out and error_out should be another
 * parameter. Eg for SeeError that might be a const char* msg. This
 * should also be added in the header file.
 */
int see_bst_new(SeeBST** obj_out, SeeError** error_out)
{
    const SeeObjectClass* cls = SEE_OBJECT_CLASS(
        see_bst_class()
        );

    if (!cls)
        return SEE_NOT_INITIALIZED;

    if (!obj_out || !error_out || *obj_out || *error_out)
        return SEE_INVALID_ARGUMENT;

    return cls->new_obj(
            cls,
            0,
            SEE_OBJECT_REF(obj_out),
            /* add extra params here and remove,*/
            error_out
            );
}

/* **** initialization of the class **** */

SeeBSTClass* g_SeeBSTClass = NULL;

static int bst_class_init(SeeObjectClass* new_cls)
{
    int ret = SEE_SUCCESS;
    
    /* Override the functions on the SeeObject here */
    new_cls->init = init;

    // Every class should have a unique name.
    new_cls->name = "SeeBST";

    // These can be optionally overwritten
    // Then you need to create static function above with the proper
    // signature.
    // new_cls->compare        = bst_compare;
    // new_cls->less           = bst_less;
    // new_cls->less_equal     = bst_less_equal;
    // new_cls->equal          = bst_equal;
    // new_cls->not_equal      = bst_not_equal;
    // new_cls->greater_equal  = bst_greater_equal;
    // new_cls->greater        = bst_greater;
    // new_cls->copy           = bst_copy;


    /* Set the function pointers of the own class here */
    SeeBSTClass* cls = (SeeBSTClass*) new_cls;
    cls->bst_init = bst_init;
    cls->insert   = bst_insert;
    cls->find     = bst_find;
    //TODO
    //cls->delete   = bst_delete;
    
    return ret;
}

/**
 * \private
 * \brief this class initializes SeeBST(Class).
 *
 * You might want to call this from the library initialization func.
 */
int
see_bst_init()
{
    int ret;
    const SeeMetaClass* meta = see_meta_class_class();

    ret = see_meta_class_new_class(
        meta,
        (SeeObjectClass**) &g_SeeBSTClass,
        sizeof(SeeBSTClass),
        sizeof(SeeBST),
        SEE_OBJECT_CLASS(see_object_class()),
        sizeof(SeeObjectClass),
        bst_class_init
        );

    return ret;
}

void
see_bst_deinit()
{
    if(!g_SeeBSTClass)
        return;

    see_object_decref(SEE_OBJECT(g_SeeBSTClass));
    g_SeeBSTClass = NULL;
}

const SeeBSTClass*
see_bst_class()
{
    return g_SeeBSTClass;
}

