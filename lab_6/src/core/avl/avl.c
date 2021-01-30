#include <assert.h>
#include "./avl.h"
#include "utils/logger.h"
#define CALLOCS_REDEFINE
#include "callocs.h"

static int __create_leaf(struct avl **leaf, int data)
{
    *leaf = malloc(sizeof(struct avl));
    if (!*leaf)
    {
        log_error("__create_leaf (AVL): Невозможно выделить %u байт памяти", sizeof(struct avl));
        return -1;
    }
    (*leaf)->depth = 1;
    (*leaf)->diff = 0;
    (*leaf)->data = data;
    (*leaf)->count = 1;
    (*leaf)->left = NULL;
    (*leaf)->right = NULL;
    return 0;
}

struct avl *avl_create(void)
{
    return NULL;
}

static void __rc_destroy(struct avl *tree)
{
    if (tree->left)
    {
        __rc_destroy(tree->left);
        free(tree->left);
    }
    if (tree->right)
    {
        __rc_destroy(tree->right);
        free(tree->right);
    }
}

void avl_destroy(struct avl **tree)
{
    if (*tree != NULL)
    {
        __rc_destroy(*tree);
        free(*tree);
        *tree = NULL;
    }
}

#define MAX_DEPTH(depth_1, depth_2) ((depth_1) > (depth_2) ? (depth_1) : (depth_2))
#define DETERM_DEPTH(node)                                                              \
    (node->left ? (node->right ? (MAX_DEPTH(node->left->depth, node->right->depth) + 1) \
                               : (node->left->depth + 1))                               \
                : (node->right ? (node->right->depth + 1)                               \
                               : (1)))

#define DETERM_DIFF(node)                                                 \
    (node->left ? (node->right ? (node->right->depth - node->left->depth) \
                               : (-node->left->depth))                    \
                : (node->right ? (node->right->depth)                     \
                               : (0)))

static struct avl *__balance_tree(struct avl *tree);

static struct avl *__right_short_rotate(struct avl *tree)
{
    struct avl *node_x = tree;
    struct avl *node_y = node_x->left;

    struct avl *sub_tree = node_y->right;

    node_y->right = node_x;
    node_x->left = sub_tree;

    // update depth in nodes
    node_x->depth = DETERM_DEPTH(node_x);
    node_y->depth = DETERM_DEPTH(node_y);

    node_x->diff = DETERM_DIFF(node_x);
    node_y->diff = DETERM_DIFF(node_y);

    tree = node_y;

    tree->left = __balance_tree(tree->left);
    tree->right = __balance_tree(tree->right);
    tree->depth = DETERM_DEPTH(tree);
    tree->diff = DETERM_DIFF(tree);

    return tree;
}

static struct avl *__right_deep_rotate(struct avl *tree)
{
    struct avl *node_x = tree;
    struct avl *node_y = node_x->left;
    struct avl *node_z = node_y->right;

    struct avl *sub_tree_left = node_z->left;
    struct avl *sub_tree_right = node_z->right;

    node_z->left = node_y;
    node_z->right = node_x;

    node_x->left = sub_tree_right;
    node_y->right = sub_tree_left;

    // update depth in nodes
    node_x->depth = DETERM_DEPTH(node_x);
    node_y->depth = DETERM_DEPTH(node_y);
    node_z->depth = MAX_DEPTH(node_x->depth, node_y->depth) + 1;

    node_x->diff = DETERM_DIFF(node_x);
    node_y->diff = DETERM_DIFF(node_y);
    node_z->diff = DETERM_DIFF(node_z);

    tree = node_z;

    tree->left = __balance_tree(tree->left);
    tree->right = __balance_tree(tree->right);
    tree->depth = DETERM_DEPTH(tree);
    tree->diff = DETERM_DIFF(tree);

    return tree;
}

static struct avl *__left_short_rotate(struct avl *tree)
{
    struct avl *node_x = tree;
    struct avl *node_y = node_x->right;

    struct avl *sub_tree = node_y->left;

    node_y->left = node_x;
    node_x->right = sub_tree;

    // update depth in nodes
    node_x->depth = DETERM_DEPTH(node_x);
    node_y->depth = DETERM_DEPTH(node_y);

    node_x->diff = DETERM_DIFF(node_x);
    node_y->diff = DETERM_DIFF(node_y);

    tree = node_y;

    tree->left = __balance_tree(tree->left);
    tree->right = __balance_tree(tree->right);
    tree->depth = DETERM_DEPTH(tree);
    tree->diff = DETERM_DIFF(tree);

    return tree;
}

