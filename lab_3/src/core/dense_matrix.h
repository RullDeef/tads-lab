#ifndef __DENSE_MATRIX_H_
#define __DENSE_MATRIX_H_

#include <stdint.h>
#include <stdbool.h>

#ifndef __MAT_ELEM_DEFINED
#define __MAT_ELEM_DEFINED
typedef int mat_elem_t;
#endif

#define DN_INITIAL_ALLOC_SIZE 25
#define DN_ALLOC_MULTIPILER 1.3

typedef struct
{
    uint32_t rows_size;
    uint32_t cols_size;
    mat_elem_t **data;
} dense_matrix_t;

dense_matrix_t dn_null_matrix(void);
dense_matrix_t dn_create(uint32_t rows, uint32_t cols);
void dn_free(dense_matrix_t *matrix);

bool dn_mat_is_null(const dense_matrix_t *matrix);

mat_elem_t dn_get(const dense_matrix_t *matrix, uint32_t row, uint32_t col);
void dn_set(dense_matrix_t *matrix, uint32_t row, uint32_t col, mat_elem_t value);

/**
 * @brief Обнуляет матрицу.
 */
void dn_clear(dense_matrix_t *matrix);
void dn_randomize(dense_matrix_t *matrix, float nz_percent);

int dn_mult_matrix(const dense_matrix_t *matrix_1, const dense_matrix_t *matrix_2, dense_matrix_t *out);

#endif
