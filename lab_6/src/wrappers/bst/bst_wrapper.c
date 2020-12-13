#include <stdbool.h>
#include "utils/colors.h"
#include "utils/logger.h"
#include "bst_wrapper.h"

#define PRN_DATA_LEN 6
#define PRN_COLOR CLR_BR_GREEN

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
    int num;

    *tree = bstw_create();

    rewind(file);
    while (status == 0 && fscanf(file, "%d", &num) != EOF)
        status = bstw_insert(tree, num);

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
                fprintf(file, "│");
            else
                fprintf(file, " ");
            fprintf(file, "%*s", PRN_DATA_LEN, "");
        }
        fprintf(file, "%s", (is_left ? "╭" : (is_right ? "╰" : " ")));

        int n = snprintf(NULL, 0, "[%d]", node->data);
        fprintf(file, "%.*s", (PRN_DATA_LEN - n) * 3, "──────────────");
        fprintf(file, PRN_COLOR " %d " CLR_RESET, node->data);

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
    fprintf(file, "ДДП:\n");
    __prn_rec(file, bstw->root, 0, false, false, 0U);

    fprintf(file, "Размер структуры: " PRN_COLOR "%lu" CLR_RESET " байт.\n", bstw_calc_size(bstw));
    fprintf(file, "Среднее число сравнений в структуре: " PRN_COLOR "%.2f" CLR_RESET ".\n\n", bstw_calc_mean_cmp_amount(bstw));
}

static size_t __calc_node_size(struct bst *node)
{
    if (node == NULL)
        return 0U;
    else
        return sizeof(struct bst) + __calc_node_size(node->left) + __calc_node_size(node->right);
}

size_t bstw_calc_size(struct bst_wrapper *tree)
{
    return __calc_node_size(tree->root);
}

static int __calc_mean_cmp_amount(struct bst *node, int *count)
{
    if (!node)
        return 0;

    int left_amount = 0;
    int right_amount = 0;
    int cmps = 1;

    if (node->left)
    {
        int left_cmps = __calc_mean_cmp_amount(node->left, &left_amount);
        cmps += left_cmps + left_amount;
    }

    if (node->right)
    {
        int right_cmps = __calc_mean_cmp_amount(node->right, &right_amount);
        cmps += right_cmps + right_amount;
    }

    *count = 1 + left_amount + right_amount;
    return cmps;
}

float bstw_calc_mean_cmp_amount(struct bst_wrapper *tree)
{
    int count = 0;
    int cmps = __calc_mean_cmp_amount(tree->root, &count);
    return count == 0 ? 0.0f : (float)cmps / count;
}
