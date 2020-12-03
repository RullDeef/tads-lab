#include "avl_wrapper.h"

struct avl_wrapper avlw_create(void)
{
    struct avl_wrapper avlw;
    avlw.root = avl_create();
    return avlw;
}

void avlw_destroy(struct avl_wrapper *avlw)
{
    avl_destroy(&avlw->root);
}

int avlw_insert(struct avl_wrapper *avlw, int data)
{
    return avl_insert(&avlw->root, data);
}

// возвращает -1 если элемента нет, и 0 - если успешно удалён.
int avlw_remove(struct avl_wrapper *avlw, int data)
{
    return avl_remove(avlw->root, data);
}

// NULL - если не найдено.
struct avl *avlw_find(struct avl_wrapper *avlw, int data)
{
    return avl_find(avlw->root, data);
}

static void __prn_rec(FILE *file, struct avl *node, int tab)
{
    if (node != NULL)
    {
        __prn_rec(file, node->left, tab + 1);

        for (int i = 0; i < tab; i++)
            fprintf(file, "%*s", 9, "");
        fprintf(file, "%2d[%2d|%2d]\n", node->data, node->depth, node->diff);

        __prn_rec(file, node->right, tab + 1);
    }
}

void avlw_fprintf(FILE *file, struct avl_wrapper *avlw)
{
    __prn_rec(file, avlw->root, 0);
}
