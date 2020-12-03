#include "./bst.h"

#define CALLOCS_REDEFINE
#include "callocs.h"

static int __create_leaf(struct bst **leaf, int data)
{
    *leaf = malloc(sizeof(struct bst));
    if (!*leaf)
        return -1;
    (*leaf)->depth = 1;
    (*leaf)->data = data;
    (*leaf)->count = 1;
    (*leaf)->left = NULL;
    (*leaf)->right = NULL;
    return 0;
}

struct bst *bst_create(void)
{
    return NULL;
}

static void __rc_destroy(struct bst *tree)
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

void bst_destroy(struct bst **tree)
{
    if (*tree != NULL)
    {
        __rc_destroy(*tree);
        free(*tree);
        *tree = NULL;
    }
}

int bst_insert(struct bst **tree, int data)
{
    int status = 0;

    if (*tree == NULL)
    {
        status = __create_leaf(tree, data);
    }
    else if ((*tree)->depth == 0)
    {
        (*tree)->data = data;
        (*tree)->count = 1;
        (*tree)->depth = 1;
    }
    else if (data == (*tree)->data)
        (*tree)->count++;
    else if (data < (*tree)->data)
    {
        if ((*tree)->left)
        {
            status = bst_insert(&(*tree)->left, data);
            (*tree)->depth = (*tree)->depth > 1 + (*tree)->left->depth ?
                (*tree)->depth : (1 + (*tree)->left->depth);
        }
        else
        {
            status = __create_leaf(&(*tree)->left, data);
            (*tree)->depth = (*tree)->depth == 1 ? 2 : (*tree)->depth;
        }
    }
    else
    {
        if ((*tree)->right)
        {
            status = bst_insert(&(*tree)->right, data);
            (*tree)->depth = (*tree)->depth > 1 + (*tree)->right->depth ?
                (*tree)->depth : (1 + (*tree)->right->depth);
        }
        else
        {
            status = __create_leaf(&(*tree)->right, data);
            (*tree)->depth = (*tree)->depth == 1 ? 2 : (*tree)->depth;
        }
    }

    return status;
}

int bst_remove(struct bst *tree, int data)
{
    int status = 0;

    if (tree->depth-- > 0) // expect found
    {
        if (tree->data == data)
        {
            if (tree->count > 1)
            {
                tree->count--;
                tree->depth++;
            }
        }
        else if (data < tree->data && tree->left)
        {
            status = bst_remove(tree->left, data);
            if (tree->left->depth == 0)
            {
                free(tree->left);
                tree->left = NULL;
            }
        }
        else if (tree->data < data && tree->right)
        {
            status = bst_remove(tree->right, data);
            if (tree->right->depth == 0)
            {
                free(tree->right);
                tree->right = NULL;
            }
        }
    }
    else
        status = -1;

    if (status == -1)
        tree->depth++; // not found

    return status;
}

struct bst *bst_find(struct bst *tree, int data)
{
    if (tree->depth == 0)
        return NULL;
    else if (tree->data == data)
        return tree;
    else if (data < tree->data)
    {
        if (tree->left)
            return bst_find(tree->left, data);
        else
            return NULL;
    }
    else if (tree->data < data)
    {
        if (tree->right)
            return bst_find(tree->right, data);
        else
            return NULL;
    }
    return NULL;
}