static struct avl *__left_deep_rotate(struct avl *tree)
{
    struct avl *node_x = tree;
    struct avl *node_y = node_x->right;
    struct avl *node_z = node_y->left;

    struct avl *sub_tree_left = node_z->left;
    struct avl *sub_tree_right = node_z->right;

    node_z->left = node_x;
    node_z->right = node_y;

    node_x->right = sub_tree_left;
    node_y->left = sub_tree_right;

    // update depth in nodes
    node_x->depth = DETERM_DEPTH(node_x);
    node_y->depth = DETERM_DEPTH(node_y);
    node_z->depth = MAX_DEPTH(node_x->depth, node_y->depth) + 1;

    node_x->diff = DETERM_DIFF(node_x);
    node_y->diff = DETERM_DIFF(node_y);
    node_z->diff = DETERM_DIFF(node_z);

    tree = node_z;

    tree->left = __balance_tree(tree->left);
    tree->right = __balance_tree(tree->right);
    tree->depth = DETERM_DEPTH(tree);
    tree->diff = DETERM_DIFF(tree);

    return tree;
}

static struct avl *__balance_tree(struct avl *tree)
{
    while (tree != NULL && (tree->diff > 1 || tree->diff < -1))
    {
        if (tree->diff < -1)
        {
            if (tree->left && tree->left->right)
                tree = __right_deep_rotate(tree);
            else
                tree = __right_short_rotate(tree);
        }
        else
        {
            if (tree->right && tree->right->left)
                tree = __left_deep_rotate(tree);
            else
                tree = __left_short_rotate(tree);
        }
    }

    return tree;
}

int avl_insert(struct avl **tree, int data, int *cmp)
{
    int status = 0;
    struct avl *root = *tree;

    if (root == NULL)
    {
        status = __create_leaf(tree, data);
    }
    else if (root->depth == 0)
    {
        root->data = data;
        root->count = 1;
        root->depth = 1;
        (*cmp)++;
    }
    else if (data == root->data)
    {
        root->count++;
        (*cmp)++;
    }
    else if (data < root->data)
    {
        if (root->left)
        {
            status = avl_insert(&root->left, data, cmp);
            root->depth = DETERM_DEPTH(root);
            if (root->right)
                root->diff = root->right->depth - root->left->depth;
            else
                root->diff = -root->left->depth;
            (*cmp)++;
        }
        else
        {
            status = __create_leaf(&root->left, data);
            root->depth = root->depth == 1 ? 2 : root->depth;
            root->diff--;
            (*cmp)++;
        }
    }
    else
    {
        if (root->right)
        {
            status = avl_insert(&root->right, data, cmp);
            root->depth = DETERM_DEPTH(root);
            if (root->left)
                root->diff = root->right->depth - root->left->depth;
            else
                root->diff = root->right->depth;
            (*cmp)++;
        }
        else
        {
            status = __create_leaf(&root->right, data);
            root->depth = root->depth == 1 ? 2 : root->depth;
            root->diff++;
            (*cmp)++;
        }
    }

    if (root != NULL && (root->diff < -1 || root->diff > 1))
        *tree = __balance_tree(*tree);

    return status;
}

int avl_shallow_insert(struct avl **tree, int data)
{
    int status = 0;
    struct avl *root = *tree;

    if (root == NULL)
    {
        status = 0;
    }
    else if (root->depth == 0)
    {
    }
    else if (data == root->data)
    {
    }
    else if (data < root->data)
    {
        if (root->left)
        {
            status = avl_shallow_insert(&root->left, data);
            if (root->right)
                (void)(root->right->depth - root->left->depth);
            else
                (void)(-root->left->depth);
        }
        else
        {
            status = 0;
            (void)(root->depth == 1 ? 2 : root->depth);
        }
    }
    else
    {
        if (root->right)
        {
            status = avl_shallow_insert(&root->right, data);
            if (root->left)
                (void)(root->right->depth - root->left->depth);
            else
                (void)(root->right->depth);
        }
        else
        {
            (void)(root->depth == 1 ? 2 : root->depth);
        }
    }

    return status;
}

// возвращает -1 если элемента нет, и 0 - если успешно удалён.
int avl_remove(struct avl *tree, int data)
{
    (void)tree;
    (void)data;
    return 0;
}

// NULL - если не найдено.
struct avl *avl_find(struct avl *tree, int data, int *cmp)
{
    (*cmp)++;
    if (tree == NULL)
    {
        (*cmp)--;
        return NULL;
    }
    else if (data == tree->data)
        return tree;
    else if (data < tree->data)
        return avl_find(tree->left, data, cmp);
    else
        return avl_find(tree->right, data, cmp);
}
