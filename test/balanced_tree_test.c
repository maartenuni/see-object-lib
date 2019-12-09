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
#include <stdlib.h>
#include "test_macros.h"
#include "../src/BalancedTree.h"
#include "../src/DynamicArray.h"

static const char* SUITE_NAME = "SeeBalanceTree Suite";

/* *************************************************/
/* ******** helper functions and structs ***********/
/* *************************************************/

typedef struct int_int_node{
    SeeBSTNode node;
    int key;
    int value;
} int_int_node;

static int
int_int_node_cmp(const void* l, const void* r)
{
    const int_int_node* left = l;
    const int_int_node* right = r;
    return right->key - left->key;
}

static char* int_int_stringify(const SeeBSTNode* key)
{
    char buffer [64];
    int_int_node* n = (int_int_node*) key;
    snprintf(buffer, sizeof(buffer), "%d", n->key);
    return strdup(buffer);
}

static int_int_node*
int_int_node_new(int key, int value)
{
    int_int_node* new = malloc(sizeof(int_int_node));
    memset(new, 0, sizeof(int_int_node));

    if (!new)
        return new;

    new->key = key;
    new->value = value;

    return new;
}

typedef struct str_int_node{
    SeeBSTNode node;
    char* key;
    int value;
} str_int_node;

static int
str_int_node_cmp(const void* l, const void* r)
{
    const str_int_node* left = l;
    const str_int_node* right = r;
    return strcmp(left->key, right->key);
}

static char* str_int_stringify(const SeeBSTNode* key)
{
    str_int_node* n = (str_int_node*) key;
    return strdup(n->key);
}

static str_int_node*
str_int_node_new(const char* key, int val)
{
    str_int_node* new = calloc(1, sizeof(str_int_node));
    char* k = strdup(key);
    if (!k || !new)
        goto fail;

    new->key = k;
    new->value = val;
    return new;

fail:
    free(new);
    free(k);
    return NULL;
}

static void
str_int_node_free(void* node)
{
    str_int_node* n = node;
    free(n->key);
    free(node);
}

/* ****************************** */
/* ******** unit tests ********** */
/* ****************************** */

static void
balanced_tree_create(void)
{
    SeeError* error = NULL;
    SeeBST* bst = NULL;
    size_t depth, size;

    int ret = see_balanced_tree_new(
        SEE_BALANCED_TREE_REF(&bst),
        int_int_node_cmp,
        free,
        &int_int_stringify,
        &error
        );
    SEE_UNIT_HANDLE_ERROR();

    ret = see_bst_depth(bst, &depth);
    SEE_UNIT_HANDLE_ERROR();
    ret = see_bst_size(bst, &size);
    SEE_UNIT_HANDLE_ERROR();

    CU_ASSERT_EQUAL(depth, 0);
    CU_ASSERT_EQUAL(size, 0);

fail:
    see_object_decref(SEE_OBJECT(bst));
    see_object_decref(SEE_OBJECT(error));
}

static void
balanced_tree_insert(void)
{
    SeeError* error = NULL;
    SeeBST* bst = NULL;
    size_t depth, size;

    struct key_val_pair {
        const char* key;
        int value;
    };

    struct key_val_pair table[] = {
        {"boom",    0},
        {"roos",    1},
        {"vis",     2},
        {"mus",     3},
        {"vuur",    4},
        {"pim",     5},
        {"school",  6},
    };

    int ret = see_balanced_tree_new(
        SEE_BALANCED_TREE_REF(&bst),
        str_int_node_cmp,
        str_int_node_free,
        &str_int_stringify,
        &error
        );
    SEE_UNIT_HANDLE_ERROR();

    for (size_t i = 0; i < sizeof(table)/sizeof(table[0]); i++)
    {
        str_int_node* n = str_int_node_new(table[i].key, table[i].value);
        assert(n);
        ret = see_bst_insert(bst, (SeeBSTNode*) n);
        SEE_UNIT_HANDLE_ERROR();
    }

    see_bst_depth(bst, &depth);
    see_bst_size(bst, &size);
    CU_ASSERT_EQUAL(depth, 4);
    CU_ASSERT_EQUAL(size, 7);

fail:
    see_object_decref(SEE_OBJECT(bst));
    see_object_decref(SEE_OBJECT(error));
}

static void
balanced_tree_find(void)
{
    SeeError* error = NULL;
    SeeBST* bst = NULL;

    struct key_val_pair {
        const char* key;
        int value;
    };
    const char* expected_error = "SeeKeyError: No such key \"noot\"";

    struct key_val_pair table[] = {
        {"boom",    0},
        {"roos",    1},
        {"vis",     2},
        {"mus",     3},
        {"vuur",    4},
        {"pim",     5},
        {"school",  6},
    };

    int ret = see_balanced_tree_new(
        SEE_BALANCED_TREE_REF(&bst),
        str_int_node_cmp,
        str_int_node_free,
        str_int_stringify,
        &error
        );
    SEE_UNIT_HANDLE_ERROR();

    for (size_t i = 0; i < sizeof(table)/sizeof(table[0]); i++)
    {
        str_int_node* n = str_int_node_new(table[i].key, table[i].value);
        assert(n);
        ret = see_bst_insert(bst, (SeeBSTNode*) n);
        SEE_UNIT_HANDLE_ERROR();
    }

    for (size_t i = 0; i < sizeof(table)/sizeof(table[0]); i++) {
        str_int_node* n = NULL;
        str_int_node key = {.key = (char*)table[i].key};
        ret = see_bst_find(
            bst,
            SEE_BST_NODE(&key),
            SEE_BST_NODE_REF(&n),
            &error
            );
        SEE_UNIT_HANDLE_ERROR();
        CU_ASSERT_PTR_NOT_NULL(n);
        CU_ASSERT_EQUAL(table[i].value, n->value);
        CU_ASSERT_STRING_EQUAL(table[i].key, n->key);
    }

    str_int_node* found_key = NULL;
    str_int_node key = {.key = "noot"};
    ret = see_bst_find(
        bst,
        SEE_BST_NODE(&key),
        SEE_BST_NODE_REF(&found_key),
        &error
        );
    CU_ASSERT_EQUAL(ret, SEE_ERROR_KEY);
    CU_ASSERT_PTR_NOT_NULL(error);
    CU_ASSERT_STRING_EQUAL(expected_error, see_error_msg(error));

fail:
    see_object_decref(SEE_OBJECT(bst));
    see_object_decref(SEE_OBJECT(error));
}

