#include "cheat.h"
#include "cheats.h"
#include "../../include/sparse_matrix.h"

#define cheat_assert_mat(mat)                                                               \
    cheat_assert((mat) != NULL);                                                            \
    cheat_assert((mat)->rows_size > 0 && (mat)->cols_size > 0 && (mat)->nonzero_size >= 0); \
    cheat_assert((mat)->rows != NULL && (mat)->cols != NULL && (mat)->nonzero_array != NULL)


CHEAT_TEST(mat_creation,
    for (int col = 1; col < 10; col++)
    {
        for (int row = 1; row < 10; row++)
        {
            sparse_matrix_t mat = sp_create(row, col);
            cheat_assert_mat(&mat);
            sp_free(&mat);
        }
    }
)

CHEAT_TEST(mat_getter,
    sparse_matrix_t mat = (sparse_matrix_t) {
        .cols_size = 7,
        .rows_size = 5,
        .nonzero_size = 7,
        .nonzero_array = malloc(7 * sizeof (mat_elem_t)),
        .rows = malloc(7 * sizeof(mat_index_t)),
        .cols = malloc(8 * sizeof(mat_index_t))
    };

    mat.nonzero_array[0] = 2;
    mat.nonzero_array[1] = 5;
    mat.nonzero_array[2] = 3;
    mat.nonzero_array[3] = 6;
    mat.nonzero_array[4] = 1;
    mat.nonzero_array[5] = 7;
    mat.nonzero_array[6] = 4;

    mat.rows[0] = 2;
    mat.rows[1] = 3;
    mat.rows[2] = 0;
    mat.rows[3] = 2;
    mat.rows[4] = 1;
    mat.rows[5] = 0;
    mat.rows[6] = 3;

    mat.cols[0] = 0;
    mat.cols[1] = 1;
    mat.cols[2] = 2;
    mat.cols[3] = 4;
    mat.cols[4] = 4;
    mat.cols[5] = 5;
    mat.cols[6] = 7;
    mat.cols[7] = 7;

    mat_elem_t value = sp_get(&mat, 0, 2);
    cheat_assert_int(value, 3);

    value = sp_get(&mat, 0, 5);
    cheat_assert_int(value, 7);

    value = sp_get(&mat, 1, 4);
    cheat_assert_int(value, 1);

    value = sp_get(&mat, 2, 0);
    cheat_assert_int(value, 2);

    value = sp_get(&mat, 2, 2);
    cheat_assert_int(value, 6);

    value = sp_get(&mat, 3, 1);
    cheat_assert_int(value, 5);

    value = sp_get(&mat, 3, 5);
    cheat_assert_int(value, 4);

    // sp_print_info(&mat);
    // sp_print(&mat);
    sp_free(&mat);
)

CHEAT_TEST(mat_setter,
    sparse_matrix_t mat = sp_create(5, 7);
    mat_elem_t value;

    sp_set(&mat, 2, 2, 6);
    value = sp_get(&mat, 2, 2);
    cheat_assert_int(value, 6);

    // printf("inserted value 6\n");
    // sp_print_info(&mat);
    // sp_print(&mat);
    // printf("\n\n");

    sp_set(&mat, 3, 1, 5);
    value = sp_get(&mat, 3, 1);
    cheat_assert_int(value, 5);

    // printf("inserted value 5\n");
    // sp_print_info(&mat);
    // sp_print(&mat);
    // printf("\n\n");

    sp_set(&mat, 0, 2, 3);
    value = sp_get(&mat, 0, 2);
    cheat_assert_int(value, 3);

    // printf("inserted value 3\n");
    // sp_print_info(&mat);
    // sp_print(&mat);
    // printf("\n\n");

    sp_set(&mat, 0, 5, 7);
    value = sp_get(&mat, 0, 5);
    cheat_assert_int(value, 7);

    // printf("inserted value 7\n");
    // sp_print_info(&mat);
    // sp_print(&mat);
    // printf("\n\n");

    sp_set(&mat, 1, 4, 1);
    value = sp_get(&mat, 1, 4);
    cheat_assert_int(value, 1);

    // printf("inserted value 1\n");
    // sp_print_info(&mat);
    // sp_print(&mat);
    // printf("\n\n");

    sp_set(&mat, 2, 0, 2);
    value = sp_get(&mat, 2, 0);
    cheat_assert_int(value, 2);

    // printf("inserted value 2\n");
    // sp_print_info(&mat);
    // sp_print(&mat);
    // printf("\n\n");

    sp_set(&mat, 3, 5, 4);
    value = sp_get(&mat, 3, 5);
    cheat_assert_int(value, 4);

    // printf("inserted value 4\n");
    // sp_print_info(&mat);
    // sp_print(&mat);
    // printf("\n\n");

    sp_free(&mat);
)
