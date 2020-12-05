#include <stdbool.h>
#include "utils/colors.h"
#include "utils/logger.h"
#include "bst_wrapper.h"

#define PRN_DATA_LEN 6
#define PRN_COLOR CLR_BR_GREEN_U

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

static void __prn_rec(FILE *file, struct bst *node, int tab, bool is_left, bool is_right, unsigned int notch)
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

void bstw_fprintf(FILE *file, struct bst_wrapper *bstw)
{
    __prn_rec(file, bstw->root, 0, false, false, 0U);
}
