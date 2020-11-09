#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include "uki.h"
#include "core/matrix.h"
#include "matrix_input.h"

static bool imp__input_mat_dims(uint32_t *rows, uint32_t *cols);
static bool imp__input_triplet(uint32_t *row, uint32_t *col, int *value);
static bool imp__input_duplet(uint32_t *row, int *value);

static bool imp__input_percent_bounds(const char *msg, const char *err_msg, float *percent, mat_elem_t *min, mat_elem_t *max);

static sparse_matrix_t imp__input_matrix_regular(void);
static sparse_matrix_t imp__input_matrix_coordinate(void);
static sparse_matrix_t imp__input_matrix_auto(void);

static bool imp__input_mat_dims(uint32_t *rows, uint32_t *cols)
{
    int32_t rows_int, cols_int;
    if (uki_input_int32("Введите число строк матрицы: ",
                        "Число строк должно быть положительным.\n", &rows_int) &&
        rows_int > 0 &&
        uki_input_int32("Введите число строк матрицы: ",
                        "Число столбцов должно быть положительным.\n", &cols_int) &&
        cols_int > 0)
    {
        *rows = rows_int;
        *cols = cols_int;
        return 0;
    }

    printf("Число строк и столбцов должно быть положительным.\n");
    return -1;
}

bool input_auto_mat_dims(uint32_t *dim1, uint32_t *dim2, uint32_t *dim3)
{
    int32_t dim1_int, dim2_int, dim3_int;
    if (uki_input_int32("Введите число строк первой матрицы: ",
                        "Число строк должно быть положительным.\n", &dim1_int) &&
        dim1_int > 0 &&
        uki_input_int32("Введите число столбцов первой матрицы: ",
                        "Число столбцов должно быть положительным.\n", &dim2_int) &&
        dim2_int > 0 &&
        uki_input_int32("Введите число столбцов второй матрицы: ",
                        "Число столбцов должно быть положительным.\n", &dim3_int) &&
        dim3_int > 0)
    {
        *dim1 = dim1_int;
        *dim2 = dim2_int;
        *dim3 = dim3_int;
        return 0;
    }

    printf("Число строк и столбцов должно быть положительным.\n");
    return -1;
}

sparse_matrix_t input_matrix(void)
{
    sparse_matrix_t matrix = sp_null_matrix();
    uint32_t rows, cols;
    if (!imp__input_mat_dims(&rows, &cols))
    {
        matrix = sp_create(rows, cols);
        printf("Вводите тройки чисел: <строка столбец элемент> для каждого ненулевого элемента в матрице.\n");

        uint32_t row, col;
        int value;
        while (!imp__input_triplet(&row, &col, &value))
            sp_set(&matrix, row, col, value);
    }
    return matrix;
}

sparse_matrix_t input_vector(uint32_t size)
{
    sparse_matrix_t vector = sp_create(size, 1);

    printf("Ввод значений умножаемого вектора размером %u строк.\n", size);
    printf("Вводите пары чисел: <строка элемент> для каждого ненулевого элемента вектора.\n");

    uint32_t row;
    int value;
    while (!imp__input_duplet(&row, &value))
        sp_set(&vector, row - 1, 0, value);

    return vector;
}

sparse_matrix_t ultimate_input_matrix(void)
{
    // request input format
    uint32_t input_format;
    if (uki_input_uint32_minmax("Выберите формат ввода матрицы:\n"
                                "  1. Размер и все элементы\n"
                                "  2. Размер, кол-во ненулевых эл-тов и ненулевые эл-ты\n"
                                "  3. Размер и процент ненулевых эл-тов\n"
                                "> ",
                                "Неверный ввод.\n", 1, 4, &input_format))
    {
        switch (input_format)
        {
        case 1:
            return imp__input_matrix_regular();
        case 2:
            return imp__input_matrix_coordinate();
        case 3:
            return imp__input_matrix_auto();
        }
    }

    return sp_null_matrix();
}

static bool imp__input_triplet(uint32_t *row, uint32_t *col, int *value)
{
    char line[256];
    if (fgets(line, 256, stdin) == NULL)
        return -1;

    char *start = line;
    char *end = NULL;
    *row = strtoul(start, &end, 10);
    if (end == start)
        return -1;

    start = end;
    *col = strtoul(start, &end, 10);
    if (end == start)
        return -1;

    start = end;
    *value = strtod(start, &end);
    if (end == start)
        return -1;
    
    if (*row == 0 || *col == 0)
        return -1;

    return 0;
}

static bool imp__input_duplet(uint32_t *row, int *value)
{
    char line[256];
    if (fgets(line, 256, stdin) == NULL)
        return -1;

    char *start = line;
    char *end = NULL;
    *row = strtoul(start, &end, 10);
    if (end == start)
        return -1;

    start = end;
    *value = strtol(start, &end, 10);
    if (end == start)
        return -1;

    if (*row == 0)
        return -1;

    return 0;
}

