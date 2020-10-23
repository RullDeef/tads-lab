/*

Разреженная матрица хранится в форме 3-х обьектов:

    - вектор A содержит значения ненулевых элементов
    - вектор IA содержит номера строк для элементов вектора A
    - связный список JA, в элементе Nk которого находится номер компонент в A и IA,
        с которых начинается описание столбца Nk матрицы A

    (Йельский формат)

NULL-матрица детерминируется по значению указателя на список столбцов.

Корректная матрица проверяется утверждением и следующими условиями:
    0. Она не является NULL-матрицей
    1. Имеет положительный размер как строк, так и столбцов
    2. При положительном числе ненулевых элементов имеет ненулевые указатели
        на массив ненулевых элементов и список их строк.
*/

#ifndef __SPARSE_MATRIX_H_
#define __SPARSE_MATRIX_H_

#include <stddef.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <assert.h>

typedef int mat_elem_t;

#define SP_INITIAL_ALLOC_SIZE 10
#define SP_ALLOC_MULTIPILER 1.5

// параметры, при конвертации матричных индексов в линейный
#define NO_NEED_ADJUST 0
#define NEED_ADJUST 1
#define NEED_START_COL 2

// для возврата ошибки при умножении неумножаемых матриц
#define BAD_DIMENSIONS -2
#define BAD_ALLOC -3

#ifndef NDEBUG
#define assert_sp_mat(mat) assert(                                      \
    (mat).cols != NULL && (mat).rows_size > 0 && (mat).cols_size > 0 && \
    ((mat).nonzero_size == 0 || ((mat).nonzero_array != NULL && (mat).rows != NULL)))
#else
#define assert_sp_mat(mat) (void *)0
#endif

typedef struct sparse_matrix_t
{
    uint32_t rows_size;
    uint32_t cols_size;
    uint32_t nonzero_size;
    uint32_t __alloc_nz_sz;
    uint32_t __alloc_cl_sz;
    uint32_t *cols;            // JA
    uint32_t *rows;            // IA
    mat_elem_t *nonzero_array; // A
} sparse_matrix_t;

sparse_matrix_t sp_null_matrix(void);
sparse_matrix_t sp_create(uint32_t rows, uint32_t cols);
int sp_recreate(sparse_matrix_t *matrix, uint32_t rows, uint32_t cols);
// int sp_resize(sparse_matrix_t* matrix, uint32_t new_rows, uint32_t new_cols);
void sp_free(sparse_matrix_t *matrix);

sparse_matrix_t sp_copy(const sparse_matrix_t *matrix);

bool sp_mat_is_null(const sparse_matrix_t *matrix);

mat_elem_t sp_get(const sparse_matrix_t *matrix, uint32_t row, uint32_t col);
void sp_set(sparse_matrix_t *matrix, uint32_t row, uint32_t col, mat_elem_t value);

/**
 * @brief Пытается сжать матрицу, удалив нулевые элементы из неё.
 * 
 * При передаче NULL-матрицы ничего не делает.
 * 
 * @param matrix - сжимаемая матрица.
 */
void sp_compress(sparse_matrix_t *matrix);

/**
 * @brief Обнуляет матрицу.
 */
void sp_clear(sparse_matrix_t *matrix);
void sp_randomize(sparse_matrix_t *matrix, float nz_percent);

void sp_transpose(sparse_matrix_t *matrix);

int sp_mult_matrix(const sparse_matrix_t *matrix_1, const sparse_matrix_t *matrix_2, sparse_matrix_t *out);

void sp_print_info(const sparse_matrix_t *matrix);
void sp_print(const sparse_matrix_t *matrix);

#endif
