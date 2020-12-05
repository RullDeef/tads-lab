#include <stdio.h>
#include <stdbool.h>
#include "avl_wrapper.h"
#include "utils/colors.h"
#include "utils/logger.h"

#define PRN_DATA_LEN 6
#define PRN_COLOR CLR_BR_GREEN_U

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

int avlw_fscanf(FILE *file, struct avl_wrapper *tree)
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
        *tree = avlw_create();

        rewind(file);
        while (fscanf(file, "%d", &num) != EOF)
            avlw_insert(tree, num);
    }

    return status;
}

static void __prn_rec(FILE *file, struct avl *node, int tab, bool is_left, bool is_right, unsigned int notch)
{
    if (node != NULL && node->depth > 0)
    {
        __prn_rec(file, node->left, tab + 1, true, false,
            (is_left ? (notch << 1) : (notch << 1) | 1));

        for (int i = 0; i < tab; i++)
        {
            if ((i != 0) && (notch & (1U << (tab - i - 1))))
                fprintf(file, "|");
            else
                fprintf(file, " ");
            fprintf(file, "%*s", PRN_DATA_LEN, "");
        }
        fprintf(file, "%s", (is_left ? "╭" : (is_right ? "╰" : " ")));

        int n = snprintf(NULL, 0, "[%d]", node->data);
        fprintf(file, "%.*s", PRN_DATA_LEN - n, "------------------");
        fprintf(file, PRN_COLOR "[%d]" CLR_RESET, node->data);

        if (node->left != NULL && node->right != NULL)
            fprintf(file, "┥");
        else if (node->left != NULL)
            fprintf(file, "┘");
        else if (node->right != NULL)
            fprintf(file, "┐");
        fprintf(file, "\n");

        __prn_rec(file, node->right, tab + 1, false, true,
            (is_right ? (notch << 1) : (notch << 1) | 1));
    }
}

void avlw_fprintf(FILE *file, struct avl_wrapper *avlw)
{
    __prn_rec(file, avlw->root, 0, false, false, 0U);
}
