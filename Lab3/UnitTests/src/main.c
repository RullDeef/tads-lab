#include <stdio.h>
#include "sparse_matrix.h"

int main(void)
{
    sparse_matrix_t mat = sp_create(10, 10);

    /* target matrix:
        0 0 0 0 0 0 2 0 0 0
        0 1 0 0 0 0 0 0 0 0
       10 0 0 0 0 0 7 0 0 0
        0 0 9 0 0 0 0 8 0 0
        0 0 4 0 0 0 0 0 0 0
        0 0 0 0 0 0 0 0 5 0
        0 6 0 0 0 0 0 0 0 0
        0 0 0 0 3 0 0 0 0 0
        0 0 0 0 0 0 0 0 0 0
        0 0 0 0 0 0 0 0 0 0
    */
    sp_set(&mat, 1, 1, 1);
    sp_set(&mat, 0, 6, 2);
    sp_set(&mat, 7, 4, 3);
    sp_set(&mat, 4, 2, 4);
    sp_set(&mat, 5, 8, 5);
    sp_set(&mat, 6, 1, 6);
    sp_set(&mat, 2, 6, 7);
    sp_set(&mat, 3, 7, 8);
    sp_set(&mat, 3, 2, 9);
    sp_set(&mat, 2, 0, 10);

    // add random zeros
    sp_set(&mat, 7, 4, 0);
    sp_set(&mat, 5, 8, 0);
    sp_set(&mat, 2, 6, 0);
    sp_set(&mat, 3, 2, 0);

    sp_print(&mat);
    sp_print_info(&mat);
    printf("allocated memory for 10 elems was: %ld\n", mat.__alloc_nz_sz);

    printf("-COMPRESSION-------------------------\n");
    sp_compress(&mat);
    printf("-COMPRESSION-------------------------\n");

    sp_print(&mat);
    sp_print_info(&mat);
    printf("allocated memory for 10 elems become: %ld\n", mat.__alloc_nz_sz);
};
