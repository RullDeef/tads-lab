#include <stdio.h>
#include <memory.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>
#include "sparse_matrix.h"

#define PRINT_ELEM_SIZE 3

sparse_matrix_t sp_null_matrix()
{
    sparse_matrix_t matrix = {
        .rows_size = 0,
        .cols_size = 0,
        .nonzero_size = 0,
        .__alloc_nz_sz = 0,
        .__alloc_cl_sz = 0,
        .cols = NULL,
        .rows = NULL,
        .nonzero_array = NULL
    };
    return matrix;
}

sparse_matrix_t sp_create(mat_size_t rows, mat_size_t cols)
{
    if (rows <= 0 || cols <= 0)
        return sp_null_matrix();

    sparse_matrix_t matrix;

    matrix.rows_size = rows;
    matrix.cols_size = cols;
    matrix.nonzero_size = 0;

    matrix.__alloc_nz_sz = SP_INITIAL_ALLOC_SIZE * sizeof(mat_elem_t);
    matrix.nonzero_array = calloc(SP_INITIAL_ALLOC_SIZE, sizeof(mat_elem_t));
    matrix.rows = calloc(SP_INITIAL_ALLOC_SIZE, sizeof(mat_index_t));

    // "fixed" size space allocation
    matrix.__alloc_cl_sz = (cols + 1) * SP_ALLOC_MULTIPILER;
    matrix.cols = calloc(matrix.__alloc_cl_sz, sizeof(mat_index_t));

    matrix.cols[0] = 0;
    for (mat_index_t i = 1; i < cols + 1; i++)
        matrix.cols[i] = 0;

    return matrix;
}

int sp_recreate(sparse_matrix_t* matrix, mat_size_t rows, mat_size_t cols)
{
    sp_free(matrix);
    *matrix = sp_create(rows, cols);
}

/*
int sp_resize(sparse_matrix_t* matrix, mat_size_t new_rows, mat_size_t new_cols)
{
    int status = EXIT_SUCCESS;

    if (sp_mat_is_null(matrix))
    {
        *matrix = sp_create(new_rows, new_cols);
        if (sp_mat_is_null(matrix))
            status = BAD_ALLOC;
    }
    else if (matrix->rows != new_rows || matrix->cols != new_cols)
    {
        // realloc if need
        if (matrix->__alloc_cl_sz < new_cols + 1)
        {
            mat_size_t new_alloc_cl_sz = (new_cols + 1) * SP_ALLOC_MULTIPILER;
            mat_index_t* new_cols_ptr = realloc(matrix->cols, new_alloc_cl_sz);
            if (new_cols_ptr == NULL)
                status = BAD_ALLOC;
            else
            {
                matrix->cols = new_cols_ptr;
                matrix->__alloc_cl_sz = new_alloc_cl_sz;

                for (mat_index_t i = matrix->cols_size + 1; i < new_cols + 1; i++)
                    matrix->cols[i] = matrix->cols[matrix->cols_size];

                // update size variables
                matrix->rows_size = new_rows;
                matrix->cols_size = new_cols;
                // ...
            }
        }
    }

    return status;
}
*/

void sp_free(sparse_matrix_t *matrix)
{
    if (matrix == NULL)
        return;

    free(matrix->rows);
    matrix->rows = NULL;

    free(matrix->cols);
    matrix->cols = NULL;

    free(matrix->nonzero_array);
    matrix->nonzero_array = NULL;

    matrix->rows_size = INVALID_SIZE;
    matrix->cols_size = INVALID_SIZE;
    matrix->nonzero_size = INVALID_SIZE;
}

bool sp_mat_is_null(const sparse_matrix_t* matrix)
{
    return matrix->nonzero_array != NULL;
}

/**
 * @brief проверяет столбец на наличие элементов.
 */
static bool imp__has_values_in_col(const sparse_matrix_t *matrix, mat_index_t col)
{
    assert_sp_mat(matrix);
    assert(0 <= col && col < matrix->cols_size);

    return matrix->cols[col] != matrix->cols[col + 1];
}