static void
balanced_tree_insert_in_order(void)
{
    SeeError* error = NULL;
    SeeBST* bst = NULL;
    size_t depth, size;
    const size_t N = 32;
    double logbase2;

    int ret = see_balanced_tree_new(
        SEE_BALANCED_TREE_REF(&bst),
        int_int_node_cmp,
        free,
        &int_int_stringify,
        &error
        );
    SEE_UNIT_HANDLE_ERROR();

    for (size_t i = 0; i < N; i++)
    {
        int_int_node* n = int_int_node_new(i, (N-1)-i);
        assert(n);
        ret = see_bst_insert(bst, SEE_BST_NODE(n));
        SEE_UNIT_HANDLE_ERROR();
    }

    see_bst_depth(bst, &depth);
    see_bst_size(bst, &size);

    logbase2 = log(depth) / log(2.0);
    fprintf(stderr, "Depth of tree = %lu\n", depth);

    CU_ASSERT(N <= ceil(logbase2));
    CU_ASSERT_EQUAL(size, N);

fail:
    see_object_decref(SEE_OBJECT(bst));
    see_object_decref(SEE_OBJECT(error));
}

static void
balanced_tree_delete(void)
{
    SeeError* error = NULL;
    SeeBST* tree = NULL;
    SeeDynamicArray* array = NULL;
    const size_t N = 1024;
    size_t depth, size;

    typedef struct int_pair {
        int key;
        int value;
    } int_pair;

    int ret = see_balanced_tree_new(
            SEE_BALANCED_TREE_REF(&tree),
            &int_int_node_cmp,
            &free,
            &int_int_stringify,
            &error
            );
    SEE_UNIT_HANDLE_ERROR();

    ret = see_dynamic_array_new(
            &array,
            sizeof(int_pair),
            NULL,
            NULL,
            NULL,
            &error
            );
    SEE_UNIT_HANDLE_ERROR();
    for (size_t i = 0; i < N; ++i) {
        int_pair pair = {i, i};
        ret = see_dynamic_array_add(array, &pair, &error);
        SEE_UNIT_HANDLE_ERROR();
    }

    ret = see_dynamic_array_shuffle(array, NULL, &error);
    SEE_UNIT_HANDLE_ERROR();

    for (size_t i = 0; i < N; i++) {
        int_pair pair;
        ret = see_dynamic_array_get(array, i, &pair, &error);
        if (ret)
            SEE_UNIT_HANDLE_ERROR();
        int_int_node* n = int_int_node_new(pair.key, pair.value);
        if (!n) {
            CU_ASSERT_PTR_NOT_NULL(n);
            goto fail;
        }
        ret = see_bst_insert(tree, SEE_BST_NODE(n));
        if (ret)
            SEE_UNIT_HANDLE_ERROR();
    }

    ret = see_bst_size(tree, &size);
    SEE_UNIT_HANDLE_ERROR();
    ret = see_bst_depth(tree, &depth);
    SEE_UNIT_HANDLE_ERROR();

    CU_ASSERT_EQUAL(size, N);
    CU_ASSERT(depth < N);

    ret = see_dynamic_array_shuffle(array, NULL, &error);
    SEE_UNIT_HANDLE_ERROR();

    for (size_t i = 0; i < N; i++) {
        int_pair pair;
        int_int_node key;
        see_dynamic_array_get(array, i, &pair, &error);
        if (ret)
            SEE_UNIT_HANDLE_ERROR();
        key.key = pair.key;
        ret = see_bst_delete(tree, SEE_BST_NODE(&key), &error);
        if (ret)
            SEE_UNIT_HANDLE_ERROR();
    }
    
    ret = see_bst_size(tree, &size);
    SEE_UNIT_HANDLE_ERROR();
    ret = see_bst_depth(tree, &depth);
    SEE_UNIT_HANDLE_ERROR();
    
    CU_ASSERT_EQUAL(size, 0);
    CU_ASSERT_EQUAL(depth, 0);


fail:
    see_object_decref(SEE_OBJECT(error));
    see_object_decref(SEE_OBJECT(tree));
    see_object_decref(SEE_OBJECT(array));
}

int
add_balanced_tree_suite(void)
{
    SEE_UNIT_SUITE_CREATE(NULL, NULL);
    SEE_UNIT_TEST_CREATE(balanced_tree_create);
    SEE_UNIT_TEST_CREATE(balanced_tree_insert);
    SEE_UNIT_TEST_CREATE(balanced_tree_find);
    SEE_UNIT_TEST_CREATE(balanced_tree_insert_in_order);
    SEE_UNIT_TEST_CREATE(balanced_tree_delete);
    (void) balanced_tree_delete;

    return 0;
}


