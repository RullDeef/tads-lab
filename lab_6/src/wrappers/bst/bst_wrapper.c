#include "bst_wrapper.h"

#define CALLOCS_REDEFINE
#include "callocs.h"

struct bst_wrapper bstw_create(void)
{
    struct bst_wrapper bstw;
    bstw.root = bst_create();
    return bstw;
}

void bstw_destroy(struct bst_wrapper *bstw)
{
    bst_destroy(&bstw->root);
}

int bstw_insert(struct bst_wrapper *bstw, int data)
{
    return bst_insert(&bstw->root, data);
}

// возвращает -1 если элемента нет, и 0 - если успешно удалён.
int bstw_remove(struct bst_wrapper *bstw, int data)
{
    return bst_remove(bstw->root, data);
}

// NULL - если не найдено.
struct bst *bstw_find(struct bst_wrapper *bstw, int data)
{
    return bst_find(bstw->root, data);
}

/*
#define PRN_DATA_WIDTH 5
static int *__get_by_path(struct bst *tree, long path, int len)
{
    if (len == 0)
        return &tree->data;
    else if ((path & (1 << (len - 1))) == 0 && tree->left)
        return __get_by_path(tree->left, path, len - 1);
    else if ((path & (1 << (len - 1))) == 1 && tree->right)
        return __get_by_path(tree->right, path, len - 1);
    else
        return NULL;
}
*/

static void __prn_rec(FILE *file, struct bst *node, int tab)
{
    if (node != NULL && node->depth > 0)
    {
        __prn_rec(file, node->left, tab + 1);

        for (int i = 0; i < tab; i++)
            fprintf(file, "%*s", 9, "");
        fprintf(file, "%5d[%2d]\n", node->data, node->depth);

        __prn_rec(file, node->right, tab + 1);
    }
}

void bstw_fprintf(FILE *file, struct bst_wrapper *bstw)
{
    __prn_rec(file, bstw->root, 0);
}
