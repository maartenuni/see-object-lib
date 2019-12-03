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
#include "KeyError.h"

/* **** functions that implement SeeBST or override SeeObject **** */

static int
bst_init(
    SeeBST*                 bst,
    const SeeBSTClass*      bst_cls,
    see_cmp_func            bst_cmp_node,
    see_free_func           bst_free_node,
    see_node_stringfy_func  bst_stringify_key,
    SeeError**              error_out
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

    bst->cmp_node       = bst_cmp_node;
    bst->free_node      = bst_free_node;
    bst->stringify_node = bst_stringify_key;
    
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
    see_node_stringfy_func stringify_func = va_arg(args, see_node_stringfy_func);
    SeeError** error = va_arg(args, SeeError**);
    
    return bst_cls->bst_init(
        bst,
        bst_cls,
        cmp_func,
        free_func,
        stringify_func,
        error
        );
}

static void
bst_destroy(SeeBSTNode* tree, see_free_func freefunc)
{
    if (!tree)
        return;
    bst_destroy(tree->left, freefunc);
    bst_destroy(tree->right, freefunc);
    freefunc(tree);
}

static void
destroy(SeeObject* obj)
{
    SeeBST* tree = SEE_BST(obj);
    bst_destroy(tree->root, tree->free_node);
    see_object_class()->destroy(obj);
}

static SeeBSTNode* tree_insert(
    SeeBST* tree,
    SeeBSTNode* tree_node,
    SeeBSTNode* new_node
    )
{
    assert(new_node->left == NULL);
    assert(new_node->right == NULL);

    if (!tree_node)
        return new_node;

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
        char* strkey = tree->stringify_node(key);
        see_key_error_new(error_out, strkey);
        free(strkey);
        return SEE_ERROR_KEY;
    }

    int cmp = tree->cmp_node(root, key);
    if (cmp == 0) {
        *out = root;
        return SEE_SUCCESS;
    }
    else if (cmp > 0) //key is smaller then root, hence look in the right subtree
        return tree_find(tree, root->left, key, out, error_out);
    else
        return tree_find(tree, root->right, key, out, error_out);
}

static int
bst_find(
    SeeBST*             tree,
    const SeeBSTNode*   key,
    SeeBSTNode**        out,
    SeeError**          error_out
    )
{
    return tree_find(tree, tree->root, key, out, error_out);
}

/**
 * \brief Removes the minimal node of a given subtree.
 *
 * This function retrieves the minimum key-value pair from the tree
 * by storing it in out. 
 */
static SeeBSTNode*
bst_extract_min(
        SeeBSTNode* node,
        SeeBSTNode** out
        )
{
    if (!node) {
        *out = NULL;
        return node;
    }
    
    if (node->left) {
        node->left = bst_extract_min(node->left, out);
        return node;
    }

    *out = node;
    return node->right;
}


static int
bst_delete(
        SeeBST*             tree,
        SeeBSTNode**        node,
        const SeeBSTNode*   key,
        SeeError**          error_out
        )
{
    int cmp, ret;

    if (!(*node)) {
        if (error_out) {
            char* tempstr = tree->stringify_node(key);
            see_key_error_new(error_out, tempstr);
            free(tempstr);
        }
        return SEE_ERROR_KEY;
    }

    cmp = tree->cmp_node(*node, key);
    if (cmp > 0) // Key is larger.
        ret = bst_delete(tree, &((*node)->right), key, error_out);
    else if (cmp < 0) // Key is smaller
        ret = bst_delete(tree, &((*node)->left), key, error_out);
    else {
        if ((*node)->right == NULL) { // Return left node.
            SeeBSTNode* left = (*node)->left;
            tree->free_node(*node);
            *node = left;
            return SEE_SUCCESS;
        }
        if ((*node)->left == NULL) { // Return right node
            SeeBSTNode* right = (*node)->right;
            tree->free_node(*node);
            *node = right;
            return SEE_SUCCESS;
        }
        //TODO  randomly replace the current node with either the largest
        //      of the left subtree or with the smallest of the right subtree
        SeeBSTNode* minormax = NULL;
        (*node)-> right = bst_extract_min((*node)->right, &minormax);
        minormax->right = (*node)->right;
        minormax->left  = (*node)->left;
        tree->free_node(*node);
        *node = minormax;

        ret = SEE_SUCCESS;
    }
    return ret;
}

static size_t
bst_depth(const SeeBSTNode* node)
{
    size_t left;
    size_t right;
    if (!node)
        return 0;

    left = bst_depth(node->left);
    right = bst_depth(node->right);
    return (left > right ? left : right) + 1;
}

static size_t
bst_size(const SeeBSTNode* node)
{
    if (!node)
        return 0;

    return bst_size(node->left) + bst_size(node->right) + 1;
}

/* **** implementation of the public API **** */

int
see_bst_new(
   SeeBST**                 obj_out,
   see_cmp_func             bst_cmp_node,
   see_free_func            bst_free_node,
   see_node_stringfy_func   key_stringify,
   SeeError**               error_out
   )
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
            bst_cmp_node,
            bst_free_node,
            key_stringify,
            error_out
            );
}

int
see_bst_insert(SeeBST* tree, SeeBSTNode* node)
{
    if (!tree || !node)
        return SEE_INVALID_ARGUMENT;

    const SeeBSTClass* cls = SEE_BST_GET_CLASS(tree);
    return cls->insert(tree, node);
}

int
see_bst_depth(const SeeBST* tree, size_t* size_out)
{
    if (!tree || !size_out)
        return SEE_INVALID_ARGUMENT;

    size_t depth = bst_depth(tree->root);
    *size_out = depth;
    return SEE_SUCCESS;
}

int
see_bst_size(const SeeBST* tree, size_t* size_out)
{
    if (!tree || !size_out)
        return SEE_INVALID_ARGUMENT;

    *size_out = bst_size(tree->root);
    return SEE_SUCCESS;
}

int
see_bst_find(
    SeeBST*             tree,
    const SeeBSTNode*   key,
    SeeBSTNode**        out,
    SeeError**          error_out
    )
{
    if (!tree || !key)
        return SEE_INVALID_ARGUMENT;

    if (!out || *out || !error_out || *error_out)
        return SEE_INVALID_ARGUMENT;

    const SeeBSTClass* cls = SEE_BST_GET_CLASS(tree);
    return cls->find(tree, key, out, error_out);
}

int
see_bst_delete(
    SeeBST*             tree,
    const SeeBSTNode*   key,
    SeeError**          error_out
    )
{
    if (!tree || !key)
        return SEE_INVALID_ARGUMENT;
    if (!error_out || *error_out)
        return SEE_INVALID_ARGUMENT;
    
    return SEE_BST_GET_CLASS(tree)->delete(tree, &tree->root, key, error_out);
}

/* **** initialization of the class **** */

SeeBSTClass* g_SeeBSTClass = NULL;

static int bst_class_init(SeeObjectClass* new_cls)
{
    int ret = SEE_SUCCESS;
    
    /* Override the functions on the SeeObject here */
    new_cls->init = init;
    new_cls->destroy = destroy;

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
    cls->delete   = bst_delete;
    
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

