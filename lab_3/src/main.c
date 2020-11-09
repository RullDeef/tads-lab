#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "uki.h"
#include "dense_matrix.h"
#include "sparse_matrix.h"
#include "conio.h"
#include "matrix_input.h"
#include "converter.h"


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

int menu_mult_vec(void *data)
{
    data = data;
    sparse_matrix_t matrix = ultimate_input_matrix();
    if (!sp_mat_is_null(&matrix))
    {
        sparse_matrix_t vector = input_vector(matrix.cols_size);

        printf("Введена матрица:\n");
        sp_print_info(&matrix);

        printf("\nВведен вектор:\n");
        sp_print_info(&vector);

        printf("\nВыполняем умножение...\n");
        sparse_matrix_t result = sp_null_matrix();

        unsigned long long start, time_1, time_2;
        { // dense variant
            dense_matrix_t mat_1 = cv_dense_from_sparse(&matrix);
            dense_matrix_t mat_2 = cv_dense_from_sparse(&vector);
            dense_matrix_t res = dn_null_matrix();

            start = __rdtsc();
            dn_mult_matrix(&mat_1, &mat_2, &res);
            time_1 = __rdtsc() - start;

            result = cv_sparse_from_dense(&res);

            dn_free(&mat_1);
            dn_free(&mat_2);
            dn_free(&res);
        }

        sp_transpose(&matrix);
        start = __rdtsc();
        int res = sp_mult_matrix(&matrix, &vector, &result);
        time_2 = __rdtsc() - start;

        if (res != 0)
            printf("Ошибки при умножении.\n");
        else
        {
            printf("Результат умножения:\n");
            sp_print_info(&result);

            printf("\nВремя обычного умножения: %llu тактов.\n", time_1);
            printf("Время специального умножения: %llu тактов.\n", time_2);
            printf("Эффективность: %.2f%%\n\n", (100.0f * ((float)time_1 - time_2) / (time_1)));
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
    sparse_matrix_t matrix_1 = ultimate_input_matrix();
    if (!sp_mat_is_null(&matrix_1))
    {
        printf("Введите вторую матрицу:\n");
        sparse_matrix_t matrix_2 = ultimate_input_matrix();
        if (!sp_mat_is_null(&matrix_2))
        {
            if (matrix_1.cols_size != matrix_2.rows_size)
                printf("Размеры матриц не удовлетворяют правилу умножения матриц.\n");
            else
            {
                printf("Введены матрицы:\n\n");
                sp_print_info(&matrix_1);
                sp_print_info(&matrix_2);

                sparse_matrix_t result = sp_null_matrix();
                printf("\nВыполняем умножение...\n");

                unsigned long long start, time_1, time_2;
                { // dense variant
                    dense_matrix_t mat_1 = cv_dense_from_sparse(&matrix_1);
                    dense_matrix_t mat_2 = cv_dense_from_sparse(&matrix_2);
                    dense_matrix_t res = dn_null_matrix();

                    start = __rdtsc();
                    dn_mult_matrix(&mat_1, &mat_2, &res);
                    time_1 = __rdtsc() - start;

                    result = cv_sparse_from_dense(&res);

                    dn_free(&mat_1);
                    dn_free(&mat_2);
                    dn_free(&res);
                }

                sp_transpose(&matrix_1);
                start = __rdtsc();
                int res = sp_mult_matrix(&matrix_1, &matrix_2, &result);
                time_2 = __rdtsc() - start;

                if (res != 0)
                    printf("Ошибки при умножении.\n");
                else
                {
                    printf("Результат умножения (разреженного):\n");
                    sp_print_info(&result);

                    printf("\nВремя обычного умножения: %llu тактов.\n", time_1);
                    printf("Время специального умножения: %llu тактов.\n", time_2);
                    printf("Эффективность: %.2f%%\n\n", (100.0f * ((float)time_1 - time_2) / (time_1)));
                }

                sp_free(&result);
            }
            sp_free(&matrix_2);
        }
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
        FILE *stats = fopen("eff_test/stats.txt", "at");

        sparse_matrix_t matrix_1 = sp_create(rows1, cols1);
        sparse_matrix_t matrix_2 = sp_create(cols1, cols2);

        sparse_matrix_t result = sp_null_matrix();

        char title[80];
        sprintf(title, "time test (%u,%u)x(%u,%u)",
            matrix_1.rows_size, matrix_1.cols_size, matrix_2.rows_size, matrix_2.cols_size);
        uki_table_t table = uki_table_create(22, 4, title);

        uki_table_set(&table, 0, 0, "nonzero");
        uki_table_set(&table, 0, 1, "dense");
        uki_table_set(&table, 0, 2, "sparse");
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
            { // dense variant
                dense_matrix_t mat_1 = cv_dense_from_sparse(&matrix_1);
                dense_matrix_t mat_2 = cv_dense_from_sparse(&matrix_2);
                dense_matrix_t res = dn_null_matrix();

                gettimeofday(&real_time_1_tv, NULL);
                time_1 = __rdtsc();
                dn_mult_matrix(&mat_1, &mat_2, &res);
                time_1 = __rdtsc() - time_1;
                gettimeofday(&real_time_2_tv, NULL);
                real_1 = 1000000 * (real_time_2_tv.tv_sec - real_time_1_tv.tv_sec) + real_time_2_tv.tv_usec - real_time_1_tv.tv_usec;

                dn_free(&mat_1);
                dn_free(&mat_2);
                dn_free(&res);
            }

            // fast method
            sp_transpose(&matrix_1);
            gettimeofday(&real_time_1_tv, NULL);
            time_2 = __rdtsc();
            sp_mult_matrix(&matrix_1, &matrix_2, &result);
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
            // fprintf(stats, "%u %u %f %lld %lld\n", rows1, cols1, (float)percent, real_1, real_2);
        }

        uki_table_print(&table);

        printf("nonzero - процент ненулевых элементов в матрицах\n");
        printf("dense - время обычного умножения\n");
        printf("sparse - время специального умножения\n");
        printf("efficiency - эффективность по времени специального умножения\n");

        sp_free(&matrix_1);
        sp_free(&matrix_2);
        sp_free(&result);

        fclose(stats);
    }

    uki_wait("Нажмите Enter для продолжения...");
    return 0;
}

int menu_mult_auto_dim(void *data)
{
    (void)data;
    printf("Переход в режим полуавтоматического тестирования.\n");

    float percent;
    if (uki_input_float_minmax("Введите процент ненулевых элементов от 1 до 100: ", "Неверный ввод\n", 0.0f, 100.0f, &percent))
    {
        // begin writing to stat file
        FILE *stats = fopen("eff_test/stats.txt", "at");

        char title[80];
        sprintf(title, "time test (sparse percent: %.2f%%)", percent);
        uki_table_t table = uki_table_create(1 + 10, 4, title);
        
        uki_table_set(&table, 0, 0, "dim");
        uki_table_set(&table, 0, 1, "dense");
        uki_table_set(&table, 0, 2, "sparse");
        uki_table_set(&table, 0, 3, "efficiency");

        uint32_t table_row = 1;
        struct timeval real_time_1_tv, real_time_2_tv;
        unsigned long long time_1, time_2;
        unsigned long long real_1, real_2;
        for (uint32_t dims = 20; dims <= 200; dims += 20)
        {
            printf("%3d%%...", (int)(dims / 2.2));
            fflush(stdout);

            sparse_matrix_t matrix_1 = sp_create(dims, dims);
            sparse_matrix_t matrix_2 = sp_create(dims, dims);

            sparse_matrix_t result = sp_null_matrix();

            sp_randomize(&matrix_1, percent / 100.0f);
            sp_randomize(&matrix_2, percent / 100.0f);

            // slow method
            { // dense variant
                dense_matrix_t mat_1 = cv_dense_from_sparse(&matrix_1);
                dense_matrix_t mat_2 = cv_dense_from_sparse(&matrix_2);
                dense_matrix_t res = dn_null_matrix();

                gettimeofday(&real_time_1_tv, NULL);
                time_1 = __rdtsc();
                dn_mult_matrix(&mat_1, &mat_2, &res);
                time_1 = __rdtsc() - time_1;
                gettimeofday(&real_time_2_tv, NULL);
                real_1 = 1000000LL * (real_time_2_tv.tv_sec - real_time_1_tv.tv_sec) + (long long)real_time_2_tv.tv_usec - real_time_1_tv.tv_usec;

                dn_free(&mat_1);
                dn_free(&mat_2);
                dn_free(&res);
            }

            // fast method
            gettimeofday(&real_time_1_tv, NULL);
            time_2 = __rdtsc();
            sp_mult_matrix(&matrix_1, &matrix_2, &result);
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
            // fprintf(stats, "%u %f %Lf\n", dims, percent, eff);

            printf("\b\b\b\b\b\b\b       \b\b\b\b\b\b\b");
            fflush(stdout);
        }

        uki_table_print(&table);

        printf("dim - размерность умножаемых матриц\n");
        printf("dense - время обычного умножения\n");
        printf("sparse - время специального умножения\n");
        printf("efficiency - эффективность по времени специального умножения\n");

        fclose(stats);
    }

    uki_wait("Нажмите Enter для продолжения...");
    return 0;
}

int menu_show_table(void *data)
{
    (void)data;
    printf("Переход в режим вывода таблицы.\n");

    const uint32_t dim_delta = 50;
    const uint32_t pc_delta = 5;

    const uint32_t rows = 6, cols = 7;

    char title[] = "efficiency table (%)";
    uki_table_t table = uki_table_create(1 + rows, 1 + cols, title);
    uki_table_set(&table, 0, 0, "dims\\sparse %");

    for (uint32_t col = 1; col <= cols; col++)
    {
        char str[30];
        sprintf(str, "<[%2d%%]>", pc_delta * col);
        uki_table_set(&table, 0, col, str);
    }

    for (uint32_t row = 1; row <= rows; row++)
    {
        char str[10];
        sprintf(str, "%d", dim_delta * row);
        uki_table_set(&table, row, 0, str);
    }

    for (uint32_t row = 1; row <= rows; row++)
    {
        for (uint32_t col = 1; col <= cols; col++)
        {
            printf("%3d%%...", 100 * (col + row * (cols + 1)) / ((rows + 1) * (cols + 1)));
            fflush(stdout);

            uint32_t percent = pc_delta * col;
            uint32_t dims = dim_delta * row;

            sparse_matrix_t matrix_1 = sp_create(dims, dims);
            sparse_matrix_t matrix_2 = sp_create(dims, dims);

            sparse_matrix_t result = sp_null_matrix();

            sp_randomize(&matrix_1, percent / 100.0f);
            sp_randomize(&matrix_2, percent / 100.0f);

            unsigned long long time_1, time_2;

            // slow method
            { // dense variant
                dense_matrix_t mat_1 = cv_dense_from_sparse(&matrix_1);
                dense_matrix_t mat_2 = cv_dense_from_sparse(&matrix_2);
                dense_matrix_t res = dn_null_matrix();

                time_1 = __rdtsc();
                dn_mult_matrix(&mat_1, &mat_2, &res);
                time_1 = __rdtsc() - time_1;

                dn_free(&mat_1);
                dn_free(&mat_2);
                dn_free(&res);
            }

            // fast method
            time_2 = __rdtsc();
            sp_mult_matrix(&matrix_1, &matrix_2, &result);
            time_2 = __rdtsc() - time_2;

            // calc efficiency
            long double eff = ((long double)time_1 - time_2) / time_1 * 100.0f;

            // print it on table
            uki_table_set_fmt(&table, row, col, "%5.2Lf", eff);

            printf("\b\b\b\b\b\b\b       \b\b\b\b\b\b\b");
            fflush(stdout);
        }
    }

    uki_table_print(&table);

    printf("В первой колонке указывается размерность перемножаемых матриц.\n");
    printf("В первой строке указывается процент разреженности матриц (процент ненулевых элементов).\n");
    printf("В ячейках таблицы указан процент, показывающий на сколько разреженная матрица работает быстрее плотной.\n");
    printf("Если это число отрицательно - значит разреженная матрица является менее эффективной.\n\n");

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
    uki_menu_t menu = uki_menu_create("Меню", 7,
        "Умножение матрицы на вектор", menu_mult_vec,
        "Умножение матрицы на матрицу", menu_mult_mat,
        "Сравнение эффективности (автоматическое заполнение)", menu_mult_auto_fill,
        "Сравнение эффективности (для конкретного заполнения)", menu_mult_auto_dim,
        "Отображение сравнительной таблицы для сравнения эффективности", menu_show_table,
        "Отображение графика всех ранее полученных результатов", menu_show_graph,
        "Выход", menu_exit);

    uki_menu_run(&menu, NULL);
    return 0;
}
