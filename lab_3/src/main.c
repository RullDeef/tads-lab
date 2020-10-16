#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "sparse_matrix.h"
#include "con_menu.h"
#include "conio.h"

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

int input_auto_mat_dims(size_t *dim1, size_t *dim2, size_t *dim3)
{
    printf("Введите число строк первой матрицы: ");

    *dim1 = imp__input_dim();
    if (*dim1 == 0)
    {
        printf("Введено неверное число строк.\n");
        return -1;
    }

    printf("Введите число столбцов первой матрицы: ");

    *dim2 = imp__input_dim();
    if (*dim2 == 0)
    {
        printf("Введено неверное число столбцов.\n");
        return -2;
    }

    printf("Введите число столбцов второй матрицы: ");

    *dim3 = imp__input_dim();
    if (*dim3 == 0)
    {
        printf("Введено неверное число столбцов.\n");
        return -3;
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
        sparse_matrix_t vector = input_vector(matrix.cols_size);
        
        printf("Введены данные:\n");
        sp_print_info(&matrix);
        sp_print_info(&vector);

        printf("Выполняем умножение...\n");
        sparse_matrix_t result = sp_null_matrix();

        unsigned long long start = __rdtsc();
        int res = sp_mult_matrix(&matrix, &vector, &result);
        unsigned long long end = __rdtsc();

        if (res != 0)
            printf("Ошибки при умножении! Код %d\n", res);
        else
        {
            printf("Результат умножения:\n");
            sp_print_info(&result);
        }

        printf("Время обычного умножения: %llu.\n", end - start);

        start = __rdtsc();
        res = sp_mult_matrix_fast(&matrix, &vector, &result);
        end = __rdtsc();

        printf("Время специального умножения: %llu.\n", end - start);

        if (res != 0)
            printf("Ошибки при умножении! Код %d\n", res);
        else
        {
            printf("Результат умножения:\n");
            sp_print_info(&result);
        }

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

int read_sparse_percent(float *percent)
{
    char line[256];
    if (fgets(line, 256, stdin) == NULL)
        return 0;
    
    char *end = NULL;
    *percent = strtof(line, &end);
    if (line == end)
        return -1;
    
    if (*percent <= 0.0 || *percent > 100)
        return -2;
    
    return 0;
}

int menu_mult_auto(void *data)
{
    printf("Переход в режим полуавтоматического тестирования.\n");
    
    size_t rows1;
    size_t cols1;
    size_t cols2;
    if (!input_auto_mat_dims(&rows1, &cols1, &cols2))
    {
        sparse_matrix_t matrix_1 = sp_create(rows1, cols1);
        sparse_matrix_t matrix_2 = sp_create(cols1, cols2);

        sparse_matrix_t result = sp_null_matrix();

        char table_data[22][4][64] = {};
        memset(table_data, '\0', sizeof(table_data));

        strcpy(table_data[0][0], "sparse %");
        strcpy(table_data[0][1], "slow time");
        strcpy(table_data[0][2], "fast time");
        strcpy(table_data[0][3], "efficiency %");

        size_t table_row = 1;
        unsigned long long time_1, time_2;
        for (int percent = 0; percent < 100; percent += 5)
        {
            sp_randomize(&matrix_1, percent / 100.0f);
            sp_randomize(&matrix_2, percent / 100.0f);

            // slow method
            time_1 = __rdtsc();
            sp_mult_matrix(&matrix_1, &matrix_2, &result);
            time_1 = __rdtsc() - time_1;

            // fast method
            time_2 = __rdtsc();
            sp_mult_matrix_fast(&matrix_1, &matrix_2, &result);
            time_2 = __rdtsc() - time_2;

            // calc efficiency
            float eff = (long double)(time_1 - time_2) / time_1 * 100.0f;

            sprintf(table_data[table_row][0], "%d", percent);
            sprintf(table_data[table_row][1], "%llu", time_1);
            sprintf(table_data[table_row][2], "%llu", time_2);
            sprintf(table_data[table_row][3], "%.0f", eff);
            table_row++;
            // printf("Процент заполнения: %3d%%, процент эффективности быстрого алгоритма: %6.2f%%\n", percent, eff);
        }

        con_print_table(table_row, 4, "Speed test",
            table_data[0][0], table_data[0][1], table_data[0][2], table_data[0][3],
            table_data[1][0], table_data[1][1], table_data[1][2], table_data[1][3],
            table_data[2][0], table_data[2][1], table_data[2][2], table_data[2][3],
            table_data[3][0], table_data[3][1], table_data[3][2], table_data[3][3],
            table_data[4][0], table_data[4][1], table_data[4][2], table_data[4][3],
            table_data[5][0], table_data[5][1], table_data[5][2], table_data[5][3],
            table_data[6][0], table_data[6][1], table_data[6][2], table_data[6][3],
            table_data[7][0], table_data[7][1], table_data[7][2], table_data[7][3],
            table_data[8][0], table_data[8][1], table_data[8][2], table_data[8][3],
            table_data[9][0], table_data[9][1], table_data[9][2], table_data[9][3],
            table_data[10][0], table_data[10][1], table_data[10][2], table_data[10][3],
            table_data[11][0], table_data[11][1], table_data[11][2], table_data[11][3],
            table_data[12][0], table_data[12][1], table_data[12][2], table_data[12][3],
            table_data[13][0], table_data[13][1], table_data[13][2], table_data[13][3],
            table_data[14][0], table_data[14][1], table_data[14][2], table_data[14][3],
            table_data[15][0], table_data[15][1], table_data[15][2], table_data[15][3],
            table_data[16][0], table_data[16][1], table_data[16][2], table_data[16][3],
            table_data[17][0], table_data[17][1], table_data[17][2], table_data[17][3],
            table_data[18][0], table_data[18][1], table_data[18][2], table_data[18][3],
            table_data[19][0], table_data[19][1], table_data[19][2], table_data[19][3],
            table_data[20][0], table_data[20][1], table_data[20][2], table_data[20][3],
            table_data[21][0], table_data[21][1], table_data[21][2], table_data[21][3],
            table_data[22][0], table_data[22][1], table_data[22][2], table_data[22][3]
        );

        sp_free(&matrix_1);
        sp_free(&matrix_2);
        sp_free(&result);
    }

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
    menu_t menu = create_menu(4,
        "Умножение матрицы на вектор", menu_mult_vec,
        "Умножение матрицы на матрицу", menu_mult_mat,
        "Умножение матриц (автоматическое)", menu_mult_auto,
        "Выход", menu_exit);

    run_menu_loop(&menu, NULL);
    return 0;
}
