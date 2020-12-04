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

int bstw_fscanf(FILE *file, struct bst_wrapper *tree)
{
    int status = 0;

    unsigned int numbers_count = 0U;
    int num;

    rewind(file);
    while (fscanf(file, "%d", &num) != EOF)
        numbers_count++;

    if (numbers_count == 0U)
        status = -1;
    else
    {
        *tree = bstw_create();

        rewind(file);
        while (fscanf(file, "%d", &num) != EOF)
            bstw_insert(tree, num);
    }

    return status;
}

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
