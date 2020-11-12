#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "uki.h"
#include "core/matrix.h"
#include "conio.h"
#include "matrix_input.h"

#include "utils/timing.h"
#include "multiplicator.h"

/*
    Меню:

    1. Умножение матрицы на вектор
    2. Умножение матрицы на матрицу
    3. Выход
*/

int menu_mult_vec(void *data)
{
    (void)data;
    sparse_matrix_t matrix = ultimate_input_matrix();
    if (!sp_mat_is_null(&matrix))
    {
        sparse_matrix_t vector = input_vector(matrix.cols_size);

        printf("Введена матрица:\n");
        sp_print_info(&matrix);

        printf("\nВведен вектор:\n");
        sp_print_info(&vector);

        printf("\nВыполняем умножение...\n");

        unsigned long long time_1, time_2;
        sparse_matrix_t result = sp_null_matrix();
        int res = mult_mats(&matrix, &vector, &result, &time_1, &time_2, NULL, NULL);

        if (res != 0)
            printf("Ошибки при умножении.\n");
        else
        {
            printf("Результат умножения:\n");
            sp_print_info(&result);

            printf("\nВремя обычного умножения: %llu тактов.\n", time_1);
            printf("Время специального умножения: %llu тактов.\n", time_2);
            printf("Эффективность по времени: %.2f%%\n\n", (100.0f * ((float)time_1 - time_2) / (time_1)));
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
    (void)data;
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

                printf("\nВыполняем умножение...\n");

                unsigned long long time_1, time_2;
                sparse_matrix_t result = sp_null_matrix();
                int res = mult_mats(&matrix_1, &matrix_2, &result, &time_1, &time_2, NULL, NULL);

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

int menu_show_table(void *data)
{
    (void)data;
    printf("Переход в режим вывода таблицы.\n");

    const uint32_t dim_delta = 50;
    const uint32_t pc_delta = 5;

    const uint32_t rows = 8, cols = 7;

    uki_table_t table = uki_table_create(1 + rows, 1 + cols, "speed efficiency table (%)");
    uki_table_set(&table, 0, 0, "dims\\sparse %");

    for (uint32_t col = 1; col <= cols; col++)
        uki_table_set_fmt(&table, 0, col, "<[%2d%%]>", pc_delta * col);

    for (uint32_t row = 1; row <= rows; row++)
        uki_table_set_fmt(&table, row, 0, "%d", dim_delta * row);

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

            sp_randomize(&matrix_1, percent / 100.0f);
            sp_randomize(&matrix_2, percent / 100.0f);

            unsigned long long time_1, time_2;
            sparse_matrix_t result = sp_null_matrix();
            mult_mats(&matrix_1, &matrix_2, &result, &time_1, &time_2, NULL, NULL);

            // calc efficiency
            float eff_speed = ((float)time_1 - time_2) / time_1 * 100.0f;

            // print it on table
            uki_table_set_fmt(&table, row, col, "%5.2f", eff_speed);

            sp_free(&matrix_1);
            sp_free(&matrix_2);
            sp_free(&result);

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

int menu_show_table_mem(void *data)
{
    (void)data;
    printf("Переход в режим вывода таблицы.\n");

    const uint32_t row_delta = 50;
    const uint32_t col_delta = 50;

    const uint32_t rows = 20, cols = 20;

    uki_table_t table = uki_table_create(1 + rows, 1 + cols, "memory efficiency table (%)");
    uki_table_set(&table, 0, 0, "N\\M");

    for (uint32_t col = 1; col <= cols; col++)
        uki_table_set_fmt(&table, 0, col, "[%d]", col_delta * col);

    for (uint32_t row = 1; row <= rows; row++)
        uki_table_set_fmt(&table, row, 0, "[%d]", row_delta * row);

    FILE *out = fopen("./eff_test/stats2.txt", "wt");
    for (uint32_t row = 1; row <= rows; row++)
    {
        for (uint32_t col = 1; col <= cols; col++)
        {
            printf("%3d%%...", 100 * (col + row * (cols + 1)) / ((rows + 1) * (cols + 1)));
            fflush(stdout);

            uint32_t real_row = row * row_delta;
            uint32_t real_col = col * col_delta;

            float prev_eff;
            float percent;
            float eff = 0.0;
            
            for (percent = 0.0f; percent < 1.0f; percent += 0.0001f)
            {
                prev_eff = eff;

                // calc efficiency
                eff = mat_mem_eff(real_row, real_col, percent);

                if (percent > 0.0f && (eff * prev_eff <= 0.0f))
                    break;
            }

            // print it on table
            uki_table_set_fmt(&table, row, col, "%5.2f", 100.0f * percent);
            fprintf(out, "%d %d %f\n", real_row, real_col, percent);

            printf("\b\b\b\b\b\b\b       \b\b\b\b\b\b\b");
            fflush(stdout);
        }
    }
    fclose(out);

    uki_table_print(&table);

    return 0;
}

int menu_show_graph(void *data)
{
    (void)data;
    return system("py eff_test/graph.py");
}

int menu_exit(void *data)
{
    (void)data;
    printf("Выход из программы.\n");
    return UKI_MENU_EXIT;
}

int main(void)
{
    uki_menu_t menu = uki_menu_create("Меню", 5,
        "Умножение матрицы на вектор", menu_mult_vec,
        "Умножение матрицы на матрицу", menu_mult_mat,
        "Отображение таблицы для сравнения эффективности по времени", menu_show_table,
        "Отображение таблицы для сравнения эффективности по памяти", menu_show_table_mem,
        /* "Отображение графика всех ранее полученных результатов", menu_show_graph, */
        "Выход", menu_exit);

    uki_menu_run(&menu, NULL);
    return 0;
}
