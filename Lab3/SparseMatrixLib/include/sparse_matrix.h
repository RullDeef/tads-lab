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
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>

typedef long ssize_t;

typedef int mat_elem_t;
typedef ssize_t mat_index_t;
typedef ssize_t mat_size_t;

#define SP_INITIAL_ALLOC_SIZE 10
#define SP_ALLOC_MULTIPILER 1.5

#define INVALID_SIZE 0
#define INVALID_INDEX -1

// параметры, при конвертации матричных индексов в линейный
#define NO_NEED_ADJUST 0
#define NEED_ADJUST 1
#define NEED_START_COL 2

// для возврата ошибки при умножении неумножаемых матриц
#define BAD_DIMENSIONS -2
#define BAD_ALLOC -3

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
    mat_size_t __alloc_nz_sz;
    mat_size_t __alloc_cl_sz;
    mat_index_t *cols; // JA
    mat_index_t *rows; // IA
    mat_elem_t *nonzero_array; // A
} sparse_matrix_t;

sparse_matrix_t sp_null_matrix();
sparse_matrix_t sp_create(mat_size_t rows, mat_size_t cols);
int sp_recreate(sparse_matrix_t *matrix, mat_size_t rows, mat_size_t cols);
// int sp_resize(sparse_matrix_t* matrix, mat_size_t new_rows, mat_size_t new_cols);
void sp_free(sparse_matrix_t* matrix);

bool sp_mat_is_null(const sparse_matrix_t* matrix);

mat_elem_t sp_get(const sparse_matrix_t* matrix, mat_index_t row, mat_index_t col);
void sp_set(sparse_matrix_t* matrix, mat_index_t row, mat_index_t col, mat_elem_t value);

int sp_mult_by_vector(const sparse_matrix_t* matrix, const mat_elem_t* vector, mat_elem_t* out);
int sp_mult_matrix(const sparse_matrix_t* matrix_1, const sparse_matrix_t* matrix_2, sparse_matrix_t* out);

void sp_print_info(const sparse_matrix_t* matrix);
void sp_print(const sparse_matrix_t* matrix);

#endif
