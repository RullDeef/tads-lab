#include <stdio.h>
#include <memory.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>
#include <time.h>
#include "sparse_matrix.h"

#define PRINT_ELEM_SIZE 6

sparse_matrix_t sp_null_matrix(void)
{
    sparse_matrix_t matrix = {
        .rows_size = 0,
        .cols_size = 0,
        .nonzero_size = 0,
        .__alloc_nz_sz = 0,
        .__alloc_cl_sz = 0,
        .cols = NULL,
        .rows = NULL,
        .nonzero_array = NULL};

    return matrix;
}

sparse_matrix_t sp_create(uint32_t rows, uint32_t cols)
{
    sparse_matrix_t matrix;

    matrix.rows_size = rows;
    matrix.cols_size = cols;
    matrix.nonzero_size = 0;

    matrix.__alloc_nz_sz = SP_INITIAL_ALLOC_SIZE;
    matrix.nonzero_array = calloc(SP_INITIAL_ALLOC_SIZE, sizeof(mat_elem_t));
    matrix.rows = calloc(SP_INITIAL_ALLOC_SIZE, sizeof(uint32_t));

    // "fixed" size space allocation
    matrix.__alloc_cl_sz = (uint32_t)((cols + 1) * SP_ALLOC_MULTIPILER);
    matrix.cols = calloc(matrix.__alloc_cl_sz, sizeof(uint32_t));

    return matrix;
}

int sp_recreate(sparse_matrix_t *matrix, uint32_t rows, uint32_t cols)
{
    sp_free(matrix);
    *matrix = sp_create(rows, cols);
    return !sp_mat_is_null(matrix);
}

/*
int sp_resize(sparse_matrix_t* matrix, uint32_t new_rows, uint32_t new_cols)
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
            uint32_t new_alloc_cl_sz = (new_cols + 1) * SP_ALLOC_MULTIPILER;
            uint32_t* new_cols_ptr = realloc(matrix->cols, new_alloc_cl_sz);
            if (new_cols_ptr == NULL)
                status = BAD_ALLOC;
            else
            {
                matrix->cols = new_cols_ptr;
                matrix->__alloc_cl_sz = new_alloc_cl_sz;

                for (uint32_t i = matrix->cols_size + 1; i < new_cols + 1; i++)
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
    if (matrix == NULL || sp_mat_is_null(matrix))
        return;

    if (matrix->nonzero_array != NULL)
    {
        free(matrix->nonzero_array);
        free(matrix->rows);
    }

    free(matrix->cols);
    *matrix = sp_null_matrix();
}

sparse_matrix_t sp_copy(const sparse_matrix_t *matrix)
{
    sparse_matrix_t copy = *matrix;
    copy.nonzero_array = malloc(matrix->__alloc_nz_sz * sizeof(mat_elem_t));
    memcpy(copy.nonzero_array, matrix->nonzero_array, matrix->nonzero_size * sizeof(mat_elem_t));

    copy.rows = malloc(matrix->__alloc_nz_sz * sizeof(uint32_t));
    memcpy(copy.rows, matrix->rows, matrix->nonzero_size * sizeof(uint32_t));

    copy.cols = malloc(matrix->__alloc_cl_sz * sizeof(uint32_t));
    memcpy(copy.cols, matrix->cols, (matrix->cols_size + 1) * sizeof(uint32_t));

    return copy;
}

bool sp_mat_is_null(const sparse_matrix_t *matrix)
{
    return matrix->cols == NULL;
}

/**
 * @brief проверяет столбец на наличие элементов.
 */
