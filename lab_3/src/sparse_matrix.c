#include <stdio.h>
#include <memory.h>
#include <stdlib.h>
#include <assert.h>
#include "../include/sparse_matrix.h"

#define PRINT_ELEM_SIZE 3

sparse_matrix_t sp_create(mat_size_t rows, mat_size_t cols)
{
    assert(rows > 0);
    assert(cols > 0);

    sparse_matrix_t matrix;

    matrix.rows_size = rows;
    matrix.cols_size = cols;
    matrix.nonzero_size = 1;

    matrix.nonzero_array = calloc(1, sizeof(mat_elem_t));
    matrix.rows = calloc(1, sizeof(mat_index_t));
    matrix.cols = calloc(cols, sizeof(mat_index_t));

    for (mat_index_t i = 1; i < cols; i++)
        matrix.cols[i] = INVALID_INDEX;

    return matrix;
}

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
int imp__find_linear_index(const sparse_matrix_t *matrix, mat_index_t row, mat_index_t col, mat_index_t *index)
{
    assert_sp_mat(matrix);
    assert(0 <= row && row < matrix->rows_size);
    assert(0 <= col && col < matrix->cols_size);

    // поиск индекса происходит в рамках заданного столбца
    // проверим, не пуст ли данный столбец
    if (matrix->cols[col] == INVALID_INDEX)
    {
        // найди последний доступный индекс для вставки
        while (col < matrix->cols_size && matrix->cols[col] == INVALID_INDEX)
            col++;
        
        if (col < matrix->cols_size)
            *index = matrix->cols[col] - 1;
        else
            *index = matrix->nonzero_size;

        return NEED_ADJUST; // NEED_START_COL; // надо начать новую колонку
    }

    // первый линейный индекс, с которого будет проходить поиск
    *index = matrix->cols[col];

    // подогнать col под следующий непустой столбец
    do {
        col++;
    } while (col < matrix->cols_size && matrix->cols[col] == INVALID_INDEX);

    // пока индекс нам не подходит переходим к следующему
    while (
        // не вышли за границу доступного
        *index < matrix->nonzero_size &&
        // мы не соскочили с текущего столбца
        (
            (col < matrix->cols_size &&
            // !!! ворнинг !!!
            // это условие может сработать только
            // через неопределённое число колонок!
            *index < matrix->cols[col]
            // как быть?? решено
            )
            ||
            col == matrix->cols_size
        ) &&
        // номер строки элемента по текущему индексу нам не подходит
        matrix->rows[*index] < row
    )
        (*index)++;
    
    // если нашли подходящий - возвращаем ок
    if (*index < matrix->nonzero_size && matrix->rows[*index] == row)
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
    else if (adjust == NEED_ADJUST)
    {
        // insert new element
        printf("inserting new element at index = %ld\n", index);
        
        // realloc first
        matrix->nonzero_size++;
        matrix->rows = realloc(matrix->rows, matrix->nonzero_size * sizeof(mat_index_t));
        matrix->nonzero_array = realloc(matrix->nonzero_array, matrix->nonzero_size * sizeof(mat_index_t));

        // smart check for memory leaks
        printf("nonzero_size = %ld\n", matrix->nonzero_size);
        assert(matrix->rows != NULL);
        assert(matrix->nonzero_array != NULL);

        // shift everything forward
        mat_elem_t next_value = value; // matrix->nonzero_array[index];
        // matrix->nonzero_array[index] = value;

        mat_index_t next_row = row; // matrix->rows[index];
        // matrix->rows[index] = row;

        while (index < matrix->nonzero_size)
        {
            if (col < matrix->cols_size && matrix->cols[col] == index + 1)
            {
                matrix->cols[col]++;
                col++;
            }

            value = matrix->nonzero_array[index];
            matrix->nonzero_array[index] = next_value;
            next_value = value;

            row = matrix->rows[index];
            matrix->rows[index] = next_row;
            next_row = row;

            index++;
        }
    }
    else
    {
        // realloc first
        matrix->nonzero_size++;
        matrix->rows = realloc(matrix->rows, matrix->nonzero_size * sizeof(mat_index_t));
        matrix->nonzero_array = realloc(matrix->nonzero_array, matrix->nonzero_size * sizeof(mat_index_t));

        // smart check for memory leaks
        printf("nonzero_size = %ld\n", matrix->nonzero_size);
        assert(matrix->rows != NULL);
        assert(matrix->nonzero_array != NULL);

        // связываем новый столбец
        matrix->cols[col] = index;

        // shift everything forward
        mat_elem_t next_value = value; // matrix->nonzero_array[index];
        // matrix->nonzero_array[index] = value;

        mat_index_t next_row = row; // matrix->rows[index];
        // matrix->rows[index] = row;

        while (index < matrix->nonzero_size)
        {
            if (col < matrix->cols_size && matrix->cols[col] == index + 1)
            {
                matrix->cols[col]++;
                col++;
            }

            value = matrix->nonzero_array[index];
            matrix->nonzero_array[index] = next_value;
            next_value = value;

            row = matrix->rows[index];
            matrix->rows[index] = next_row;
            next_row = row;

            index++;
        }
    }
}

sparse_matrix_t sp_mult_by_vector(const sparse_matrix_t *matrix, const sparse_matrix_t *vector)
{
    assert_sp_mat(matrix);
    assert_sp_mat(vector);

    return *matrix;
}

sparse_matrix_t sp_mult_matrix(const sparse_matrix_t *matrix_1, const sparse_matrix_t *matrix_2)
{
    assert_sp_mat(matrix_1);
    assert_sp_mat(matrix_2);

    return *matrix_1;
}

void imp__print_title(mat_size_t cols)
{
    printf("+-");
    for (mat_size_t col = 0; col < cols; col++)
        for (size_t i = 0; i < PRINT_ELEM_SIZE + 1; i++)
            printf("-");
    printf("+\n");
}

void imp__print_elem(const sparse_matrix_t *matrix, mat_index_t row, mat_index_t col)
{
    char format[5];
    sprintf(format, "%%%dd", PRINT_ELEM_SIZE);

#if 0
    printf(format, sp_get(matrix, row, col));
#else
    mat_index_t index;
    imp__find_linear_index(matrix, row, col, &index);
    printf(format, index);
#endif
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
