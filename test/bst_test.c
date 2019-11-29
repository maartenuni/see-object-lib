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
#include "test_macros.h"
#include "../src/BST.h"

static const char* SUITE_NAME = "SeeBST Suite";

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

//static int_int_node*
//int_int_node_new(int key, int value)
//{
//    int_int_node* new = malloc(sizeof(int_int_node));
//    memset(new, 0, sizeof(int_int_node));
//
//    if (!new)
//        return new;
//
//    new->key = key;
//    new->value = value;
//
//    return new;
//}

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

/* *******************************/
/* ******** unit tests ***********/
/* *******************************/

static void
bst_create(void)
{
    SeeError* error = NULL;
    SeeBST* bst = NULL;
    size_t depth, size;

    int ret = see_bst_new(&bst, int_int_node_cmp, free, &error);
    SEE_UNIT_HANDLE_ERROR();

    ret = see_bst_depth(bst, &depth);
    SEE_UNIT_HANDLE_ERROR();
    ret = see_bst_size(bst, &size);
    SEE_UNIT_HANDLE_ERROR();

    CU_ASSERT_EQUAL(depth, 0);
    CU_ASSERT_EQUAL(size, 0);

fail:
    see_object_decref(SEE_OBJECT(bst));
}

static void
bst_insert(void)
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

    int ret = see_bst_new(&bst, str_int_node_cmp, str_int_node_free, &error);
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
}

static void
bst_find(void)
{
    SeeError* error = NULL;
    SeeBST* bst = NULL;

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

    int ret = see_bst_new(&bst, str_int_node_cmp, str_int_node_free, &error);
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

fail:
    see_object_decref(SEE_OBJECT(bst));
}

int
add_bst_suite(void)
{
    SEE_UNIT_SUITE_CREATE(NULL, NULL);
    SEE_UNIT_TEST_CREATE(bst_create);
    SEE_UNIT_TEST_CREATE(bst_insert);
    SEE_UNIT_TEST_CREATE(bst_find);

    return 0;
}