static bool imp__has_values_in_col(const sparse_matrix_t *matrix, uint32_t col)
{
    assert_sp_mat(*matrix);
    assert(col < matrix->cols_size);

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
static int imp__find_linear_index(const sparse_matrix_t *matrix, uint32_t row, uint32_t col, uint32_t *index)
{
    assert_sp_mat(*matrix);
    assert(row < matrix->rows_size);
    assert(col < matrix->cols_size);

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

mat_elem_t sp_get(const sparse_matrix_t *matrix, uint32_t row, uint32_t col)
{
    if (matrix == NULL || matrix->nonzero_size == 0)
        return 0;

    if (row >= matrix->rows_size || col >= matrix->cols_size)
        return 0;

    uint32_t index;
    int adjust = imp__find_linear_index(matrix, row, col, &index);
    if (adjust == NO_NEED_ADJUST)
        return matrix->nonzero_array[index];

    return 0;
}

void sp_set(sparse_matrix_t *matrix, uint32_t row, uint32_t col, mat_elem_t value)
{
    assert_sp_mat(*matrix);
    assert(row < matrix->rows_size);
    assert(col < matrix->cols_size);

    uint32_t index;
    int adjust = imp__find_linear_index(matrix, row, col, &index);
    if (adjust == NO_NEED_ADJUST)
        matrix->nonzero_array[index] = value;
    else
    {
        // insert new element

        // realloc first
        matrix->nonzero_size++;
        matrix->rows = realloc(matrix->rows, matrix->nonzero_size * sizeof(uint32_t));
        matrix->nonzero_array = realloc(matrix->nonzero_array, matrix->nonzero_size * sizeof(uint32_t));

        // smart check for memory leaks
        assert(matrix->rows != NULL);
        assert(matrix->nonzero_array != NULL);

        // shift everything forward
        mat_elem_t next_value = value; // matrix->nonzero_array[index];
        // matrix->nonzero_array[index] = value;

        uint32_t next_row = row; // matrix->rows[index];
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
        uint32_t start_col = adjust == NEED_START_COL ? col + 1 : col + 1;
        while (start_col <= matrix->cols_size)
        {
            matrix->cols[start_col]++;
            start_col++;
        }
    }
}

static uint32_t imp__calc_zero_elemets(sparse_matrix_t *matrix)
{
    assert_sp_mat(*matrix);
    uint32_t count = 0;

    for (uint32_t i = 0; i < matrix->nonzero_size; i++)
        if (matrix->nonzero_array[i] == 0)
            count++;

    return count;
}

/*
    Алгоритм сжатия матрицы (работает)

    1. Поиск индексов всех нулевых элементов
    2. Смещение ненулевых элементов с конца массива
    3. Смещение индексов строк элементов с конца массива
    4. Обновление индексов столбцов в "связном списке"
*/
void sp_compress(sparse_matrix_t *matrix)
{
    if (matrix == NULL || sp_mat_is_null(matrix))
    {
        // printf("mat is null!\n");
        return;
    }

    uint32_t z_indices_len = imp__calc_zero_elemets(matrix);

    // no zero elements
    if (z_indices_len == 0)
    {
        // printf("no zero elements!\n");
        return;
    }
    // printf("found %lu zeros:", z_indices_len);

    // WARNING: check for bad alloc
    uint32_t *z_indices = calloc(z_indices_len, sizeof(uint32_t));
    uint32_t z_index = 0; // index in zero indices array

    // get zeros indices
    for (uint32_t i = 0; i < matrix->nonzero_size; i++)
    {
        if (matrix->nonzero_array[i] == 0)
        {
            z_indices[z_index] = i;
            z_index++;
            // printf(" %ld", i);
        }
    }
    // printf("\n");

    // pack non-zero elements and corresponding rows
    z_index = 0;               // index in zero indices array
    uint32_t zeros_passed = 0; // кол-во уже пропущенных нулей

    for (uint32_t nz_index = z_indices[z_index]; nz_index < matrix->nonzero_size; nz_index++)
    {
        if (z_index < z_indices_len && nz_index == z_indices[z_index])
        {
            zeros_passed++;
            z_index++;
        }
        else
        {
            matrix->nonzero_array[nz_index - zeros_passed] = matrix->nonzero_array[nz_index];
            matrix->rows[nz_index - zeros_passed] = matrix->rows[nz_index];
        }
    }

    // update cols indices
    z_index = 0;      // index in zero indices array
    zeros_passed = 0; // кол-во уже пропущенных нулей

    for (uint32_t col_index = 0; col_index < matrix->cols_size + 1; col_index++)
    {
        if (z_index < z_indices_len && matrix->cols[col_index] > z_indices[z_index])
        {
            zeros_passed++;
            z_index++;
            col_index--;
        }
        else
            matrix->cols[col_index] -= zeros_passed;
    }

    // ralloc memory
    matrix->nonzero_size -= z_indices_len;
    matrix->__alloc_nz_sz = matrix->nonzero_size;
    matrix->nonzero_array = realloc(matrix->nonzero_array, matrix->__alloc_nz_sz * sizeof(mat_elem_t));
    matrix->rows = realloc(matrix->rows, matrix->__alloc_nz_sz * sizeof(uint32_t));
    if (matrix->__alloc_nz_sz == 0)
    {
        matrix->nonzero_array = NULL;
        matrix->rows = NULL;
    }

    free(z_indices);
}

void sp_clear(sparse_matrix_t *matrix)
{
    if (sp_mat_is_null(matrix))
        return;

    matrix->nonzero_size = 0;
    matrix->__alloc_nz_sz = SP_INITIAL_ALLOC_SIZE;
    matrix->nonzero_array = realloc(matrix->nonzero_array, SP_INITIAL_ALLOC_SIZE * sizeof(mat_elem_t));
    matrix->rows = realloc(matrix->rows, SP_INITIAL_ALLOC_SIZE * sizeof(uint32_t));

    for (uint32_t i = 0; i < matrix->cols_size + 1; i++)
        matrix->cols[i] = 0;
}

/**
 * @brief Заполняет случайным образом элементы матрицы.
 * 
 * Возвращает сжатую матрицу.
 * 
 * @param nz_percent - процент ненулевых элементов.
 */
void sp_randomize(sparse_matrix_t *matrix, float nz_percent)
{
    if (sp_mat_is_null(matrix))
        return;

    uint64_t nonzero_amount = nz_percent * matrix->rows_size * matrix->cols_size;

    for (uint64_t i = 0; i < nonzero_amount; i++)
    {
        uint32_t row = ((float)rand() / (RAND_MAX + 1L)) * matrix->rows_size;
        uint32_t col = ((float)rand() / (RAND_MAX + 1L)) * matrix->cols_size;

        mat_elem_t value = rand() % 99 - 49;
        if (value <= 0)
            value--;

        if (row >= matrix->rows_size)
            row = matrix->rows_size - 1;
        if (col >= matrix->cols_size)
            col = matrix->cols_size - 1;

        sp_set(matrix, row, col, value);
    }

    sp_compress(matrix);
}

void sp_transpose(sparse_matrix_t *matrix)
{
    struct triplet
    {
        uint32_t row;
        uint32_t col;
        mat_elem_t value;
    };

    struct triplet *triplets = malloc(matrix->nonzero_size * sizeof(struct triplet));

    uint32_t col = 0;
    for (uint32_t index = 0; index < matrix->nonzero_size; index++)
    {
        while (matrix->cols[col + 1] <= index)
            col++;

        triplets[index].value = matrix->nonzero_array[index];
        triplets[index].col = matrix->rows[index];
        triplets[index].row = col; // transpose values here
    }

    // realloc cols array
    if (matrix->__alloc_cl_sz < matrix->rows_size + 1)
    {
        matrix->__alloc_cl_sz = (matrix->rows_size + 1) * SP_ALLOC_MULTIPILER;
        matrix->cols = realloc(matrix->cols, matrix->__alloc_cl_sz);
    }
    memset(matrix->cols, 0, (matrix->rows_size + 1) * sizeof(uint32_t));

    uint32_t mat_index = 0;
    uint32_t curr_row_not_transp = 0;
    while (mat_index < matrix->nonzero_size)
    {
        for (uint32_t i = 0; i < matrix->nonzero_size; i++)
        {
            struct triplet trp = triplets[i];

            if (trp.col == curr_row_not_transp)
            {
                matrix->nonzero_array[mat_index] = trp.value;
                matrix->rows[mat_index] = trp.row;
                matrix->cols[trp.col + 1] += 1;
                mat_index++;
            }
        }
        curr_row_not_transp++;
    }

    // adjust cols values
    for (uint32_t col = 0; col < matrix->rows_size + 1; col++)
        matrix->cols[col + 1] += matrix->cols[col];

    // reassign rows and cols size
    uint32_t temp = matrix->rows_size;
    matrix->rows_size = matrix->cols_size;
    matrix->cols_size = temp;

    free(triplets);
}

/*
static bool imp__get_first_in_row(const sparse_matrix_t *matrix, uint32_t row, uint32_t *index, uint32_t *col)
{
    *index = 0;
    while (*index < matrix->nonzero_size && matrix->rows[*index] != row)
        (*index)++;
    if (*index < matrix->nonzero_size)
    {
        *col = 0;
        while (matrix->cols[*col] <= *index)
            (*col)++;
        (*col)--;
    }
    return *index < matrix->nonzero_size;
}

static bool imp__get_first_in_col(const sparse_matrix_t *matrix, uint32_t col, uint32_t *index, uint32_t *row)
{
    *index = matrix->cols[col];
    if (*index < matrix->nonzero_size)
        *row = matrix->rows[*index];
    return *index < matrix->nonzero_size;
}

static bool imp__get_next_in_row(const sparse_matrix_t *matrix, uint32_t index, uint32_t *next, uint32_t *col)
{
    uint32_t row = matrix->rows[index];
    *next = index + 1;
    while (*next < matrix->nonzero_size && matrix->rows[*next] != row)
        (*next)++;
    bool founded = *next < matrix->nonzero_size && matrix->rows[*next] == row;
    if (founded)
    {
        while (matrix->cols[*col] <= *next)
            (*col)++;
        (*col)--;
    }
    return founded;
}

static bool imp__get_next_in_col(const sparse_matrix_t *matrix, uint32_t col, uint32_t index, uint32_t *next, uint32_t *row)
{
    *next = index + 1;
    bool founded = (*next < matrix->nonzero_size) && (*next < matrix->cols[col + 1]);
    if (founded)
    {
        *row = matrix->rows[*next];
    }
    return founded;
}
*/

// первая матрица должна быть транспонирована перед вызовом данной функции
int sp_mult_matrix(const sparse_matrix_t *matrix_1, const sparse_matrix_t *matrix_2, sparse_matrix_t *out)
{
    if (matrix_1 == NULL || matrix_2 == NULL || out == NULL)
        return EXIT_FAILURE;

    if (sp_mat_is_null(matrix_1) || sp_mat_is_null(matrix_2))
        return EXIT_FAILURE;

    if (matrix_1->rows_size != matrix_2->rows_size)
        return BAD_DIMENSIONS;

    // WARNING: assuming no bad allocs here
    if (!sp_recreate(out, matrix_1->cols_size, matrix_2->cols_size))
        return EXIT_FAILURE;

    uint32_t index_out = 0;

    uint32_t col_out = 0;
    while (col_out < out->cols_size)
    {
        // определить индексы элементов в текущих перемножаемых векторах
        uint32_t index_2_begin = matrix_2->cols[col_out];
        uint32_t index_2_end = matrix_2->cols[col_out + 1];

        // проверить пустоту первого вектора
        if (index_2_begin != index_2_end)
        {
            uint32_t row_out = 0;
            while (row_out < out->rows_size)
            {
                uint32_t index_1_begin = matrix_1->cols[row_out];
                uint32_t index_1_end = matrix_1->cols[row_out + 1];

                if (index_1_begin != index_1_end)
                {
                    mat_elem_t result_value = 0;

                    uint32_t index_1 = index_1_begin;
                    uint32_t index_2 = index_2_begin;

                    while (index_1 < index_1_end && index_2 < index_2_end)
                    {
                        uint32_t pos_1 = matrix_1->rows[index_1];
                        uint32_t pos_2 = matrix_2->rows[index_2];
                        if (pos_1 < pos_2)
                            index_1++;
                        else if (pos_1 > pos_2)
                            index_2++;
                        else
                        {
                            result_value += matrix_1->nonzero_array[index_1] * matrix_2->nonzero_array[index_2];
                            index_1++;
                            index_2++;
                        }
                    }

                    if (result_value != 0)
                    {
                        // insert directly (realloc if needed)
                        if (index_out >= out->__alloc_nz_sz)
                        {
                            out->__alloc_nz_sz = (index_out + 1) * SP_ALLOC_MULTIPILER;
                            out->nonzero_array = realloc(out->nonzero_array, out->__alloc_nz_sz * sizeof(mat_elem_t));
                            out->rows = realloc(out->rows, out->__alloc_nz_sz * sizeof(uint32_t));
                        }

                        out->nonzero_array[index_out] = result_value;
                        out->rows[index_out] = row_out;
                        out->cols[col_out + 1]++;
                        index_out++;
                    }
                }

                row_out++;
            }
        }

        col_out++;
    }

    // adjust cols
    for (uint32_t col = 0; col < out->cols_size + 1; col++)
        out->cols[col + 1] += out->cols[col];
    // setup new nonzero size
    out->nonzero_size = index_out;

    return EXIT_SUCCESS;
}

static void imp__print_elem(const sparse_matrix_t *matrix, uint32_t row, uint32_t col)
{
    mat_elem_t value = sp_get(matrix, row, col);
    if (value != 0)
        printf("%*d", PRINT_ELEM_SIZE, value);
    else
        printf("%*s", PRINT_ELEM_SIZE, ".");
}

void sp_print_info(const sparse_matrix_t *matrix)
{
    printf("[Внутренняя структура матрицы]\n"
           "размер: %dx%d\n"
           "кол-во ненулевых: %d\n"
           "элементы     :",
           matrix->rows_size, matrix->cols_size, matrix->nonzero_size);
    
    if (matrix->nonzero_size <= 11)
    {
        for (uint32_t i = 0; i < matrix->nonzero_size; i++)
            printf(" %5d", matrix->nonzero_array[i]);
    }
    else
    {
        for (uint32_t i = 0; i < matrix->nonzero_size; i++)
        {
            if (i > 5 && i < matrix->nonzero_size - 5)
            {
                i = matrix->nonzero_size - 5;
                printf(" ...");
            }
            else
                printf(" %5d", matrix->nonzero_array[i]);
        }
    }
    printf("\nиндексы строк:");
    if (matrix->nonzero_size <= 11)
    {
        for (uint32_t i = 0; i < matrix->nonzero_size; i++)
            printf(" %5d", matrix->rows[i]);
    }
    else
    {
        for (uint32_t i = 0; i < matrix->nonzero_size; i++)
        {
            if (i > 5 && i < matrix->nonzero_size - 5)
            {
                i = matrix->nonzero_size - 5;
                printf(" ...");
            }
            else
                printf(" %5d", matrix->rows[i]);
        }
    }
    printf("\nстолбцы:");
    if (matrix->cols_size + 1 <= 11)
    {
        for (uint32_t i = 0; i < matrix->cols_size + 1; i++)
            printf(" %d", matrix->cols[i]);
    }
    else
    {
        for (uint32_t i = 0; i < matrix->cols_size + 1; i++)
        {
            if (i > 5 && i < matrix->cols_size + 1 - 5)
            {
                i = matrix->cols_size + 1 - 5;
                printf(" ...");
            }
            else
                printf(" %d", matrix->cols[i]);
        }
    }
    if (matrix->rows_size <= 12 && matrix->cols_size <= 12)
    {
        printf("\n[В обычном виде:]\n");
        sp_print(matrix);
    }
    printf("\n[Конец структуры]\n");
}

void sp_print(const sparse_matrix_t *matrix)
{
    assert_sp_mat(*matrix);

    printf("╔═");
    for (uint32_t col = 0; col < matrix->cols_size; col++)
        for (uint32_t i = 0; i < PRINT_ELEM_SIZE + 1; i++)
            printf("═");
    printf("╗\n");

    for (uint32_t row = 0; row < matrix->rows_size; row++)
    {
        printf("║ ");
        for (uint32_t col = 0; col < matrix->cols_size; col++)
        {
            imp__print_elem(matrix, row, col);
            printf(" ");
        }
        printf("║\n");
    }

    printf("╚═");
    for (uint32_t col = 0; col < matrix->cols_size; col++)
        for (uint32_t i = 0; i < PRINT_ELEM_SIZE + 1; i++)
            printf("═");
    printf("╝\n");
}
