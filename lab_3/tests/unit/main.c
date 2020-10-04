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
    sparse_matrix_t mat = sp_create(5, 5);

    for (int row = 0; row < 5; row++)
    for (int col = 0; col < 5; col++)
    {
        int value = sp_get(&mat, 0, 1);
        cheat_assert_int(value, 0);
    }

    sp_free(&mat);
)

CHEAT_TEST(mat_setter,
    sparse_matrix_t mat = sp_create(5, 3);
    mat_elem_t value;

    sp_set(&mat, 0, 1, 1);
    value = sp_get(&mat, 0, 1);
    cheat_assert_int(value, 1);

    sp_set(&mat, 1, 0, 2);
    value = sp_get(&mat, 1, 0);
    cheat_assert_int(value, 2);

    sp_set(&mat, 2, 2, 3);
    value = sp_get(&mat, 2, 2);
    cheat_assert_int(value, 3);

    sp_set(&mat, 4, 1, 4);
    value = sp_get(&mat, 4, 1);
    cheat_assert_int(value, 4);

    sp_set(&mat, 3, 1, 5);
    value = sp_get(&mat, 3, 1);
    cheat_assert_int(value, 5);

    sp_free(&mat);
)
