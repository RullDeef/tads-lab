#ifndef __BST_WRAPPER_H_
#define __BST_WRAPPER_H_

#include <stdio.h>
#include "core/bst/bst.h"

struct bst_wrapper
{
    struct bst *root;
};

struct bst_wrapper bstw_create(void);
void bstw_destroy(struct bst_wrapper *tree);

int bstw_insert(struct bst_wrapper *tree, int data);

// возвращает -1 если элемента нет, и 0 - если успешно удалён.
int bstw_remove(struct bst_wrapper *tree, int data);

// NULL - если не найдено.
struct bst *bstw_find(struct bst_wrapper *tree, int data);

int bstw_fscanf(FILE *file, struct bst_wrapper *tree);
void bstw_fprintf(FILE *file, struct bst_wrapper *tree);

size_t bstw_calc_size(struct bst_wrapper *tree);

#endif
