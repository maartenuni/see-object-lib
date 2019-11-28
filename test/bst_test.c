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

#include "test_macros.h"
#include "../src/BST.h"

static const char* SUITE_NAME = "SeeBST Suite";

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

void
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
    SeeBSTNode* n = (SeeBSTNode*) int_int_node_new(1, 2);

    ret = see_bst_insert(bst, n);
    SEE_UNIT_HANDLE_ERROR();

    see_bst_depth(bst, &depth);
    CU_ASSERT_EQUAL(depth, 1);
    see_bst_size(bst, &size);
    CU_ASSERT_EQUAL(size, 1);

fail:
    see_object_decref(SEE_OBJECT(bst));
}

int
add_bst_suite(void)
{
    SEE_UNIT_SUITE_CREATE(NULL, NULL);
    SEE_UNIT_TEST_CREATE(bst_create);

    return 0;
}