/**
 * @brief Определяет линейный индекс в разреженной
 * матрице по заданным строке и столбцу.
 * 
 * Устанавливает в index значение индекса найденного
 * элемента или индекс для вставки нового элемента
 * 
 * @return  NO_NEED_ADJUST - элемент есть
 *          NEED_ADJUST - необходимо вставить элемент
 *          NEED_START_COL - необходимо вставить элемент
 *              и начать с него колонку col
 */
static int imp__find_linear_index(const sparse_matrix_t *matrix, mat_index_t row, mat_index_t col, mat_index_t *index)
{
    assert_sp_mat(matrix);
    assert(0 <= row && row < matrix->rows_size);
    assert(0 <= col && col < matrix->cols_size);

    // поиск индекса происходит в рамках заданного столбца
    // проверим, не пуст ли данный столбец
    if (!imp__has_values_in_col(matrix, col))
    {
        // найди последний доступный индекс для вставки
        while (col < matrix->cols_size && !imp__has_values_in_col(matrix, col))
            col++;

        // если не дошли до конца матрицы
        if (col < matrix->cols_size)
            *index = matrix->cols[col]; // берем индекс первого в найденном непустом столбце
        else
            *index = matrix->nonzero_size;

        return NEED_START_COL; // надо начать новую колонку
    }

    // первый линейный индекс, с которого будет проходить поиск
    *index = matrix->cols[col];

    // подогнать col под следующий непустой столбец
    do
    {
        col++;
    } while (col < matrix->cols_size && !imp__has_values_in_col(matrix, col));

    // пока индекс нам не подходит переходим к следующему
    while (
        // не вышли за границу доступного
        *index < matrix->nonzero_size &&
        // мы не соскочили с текущего столбца
        ((col < matrix->cols_size && *index < matrix->cols[col]) ||
         col == matrix->cols_size) &&
        // номер строки элемента по текущему индексу нам не подходит
        matrix->rows[*index] < row)
        (*index)++;

    // если нашли подходящий - возвращаем ок
    if (*index < matrix->nonzero_size && matrix->rows[*index] == row &&
        ((col < matrix->cols_size && *index < matrix->cols[col]) || col == matrix->cols_size))
        return NO_NEED_ADJUST;

    // иначе - надо вставлять новый элемент по возвращаемому индексу
    return NEED_ADJUST;
}

mat_elem_t sp_get(const sparse_matrix_t *matrix, mat_index_t row, mat_index_t col)
{
    assert_sp_mat(matrix);
    assert(0 <= row && row < matrix->rows_size);
    assert(0 <= col && col < matrix->cols_size);

    mat_index_t index;
    int adjust = imp__find_linear_index(matrix, row, col, &index);
    if (adjust == NO_NEED_ADJUST)
        return matrix->nonzero_array[index];

    return 0;
}

void sp_set(sparse_matrix_t *matrix, mat_index_t row, mat_index_t col, mat_elem_t value)
{
    assert_sp_mat(matrix);
    assert(0 <= row && row < matrix->rows_size);
    assert(0 <= col && col < matrix->cols_size);

    mat_index_t index;
    int adjust = imp__find_linear_index(matrix, row, col, &index);
    if (adjust == NO_NEED_ADJUST)
        matrix->nonzero_array[index] = value;
    else
    {
        // insert new element

        // realloc first
        matrix->nonzero_size++;
        matrix->rows = realloc(matrix->rows, matrix->nonzero_size * sizeof(mat_index_t));
        matrix->nonzero_array = realloc(matrix->nonzero_array, matrix->nonzero_size * sizeof(mat_index_t));

        // smart check for memory leaks
        assert(matrix->rows != NULL);
        assert(matrix->nonzero_array != NULL);

        // shift everything forward
        mat_elem_t next_value = value; // matrix->nonzero_array[index];
        // matrix->nonzero_array[index] = value;

        mat_index_t next_row = row; // matrix->rows[index];
        // matrix->rows[index] = row;

        while (index < matrix->nonzero_size)
        {
            // if (col < matrix->cols_size && matrix->cols[col] == index + 1)
            // {
            //    matrix->cols[col]++;
            //    col++;
            // }

            value = matrix->nonzero_array[index];
            matrix->nonzero_array[index] = next_value;
            next_value = value;

            row = matrix->rows[index];
            matrix->rows[index] = next_row;
            next_row = row;

            index++;
        }

        // propagate cols values
        mat_index_t start_col = adjust == NEED_START_COL ? col + 1 : col + 1;
        while (start_col <= matrix->cols_size)
        {
            matrix->cols[start_col]++;
            start_col++;
        }
    }
}

