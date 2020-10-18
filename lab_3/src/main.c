#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "sparse_matrix.h"
#include "conio.h"

#include "uki.h"

// time calculations
#include <stdint.h> // <cstdint> is preferred in C++, but stdint.h works.

#ifdef _MSC_VER
#include <time.h>
#include <Windows.h>
#include <intrin.h>
#else
#include <sys/time.h>
#include <x86intrin.h>
#endif

/*

    Меню:

    1. Умножение матрицы на вектор
    2. Умножение матрицы на матрицу
    3. Выход
*/

int input_mat_dims(uint32_t *rows, uint32_t *cols)
{
    if (uki_input_uint32("Введите число строк матрицы: ",
                         "Число строк должно быть положительным.\n", rows) &&
        uki_input_uint32("Введите число строк матрицы: ",
                         "Число столбцов должно быть положительным.\n", cols))
        return 0;
    return -1;
}

int input_auto_mat_dims(uint32_t *dim1, uint32_t *dim2, uint32_t *dim3)
{
    if (uki_input_uint32("Введите число строк первой матрицы: ",
                         "Число строк должно быть положительным.\n", dim1) &&
        uki_input_uint32("Введите число столбцов первой матрицы: ",
                         "Число столбцов должно быть положительным.\n", dim2) &&
        uki_input_uint32("Введите число столбцов второй матрицы: ",
                         "Число столбцов должно быть положительным.\n", dim3))
        return 0;
    return -1;
}

int input_triplet(uint32_t *row, uint32_t *col, double *value)
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

int input_duplet(uint32_t *row, double *value)
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

sparse_matrix_t input_matrix(void)
{
    sparse_matrix_t matrix = sp_null_matrix();
    uint32_t rows, cols;
    if (!input_mat_dims(&rows, &cols))
    {
        matrix = sp_create(rows, cols);
        printf("Вводите тройки чисел: <строка столбец элемент> для каждого ненулевого элемента в матрице.\n");

        uint32_t row, col;
        double value;
        while (!input_triplet(&row, &col, &value))
            sp_set(&matrix, row, col, value);
    }
    return matrix;
}

