#include <stdio.h>
#include "../include/sparse_matrix.h"

void print_info(sparse_matrix_t *mat)
{
    printf("[MATRIX INFO]\n");
    printf("data: ");
    for (int i = 0; i < mat->nonzero_size; i++)
        printf("%d, ", mat->nonzero_array[i]);
    printf("\n");

    printf("rows: ");
    for (int i = 0; i < mat->nonzero_size; i++)
        printf("%ld, ", mat->rows[i]);
    printf("\n");

    printf("cols: ");
    for (int i = 0; i < mat->cols_size; i++)
        printf("%ld, ", mat->cols[i]);
    printf("\n");
    printf("[END INFO]\n\n");
}

int main(void)
{
    sparse_matrix_t mat = sp_create(5, 5);
    sp_print(&mat);

    print_info(&mat);

    printf("\nelement at row=3 col=0 : %d\n", sp_get(&mat, 3, 0));
    printf("set it to 8!\n\n");

    sp_set(&mat, 1, 0, 10);
    sp_set(&mat, 2, 2, 22);
    sp_set(&mat, 3, 0, 30);
    sp_print(&mat);

    print_info(&mat);

    return 0;
}