// не отвечает за выделение памяти !!!
int sp_mult_by_vector(const sparse_matrix_t *matrix, const mat_elem_t *vector, mat_elem_t *out)
{
    assert_sp_mat(matrix);
    
    if (vector == NULL || out == NULL)
        return EXIT_FAILURE;

    for (mat_index_t i = 0; i < matrix->cols_size; i++)
        out[i] = 0;
    
    for (mat_index_t row = 0; row < matrix->rows_size; row++)
        for (mat_index_t col = 0; col < matrix->cols_size; col++)
            out[row] += vector[col] * sp_get(matrix, row, col);

    return EXIT_SUCCESS;
}

int sp_mult_matrix(const sparse_matrix_t *matrix_1, const sparse_matrix_t *matrix_2, sparse_matrix_t *out)
{
    assert_sp_mat(matrix_1);
    assert_sp_mat(matrix_2);

    if (out == NULL)
        return EXIT_FAILURE;

    if (matrix_1->cols_size != matrix_2->rows_size)
        return BAD_DIMENSIONS;

    for (mat_index_t row = 0; row < matrix_1->rows_size; row++)
    for (mat_index_t col = 0; col < matrix_2->cols_size; col++)
    {
        mat_elem_t value = 0;
        for (mat_index_t i = 0; i < matrix_1->cols_size; i++)
            value += sp_get(matrix_1, row, i) * sp_get(matrix_2, i, col);
        
        if (value != 0)
            sp_set(out, row, col, value);
    }

    return EXIT_SUCCESS;
}

static void imp__print_title(mat_size_t cols)
{
    printf("+-");
    for (mat_size_t col = 0; col < cols; col++)
        for (size_t i = 0; i < PRINT_ELEM_SIZE + 1; i++)
            printf("-");
    printf("+\n");
}

static void imp__print_elem(const sparse_matrix_t *matrix, mat_index_t row, mat_index_t col)
{
    char format[5];
    sprintf(format, "%%%dd", PRINT_ELEM_SIZE);
    printf(format, sp_get(matrix, row, col));
}

void sp_print_info(const sparse_matrix_t *matrix)
{
    printf("[MATRIX]\n");
    printf("rows x cols: %ldx%ld\n", matrix->rows_size, matrix->cols_size);
    printf("nonzero size: %ld\n", matrix->nonzero_size);
    printf("elements:");
    for (mat_index_t i = 0; i < matrix->nonzero_size; i++)
        printf(" %3d", matrix->nonzero_array[i]);
    printf("\nrow nums:");
    for (mat_index_t i = 0; i < matrix->nonzero_size; i++)
        printf(" %3ld", matrix->rows[i]);
    printf("\ncols:");
    for (mat_index_t i = 0; i < matrix->cols_size + 1; i++)
        printf(" %ld", matrix->cols[i]);
    printf("\n[END]\n");
}

void sp_print(const sparse_matrix_t *matrix)
{
    assert_sp_mat(matrix);

    imp__print_title(matrix->cols_size);

    for (mat_size_t row = 0; row < matrix->rows_size; row++)
    {
        printf("| ");
        for (mat_size_t col = 0; col < matrix->cols_size; col++)
        {
            imp__print_elem(matrix, row, col);
            printf(" ");
        }
        printf("|\n");
    }

    imp__print_title(matrix->cols_size);
}