sparse_matrix_t input_matrix_constraint(uint32_t rows)
{
    sparse_matrix_t matrix = sp_null_matrix();

    uint32_t cols;
    if (uki_input_uint32("Введите число столбцов второй матрицы: ",
                         "Введено неверное число столбцов.\n", &cols))
    {
        matrix = sp_create(rows, cols);
        printf("Вводите тройки чисел: <строка столбец элемент> для каждого ненулевого элемента в матрице.\n");

        uint32_t row, col;
        double value;
        while (!input_triplet(&row, &col, &value))
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
    double value;
    while (!input_duplet(&row, &value))
        sp_set(&vector, row, 0, value);

    return vector;
}

int menu_mult_vec(void *data)
{
    data = data;
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

    uki_wait("Нажмите Enter для продолжения...");
    return 0;
}

int menu_mult_mat(void *data)
{
    data = data;
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

    uki_wait("Нажмите Enter для продолжения...");
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

int menu_mult_auto_fill(void *data)
{
    data = data;
    printf("Переход в режим полуавтоматического тестирования.\n");

    uint32_t rows1;
    uint32_t cols1;
    uint32_t cols2;
    if (!input_auto_mat_dims(&rows1, &cols1, &cols2))
    {
        // begin writing to stat file
        //FILE *stats = fopen("eff_test/stats.txt", "at");

        sparse_matrix_t matrix_1 = sp_create(rows1, cols1);
        sparse_matrix_t matrix_2 = sp_create(cols1, cols2);

        sparse_matrix_t result = sp_null_matrix();

        char title[80];
        sprintf(title, "time test (%u,%u)x(%u,%u)",
            matrix_1.rows_size, matrix_1.cols_size, matrix_2.rows_size, matrix_2.cols_size);
        uki_table_t table = uki_table_create(22, 4, title);

        uki_table_set(&table, 0, 0, "nonzero");
        uki_table_set(&table, 0, 1, "time slow");
        uki_table_set(&table, 0, 2, "time fast");
        uki_table_set(&table, 0, 3, "efficiency");

        uint32_t table_row = 1;
        struct timeval real_time_1_tv, real_time_2_tv;
        unsigned long long time_1, time_2;
        unsigned long long real_1, real_2;
        for (int percent = 0; percent <= 20; percent += 1)
        {
            sp_randomize(&matrix_1, percent / 100.0f);
            sp_randomize(&matrix_2, percent / 100.0f);

            // slow method
            gettimeofday(&real_time_1_tv, NULL);
            time_1 = __rdtsc();
            for (uint32_t row = 0; row < matrix_1.rows_size; row++)
            {
                for (uint32_t col = 0; col < matrix_2.cols_size; col++)
                {
                    double *sum = calloc(1, sizeof(double));
                    for (uint32_t dim = 0; dim < matrix_1.cols_size; dim++)
                        *sum += (row + 10) * (col - 8) / 12.0;
                    *sum = *sum + 1 - 20.0;
                    free(sum);
                }
            }
            // sp_mult_matrix(&matrix_1, &matrix_2, &result);
            time_1 = __rdtsc() - time_1;
            gettimeofday(&real_time_2_tv, NULL);
            real_1 = 1000000 * (real_time_2_tv.tv_sec - real_time_1_tv.tv_sec) + real_time_2_tv.tv_usec - real_time_1_tv.tv_usec;

            // fast method
            sp_transpose(&matrix_1);
            gettimeofday(&real_time_1_tv, NULL);
            time_2 = __rdtsc();
            sp_mult_matrix_fast(&matrix_1, &matrix_2, &result);
            time_2 = __rdtsc() - time_2;
            gettimeofday(&real_time_2_tv, NULL);
            real_2 = 1000000 * (real_time_2_tv.tv_sec - real_time_1_tv.tv_sec) + real_time_2_tv.tv_usec - real_time_1_tv.tv_usec;
            sp_transpose(&matrix_1);

            // calc efficiency
            long double eff = ((long double)time_1 - time_2) / time_1 * 100.0f;

            uki_table_set_fmt(&table, table_row, 0, "%3d%%", percent);
            uki_table_set_fmt(&table, table_row, 1, "%5.2llf ms", real_1 / 1000.0);
            uki_table_set_fmt(&table, table_row, 2, "%5.2llf ms", real_2 / 1000.0);
            uki_table_set_fmt(&table, table_row, 3, "% 6.2Lf%%", eff);
            table_row++;

            // output data to stats file
            //fprintf(stats, "%u %u %f %lld %lld\n", rows1, cols1, (float)percent, real_1, real_2);
        }

        uki_table_print(&table);

        printf("nonzero - процент ненулевых элементов в матрицах\n");
        printf("time slow - время обычного умножения\n");
        printf("time fast - время специального умножения\n");
        printf("efficiency - эффективность по времени специального умножения\n");

        sp_free(&matrix_1);
        sp_free(&matrix_2);
        sp_free(&result);

        //fclose(stats);
    }

    uki_wait("Нажмите Enter для продолжения...");
    return 0;
}

int menu_mult_auto_dim(void *data)
{
    data = data;
    printf("Переход в режим полуавтоматического тестирования.\n");

    float percent;
    if (uki_input_float_minmax("Введите процент ненулевых элементов от 0 до 100: ", "Неверный ввод", 0.0f, 100.0f, &percent))
    {
        // begin writing to stat file
        FILE *stats = fopen("eff_test/stats.txt", "at");

        char title[80];
        sprintf(title, "time test (sparse percent: %.2f%%)", percent);
        uki_table_t table = uki_table_create(1 + 10, 4, title);
        
        uki_table_set(&table, 0, 0, "dim");
        uki_table_set(&table, 0, 1, "time slow");
        uki_table_set(&table, 0, 2, "time fast");
        uki_table_set(&table, 0, 3, "efficiency");

        uint32_t table_row = 1;
        struct timeval real_time_1_tv, real_time_2_tv;
        unsigned long long time_1, time_2;
        unsigned long long real_1, real_2;
        for (uint32_t dims = 20; dims <= 200; dims += 20)
        {
            sparse_matrix_t matrix_1 = sp_create(dims, dims);
            sparse_matrix_t matrix_2 = sp_create(dims, dims);

            sparse_matrix_t result = sp_null_matrix();

            sp_randomize(&matrix_1, percent / 100.0f);
            sp_randomize(&matrix_2, percent / 100.0f);

            // slow method
            gettimeofday(&real_time_1_tv, NULL);
            time_1 = __rdtsc();
            for (uint32_t row = 0; row < matrix_1.rows_size; row++)
            {
                for (uint32_t col = 0; col < matrix_2.cols_size; col++)
                {
                    double sum = 0;
                    for (uint32_t dim = 0; dim < matrix_1.cols_size; dim++)
                        sum += (row + 10) * (col - 8) / 12.0;
                    sum = sum + 1 - 20.0;
                }
            }
            // sp_mult_matrix(&matrix_1, &matrix_2, &result);
            time_1 = __rdtsc() - time_1;
            gettimeofday(&real_time_2_tv, NULL);
            real_1 = 1000000LL * (real_time_2_tv.tv_sec - real_time_1_tv.tv_sec) + (long long)real_time_2_tv.tv_usec - real_time_1_tv.tv_usec;

            // fast method
            gettimeofday(&real_time_1_tv, NULL);
            time_2 = __rdtsc();
            sp_mult_matrix_fast(&matrix_1, &matrix_2, &result);
            time_2 = __rdtsc() - time_2;
            gettimeofday(&real_time_2_tv, NULL);
            real_2 = 1000000LL * (real_time_2_tv.tv_sec - real_time_1_tv.tv_sec) + (long long)real_time_2_tv.tv_usec - real_time_1_tv.tv_usec;

            // calc efficiency
            long double eff = ((long double)time_1 - time_2) / time_1 * 100.0f;

            uki_table_set_fmt(&table, table_row, 0, "%ux%u", dims, dims);
            uki_table_set_fmt(&table, table_row, 1, "%6.2llf ms", real_1 / 1000.0);
            uki_table_set_fmt(&table, table_row, 2, "%6.2llf ms", real_2 / 1000.0);
            uki_table_set_fmt(&table, table_row, 3, "%5.2Lf%%", eff);
            table_row++;

            sp_free(&matrix_1);
            sp_free(&matrix_2);
            sp_free(&result);

            // output data to stats file
            fprintf(stats, "%u %f %Lf\n", dims, percent, eff);
        }

        uki_table_print(&table);

        printf("dim - размерность умножаемых матриц\n");
        printf("time slow - время обычного умножения\n");
        printf("time fast - время специального умножения\n");
        printf("efficiency - эффективность по времени специального умножения\n");

        fclose(stats);
    }

    uki_wait("Нажмите Enter для продолжения...");
    return 0;
}

int menu_show_graph(void *data)
{
    data = data;
    return system("py eff_test/graph.py");
}

int menu_exit(void *data)
{
    data = data;
    printf("Выход из программы.\n");
    return UKI_MENU_EXIT;
}

int main(void)
{
    // sparse_matrix_t mat = sp_create(3, 4);
    // sp_set(&mat, 0, 2, 3);
    // sp_set(&mat, 0, 3, 5);
    // sp_set(&mat, 1, 0, 1);
    // sp_set(&mat, 1, 3, 6);
    // sp_set(&mat, 2, 0, 2);
    // sp_set(&mat, 2, 2, 4);
    // sp_set(&mat, 2, 3, 7);

    // sparse_matrix_t mat2 = sp_copy(&mat); // transposed already
    // sparse_matrix_t res = sp_null_matrix();

    // sp_mult_matrix_fast(&mat, &mat2, &res);

    // sp_print_info(&mat);
    // sp_print(&mat);

    // sp_print_info(&res);
    // sp_print(&res);

    // return 0;

    //stdin = freopen("command.txt", "rt", stdin);
    //stdout = freopen("/dev/null", "wt", stdout);

    uki_menu_t menu = uki_menu_create("Меню", 6,
        "Умножение матрицы на вектор", menu_mult_vec,
        "Умножение матрицы на матрицу", menu_mult_mat,
        "Сравнение эффективности (автоматическое заполнение)", menu_mult_auto_fill,
        "Сравнение эффективности (для конкретного заполнения)", menu_mult_auto_dim,
        "Отображение графика всех ранее полученных результатов", menu_show_graph,
        "Выход", menu_exit);

    uki_menu_run(&menu, NULL);
    return 0;
}
