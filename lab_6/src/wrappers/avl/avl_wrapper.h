#ifndef __AVL_WRAPPER_H_
#define __AVL_WRAPPER_H_

#include <stdio.h>
#include "core/avl/avl.h"

struct avl_wrapper
{
    struct avl *root;
};

struct avl_wrapper avlw_create(void);
void avlw_destroy(struct avl_wrapper *tree);

int avlw_insert(struct avl_wrapper *tree, int data, int *cmp);
int avlw_shallow_insert(struct avl_wrapper *tree, int data);

// возвращает -1 если элемента нет, и 0 - если успешно удалён.
int avlw_remove(struct avl_wrapper *tree, int data);

// NULL - если не найдено.
struct avl *avlw_find(struct avl_wrapper *tree, int data, int *cmp);

int avlw_fscanf(FILE *file, struct avl_wrapper *tree);
void avlw_fprintf(FILE *file, struct avl_wrapper *tree);

size_t avlw_calc_size(struct avl_wrapper *tree);

float avlw_calc_mean_cmp_amount(struct avl_wrapper *tree);

#endif