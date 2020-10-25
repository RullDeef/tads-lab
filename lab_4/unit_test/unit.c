#include <stdio.h>
#include "dense_matrix.h"
#include "sparse_matrix.h"
#include "converter.h"

int main(void)
{
    dense_matrix_t dn_mat = dn_create(3, 5);
    dn_set(&dn_mat, 0, 1, 1);
    dn_set(&dn_mat, 2, 3, 2);
    dn_set(&dn_mat, 1, 2, 3);
    dn_set(&dn_mat, 0, 3, 4);
    dn_set(&dn_mat, 2, 4, 5);

    for (uint32_t row = 0; row < 3; row++)
    {
        for (uint32_t col = 0; col < 5; col++)
            printf("%4d", dn_get(&dn_mat, row, col));
        printf("\n");
    }
    printf("\n");

    dense_matrix_t dn_mat2 = dn_create(5, 4);
    dn_set(&dn_mat2, 4, 0, 1);
    dn_set(&dn_mat2, 0, 1, 2);
    dn_set(&dn_mat2, 3, 0, 3);
    dn_set(&dn_mat2, 2, 1, 4);
    dn_set(&dn_mat2, 1, 2, 5);
    dn_set(&dn_mat2, 2, 3, 6);
    dn_set(&dn_mat2, 3, 1, 7);

    for (uint32_t row = 0; row < 5; row++)
    {
        for (uint32_t col = 0; col < 4; col++)
            printf("%4d", dn_get(&dn_mat2, row, col));
        printf("\n");
    }
    printf("\n");

    dense_matrix_t res = dn_null_matrix();
    
    printf("dense:\n");
    if (dn_mult_matrix(&dn_mat, &dn_mat2, &res))
        printf("error!\n");
    else
    {
        for (uint32_t row = 0; row < 3; row++)
        {
            for (uint32_t col = 0; col < 4; col++)
                printf("%4d", dn_get(&res, row, col));
            printf("\n");
        }
        printf("\n");
    }

    {
        sparse_matrix_t sp_1 = cv_sparse_from_dense(&dn_mat);
        sparse_matrix_t sp_2 = cv_sparse_from_dense(&dn_mat2);

        sparse_matrix_t res = sp_null_matrix();
        sp_transpose(&sp_1);
        printf("sparse:\n");
        if (sp_mult_matrix(&sp_1, &sp_2, &res))
            printf("error!\n");
        else
        {
            dense_matrix_t rd = cv_dense_from_sparse(&res);
            for (uint32_t row = 0; row < 3; row++)
            {
                for (uint32_t col = 0; col < 4; col++)
                    printf("%4d", dn_get(&rd, row, col));
                printf("\n");
            }
            printf("\n");
        }

        sp_free(&sp_1);
        sp_free(&sp_2);
    }

    dn_free(&dn_mat);
    dn_free(&dn_mat2);
    dn_free(&res);
    return 0;
}