static sparse_matrix_t imp__input_matrix_regular(void)
{
    uint32_t rows, cols;
    sparse_matrix_t matrix = sp_null_matrix();
    if (uki_input_uint32_minmax("Введите число строк матрицы: ",
                                "Число строк должно быть меньше 10 для данного формата.\n", 1, 11, &rows) &&
        uki_input_uint32_minmax("Введите число столбцов матрицы: ",
                                "Число столбцов должно быть меньше 10 для данного формата.\n", 1, 11, &cols))
    {
        matrix = sp_create(rows, cols);
        printf("Введите элементы матрицы в виде матрицы:\n");

        bool bad_input = false;
        char temp_line[512];
        for (uint32_t row = 0; row < rows; row++)
        {
            if (fgets(temp_line, 512, stdin) == NULL)
            {
                bad_input = true;
                break;
            }
            else
            {
                char *begin = temp_line;
                char *end = NULL;
                for (uint32_t col = 0; col < cols; col++)
                {
                    int value = strtol(begin, &end, 10);

                    if (begin == end)
                        bad_input = true;
                    else
                    {
                        if (value != 0)
                            sp_set(&matrix, row, col, value);
                        begin = end;
                    }
                }
            }
        }

        if (bad_input)
            sp_free(&matrix);
    }
    return matrix;
}

static sparse_matrix_t imp__input_matrix_coordinate(void)
{
    uint32_t rows, cols, nonzero;
    sparse_matrix_t matrix = sp_null_matrix();
    if (uki_input_uint32_minmax("Введите число строк матрицы: ",
                                "Число строк должно быть положительным.\n", 1, 1 << 20, &rows) &&
        uki_input_uint32_minmax("Введите число столбцов матрицы: ",
                                "Число столбцов должно быть положительным.\n", 1, 1 << 20, &cols) &&
        uki_input_uint32("Введите число ненулевых элементов: ",
                         "Неверное число ненулевых элементов.\n", &nonzero))
    {
        if (nonzero > rows * cols)
            printf("Ненулевых элементов больше чем может быть.\n");
        else
        {
            matrix = sp_create(rows, cols);
            printf("Вводите тройки чисел: <строка столбец элемент> для каждого ненулевого элемента в матрице.\n");

            for (uint32_t i = 0; i < nonzero; i++)
            {
                uint32_t row, col;
                int value;

                if (!imp__input_triplet(&row, &col, &value))
                {
                    sp_free(&matrix);
                    break;
                }
                sp_set(&matrix, row - 1, col - 1, value);
            }
        }
    }
    return matrix;
}

static bool imp__input_percent_bounds(const char *msg, const char *err_msg, float *percent, mat_elem_t *min, mat_elem_t *max)
{
    printf("%s", msg);

    char temp[256];
    if (fgets(temp, 256, stdin) == NULL)
    {
        printf("%s", err_msg);
        return false;
    }
    
    char *begin = temp;
    char *end = begin;

    *percent = strtof(begin, &end);
    if (begin == end || *percent < 0 || *percent > 100)
    {
        printf("%s", err_msg);
        return false;
    }
    
    begin = end;
    while (isspace((int)*begin))
        begin++;
    
    if (strlen(begin) != 0)
    {
        end = begin;
        *min = strtol(begin, &end, 10);
        if (begin == end)
        {
            printf("%s", err_msg);
            return false;
        }
        
        begin = end;
        *max = strtol(begin, &end, 10);
        if (begin == end || *min > *max)
        {
            printf("%s", err_msg);
            return false;
        }

        begin = end;
        while (isspace((int)*begin))
            begin++;
        
        if (*begin != '\0')
        {
            printf("%s", err_msg);
            return false;
        }
    }

    return true;
}

static sparse_matrix_t imp__input_matrix_auto(void)
{
    uint32_t rows, cols;
    float percent;
    mat_elem_t min_elem = -100, max_elem = 100;
    sparse_matrix_t matrix = sp_null_matrix();
    if (uki_input_uint32_minmax("Введите число строк матрицы: ",
                                "Число строк должно быть положительным.\n", 1, 1 << 20, &rows) &&
        uki_input_uint32_minmax("Введите число столбцов матрицы: ",
                                "Число столбцов должно быть положительным.\n", 1, 1 << 20, &cols) &&
        imp__input_percent_bounds("Введите процент ненулевых элементов от 0 до 100 (и минимальный и максимальный элементы, если нужно): ",
                                  "Неверный ввод.\n", &percent, &min_elem, &max_elem))
    {
        matrix = sp_create(rows, cols);
        sp_randomize(&matrix, percent / 100.0f);
        for (uint32_t i = 0; i < matrix.nonzero_size; i++)
        {
            mat_elem_t value = min_elem + (float)rand() / RAND_MAX * (max_elem - min_elem);
            matrix.nonzero_array[i] = value;
        }
    }
    return matrix;
}
