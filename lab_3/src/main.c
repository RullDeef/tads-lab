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

/*
void test(void)
{
    sparse_matrix_t mat = sp_create(5, 7);
    mat_elem_t value;

    sp_set(&mat, 2, 4, 6);
    value = sp_get(&mat, 2, 2);

    printf("inserted value 6\n");
    sp_print_info(&mat);
    sp_print(&mat);
    printf("\n\n");

    sp_set(&mat, 3, 1, 5);
    value = sp_get(&mat, 3, 1);

    printf("inserted value 5\n");
    sp_print_info(&mat);
    sp_print(&mat);
    printf("\n\n");

    sp_set(&mat, 0, 2, 3);
    value = sp_get(&mat, 0, 2);

    printf("inserted value 3\n");
    sp_print_info(&mat);
    sp_print(&mat);
    printf("\n\n");

    sp_set(&mat, 0, 5, 7);
    value = sp_get(&mat, 0, 5);

    printf("inserted value 7\n");
    sp_print_info(&mat);
    sp_print(&mat);
    printf("\n\n");

    sp_set(&mat, 1, 4, 1);
    value = sp_get(&mat, 1, 4);

    printf("inserted value 1\n");
    sp_print_info(&mat);
    sp_print(&mat);
    printf("\n\n");

    sp_set(&mat, 2, 0, 2);
    value = sp_get(&mat, 2, 0);

    printf("inserted value 2\n");
    sp_print_info(&mat);
    sp_print(&mat);
    printf("\n\n");

    sp_set(&mat, 3, 5, 4);
    value = sp_get(&mat, 3, 5);

    printf("inserted value 4\n");
    sp_print_info(&mat);
    sp_print(&mat);
    printf("\n\n");

    sp_free(&mat);
}
*/

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