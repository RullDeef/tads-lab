#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "../../sparse_matrix/include/sparse_matrix.h"
#include "con_menu.h"
#include "../include/conio.h"

// time calculations
#include <stdint.h> // <cstdint> is preferred in C++, but stdint.h works.

#ifdef _MSC_VER
#include <intrin.h>
#else
#include <x86intrin.h>
#endif

/*

    Меню:

    1. Умножение матрицы на вектор
    2. Умножение матрицы на матрицу
    3. Выход
*/

size_t imp__input_dim(void)
{
    char line[256];
    if (fgets(line, 256, stdin) == NULL)
        return 0;
    return strtoul(line, NULL, 10);
}

int input_mat_dims(size_t *rows, size_t *cols)
{
    printf("Введите число строк матрицы: ");

    *rows = imp__input_dim();
    if (*rows == 0)
    {
        printf("Введено неверное число строк.\n");
        return -1;
    }

    printf("Введите число столбцов матрицы: ");

    *cols = imp__input_dim();
    if (*cols == 0)
    {
        printf("Введено неверное число столбцов.\n");
        return -2;
    }

    return 0;
}

int input_triplet(size_t *row, size_t *col, double *value)
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

    return 0;
}

int input_duplet(size_t *row, double *value)
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
    *value = strtod(start, &end);
    if (end == start)
        return -1;

    return 0;
}

sparse_matrix_t input_matrix()
{
    sparse_matrix_t matrix = sp_null_matrix();
    size_t rows, cols;
    if (!input_mat_dims(&rows, &cols))
    {
        matrix = sp_create(rows, cols);
        printf("Вводите тройки чисел: <строка столбец элемент> для каждого ненулевого элемента в матрице.\n");

        size_t row, col;
        double value;
        while (!input_triplet(&row, &col, &value))
            sp_set(&matrix, row, col, value);
    }
    return matrix;
}

sparse_matrix_t input_matrix_constraint(size_t rows)
{
    sparse_matrix_t matrix = sp_null_matrix();

    printf("Введите число столбцов второй матрицы: ");
    size_t cols = imp__input_dim();
    if (cols == 0)
        printf("Введено неверное число столбцов.\n");
    else
    {
        matrix = sp_create(rows, cols);
        printf("Вводите тройки чисел: <строка столбец элемент> для каждого ненулевого элемента в матрице.\n");

        size_t row, col;
        double value;
        while (!input_triplet(&row, &col, &value))
            sp_set(&matrix, row, col, value);
    }
    return matrix;
}

sparse_matrix_t input_vector(size_t size)
{
    sparse_matrix_t vector = sp_create(size, 1);

    printf("Ввод значений умножаемого вектора размером %lu строк.\n", size);
    printf("Вводите пары чисел: <строка элемент> для каждого ненулевого элемента вектора.\n");

    size_t row;
    double value;
    while (!input_duplet(&row, &value))
        sp_set(&vector, row, 0, value);

    return vector;
}

int menu_mult_vec(void *data)
{
    sparse_matrix_t matrix = input_matrix();
    if (!sp_mat_is_null(&matrix))
    {
        sparse_matrix_t vector = input_vector(matrix.rows_size);
        
        printf("Выполняем умножение...\n");
        sparse_matrix_t result = sp_null_matrix();

        unsigned long long start = __rdtsc();
        sp_mult_matrix(&matrix, &vector, &result);
        unsigned long long end = __rdtsc();

        printf("Время обычного умножения: %llu.\n", end - start);

        start = __rdtsc();
        sp_mult_matrix_fast(&matrix, &vector, &result);
        end = __rdtsc();

        printf("Время специального умножения: %llu.\n", end - start);

        printf("Результат умножения:\n");
        sp_print_info(&result);

        sp_free(&result);
        sp_free(&vector);
    }
    sp_free(&matrix);
    con_wait();
    return 0;
}

int menu_mult_mat(void *data)
{
    sparse_matrix_t matrix_1 = input_matrix();
    if (!sp_mat_is_null(&matrix_1))
    {
        sparse_matrix_t matrix_2 = input_matrix_constraint(matrix_1.cols_size);

        sparse_matrix_t result = sp_null_matrix();
        printf("Выполняем умножение...\n");

        unsigned long long start = __rdtsc();
        sp_mult_matrix(&matrix_1, &matrix_2, &result);
        unsigned long long end = __rdtsc();

        printf("Время обычного умножения: %llu.\n", end - start);

        start = __rdtsc();
        sp_mult_matrix_fast(&matrix_1, &matrix_2, &result);
        end = __rdtsc();

        printf("Время специального умножения: %llu.\n", end - start);

        printf("Результат умножения:\n");
        sp_print_info(&result);

        sp_free(&result);
        sp_free(&matrix_2);
    }
    sp_free(&matrix_1);
    con_wait();
    return 0;
}

int menu_exit(void *data)
{
    printf("Выход из программы.\n");
    con_wait();
    return 1;
}

int main(void)
{
    menu_t menu = create_menu(3,
        "Умножение матрицы на вектор", menu_mult_vec,
        "Умножение матрицы на матрицу", menu_mult_mat,
        "Выход", menu_exit);

    run_menu_loop(&menu, NULL);
    return 0;
}
