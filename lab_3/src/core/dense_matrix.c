#include <stdlib.h>
#include <memory.h>
#include <assert.h>
#include "dense_matrix.h"

dense_matrix_t dn_null_matrix(void)
{
    dense_matrix_t matrix = {
        .cols_size = 0,
        .rows_size = 0,
        .data = NULL};

    return matrix;
}

dense_matrix_t dn_create(uint32_t rows, uint32_t cols)
{
    dense_matrix_t matrix = dn_null_matrix();

    if (rows > 0 && cols > 0)
    {
        matrix.rows_size = rows;
        matrix.cols_size = cols;
        matrix.data = (int **)malloc(rows * sizeof(int *));
        if (matrix.data == NULL)
            matrix = dn_null_matrix();
        else
        {
            bool bad_alloc = false;
            uint32_t row = 0;
            for (; row < rows; row++)
            {
                matrix.data[row] = calloc(cols, sizeof(int));
                if (matrix.data[row] == NULL)
                {
                    bad_alloc = true;
                    break;
                }
            }
            if (bad_alloc)
            {
                while (row-- != 0)
                    free(matrix.data[row]);
                free(matrix.data);
                matrix = dn_null_matrix();
            }
        }
    }

    return matrix;
}

void dn_free(dense_matrix_t *matrix)
{
    if (dn_mat_is_null(matrix))
        return;

    for (uint32_t row = 0; row < matrix->rows_size; row++)
        free(matrix->data[row]);
    free(matrix->data);
    *matrix = dn_null_matrix();
}

bool dn_mat_is_null(const dense_matrix_t *matrix)
{
    return matrix->data == NULL;
}

int dn_get(const dense_matrix_t *matrix, uint32_t row, uint32_t col)
{
    assert(!dn_mat_is_null(matrix));
    assert(row < matrix->rows_size && col < matrix->cols_size);

    return matrix->data[row][col];
}

void dn_set(dense_matrix_t *matrix, uint32_t row, uint32_t col, int value)
{
    assert(!dn_mat_is_null(matrix));
    assert(row < matrix->rows_size && col < matrix->cols_size);

    matrix->data[row][col] = value;
}

void dn_clear(dense_matrix_t *matrix)
{
    assert(!dn_mat_is_null(matrix));

    for (uint32_t row = 0; row < matrix->rows_size; row++)
        memset(matrix->data[row], 0, matrix->cols_size * sizeof(int));
}

void dn_randomize(dense_matrix_t *matrix, float nz_percent)
{
    assert(!dn_mat_is_null(matrix));

    uint64_t nonzero_amount = nz_percent * matrix->rows_size * matrix->cols_size;
    for (uint64_t i = 0; i < nonzero_amount; i++)
    {
        uint32_t row = ((float)rand() / (RAND_MAX + 1L)) * matrix->rows_size;
        uint32_t col = ((float)rand() / (RAND_MAX + 1L)) * matrix->cols_size;

        int value = rand() % 99 - 49;
        if (value <= 0)
            value--;
        matrix->data[row][col] = value;
    }
}

int dn_mult_matrix(const dense_matrix_t *matrix_1, const dense_matrix_t *matrix_2, dense_matrix_t *out)
{
    assert(!dn_mat_is_null(matrix_1) && !dn_mat_is_null(matrix_2));

    if (matrix_1->cols_size != matrix_2->rows_size)
        return EXIT_FAILURE;

    if (dn_mat_is_null(out) || out->rows_size != matrix_1->rows_size || out->cols_size != matrix_2->cols_size)
    {
        dn_free(out);
        *out = dn_create(matrix_1->rows_size, matrix_2->cols_size);
    }
    else
        dn_clear(out);

    for (uint32_t row = 0; row < out->rows_size; row++)
    {
        for (uint32_t col = 0; col < out->cols_size; col++)
        {
            int sum = 0;
            for (uint32_t mid = 0; mid < matrix_1->cols_size; mid++)
                sum += matrix_1->data[row][mid] * matrix_2->data[mid][col];
            out->data[row][col] = sum;
        }
    }

    return EXIT_SUCCESS;
}
