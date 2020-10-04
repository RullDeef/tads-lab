/*

Разреженная матрица хранится в форме 3-х обьектов:

    - вектор A содержит значения ненулевых элементов
    - вектор IA содержит номера строк для элементов вектора A
    - связный список JA, в элементе Nk которого находится номер компонент в A и IA,
        с которых начинается описание столбца Nk матрицы A

    (Йельский формат)
*/

#ifndef __SPARSE_MATRIX_H_
#define __SPARSE_MATRIX_H_

#include <stddef.h>
#include <assert.h>

typedef int mat_elem_t;
typedef ssize_t mat_index_t;
typedef ssize_t mat_size_t;

#define INVALID_SIZE 0
#define INVALID_INDEX -1

// параметры, при конвертации матричных индексов в линейный
#define NO_NEED_ADJUST 0
#define NEED_ADJUST 1
#define NEED_START_COL 2

// для возврата ошибки при умножении неумножаемых матриц
#ifndef EXIT_SUCCESS
#define EXIT_SUCCESS 0
#endif
#ifndef EXIT_FAILURE
#define EXIT_FAILURE !EXIT_SUCCESS
#endif
#define BAD_DIMENSIONS -2

#ifndef NDEBUG
#define assert_sp_mat(mat) \
    assert((mat) != NULL); \
    assert((mat)->rows_size > 0 && (mat)->cols_size > 0 && (mat)->nonzero_size >= 0); \
    assert((mat)->rows != NULL && (mat)->cols != NULL && (mat)->nonzero_array != NULL)
#else
#define assert_sp_mat(mat) (void*)0
#endif

typedef struct sparse_matrix_t
{
    mat_size_t rows_size;
    mat_size_t cols_size;
    mat_size_t nonzero_size;
    mat_index_t *cols; // JA
    mat_index_t *rows; // IA
    mat_elem_t *nonzero_array; // A
} sparse_matrix_t;

sparse_matrix_t sp_create(mat_size_t rows, mat_size_t cols);
void sp_free(sparse_matrix_t *matrix);

mat_elem_t sp_get(const sparse_matrix_t *matrix, mat_index_t row, mat_index_t col);
void sp_set(sparse_matrix_t *matrix, mat_index_t row, mat_index_t col, mat_elem_t value);

int sp_mult_by_vector(const sparse_matrix_t *matrix, const mat_elem_t *vector, mat_elem_t *out);
int sp_mult_matrix(const sparse_matrix_t *matrix_1, const sparse_matrix_t *matrix_2, sparse_matrix_t *out);

void sp_print_info(const sparse_matrix_t *matrix);
void sp_print(const sparse_matrix_t *matrix);

#endif
