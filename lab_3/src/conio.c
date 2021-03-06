#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <errno.h>
#include "conio.h"

int imp__get_indices_pair(uint32_t rows, uint32_t cols, uint32_t *row, uint32_t *col)
{
    char temp[256];
    if (fgets(temp, 256, stdin) != temp)
        return -1;

    char *end;

    *row = strtoul(temp, &end, 10);
    if (*row == 0 && (errno == EINVAL || end == temp))
        return -2;

    while (*end == ' ' || *end == '\t' || *end == '\r' || *end == '\n')
        end++;

    *col = strtoul(end, &end, 10);
    if (*col == 0 && (errno == EINVAL || end == temp))
        return -2;

    while (*end == ' ' || *end == '\t' || *end == '\r' || *end == '\n')
        end++;

    if (*end != '\0')
        return -3;

    if (*row >= rows || *col >= cols)
        return -3;

    return 0;
}

inline void con_clear()
{
    system("clear");
}

inline void con_print_menu_header()
{
    printf("-- [меню] --\n\n");
}

inline void con_print_opt(int opt, const char *text)
{
    printf("  %2d. %s\n", opt, text);
}

inline void con_print_menu_footer()
{
    printf("\n");
}

void con_wait(void)
{
    char temp[2];
    fgets(temp, 2, stdin);
}

bool con_confirm(bool default_yes)
{
    printf(default_yes ? "(Y/n): " : "(y/N): ");

    char line[64];
    if (fgets(line, 64, stdin) == line)
    {
        if (line[0] == 'y' || line[0] == 'Y')
            return true;
        else if (line[0] == 'n' || line[0] == 'N')
            return false;
    }

    return default_yes;
}

bool con_ask(const char *msg, bool default_yes)
{
    printf("%s ", msg);
    return con_confirm(default_yes);
}

int con_get_numeric_opt(int min, int max)
{
    printf("[%d-%d]: ", min, max);

    char temp[256];
    if (fgets(temp, 256, stdin) != temp)
        return -1;

    char *end;
    unsigned long opt = strtoul(temp, &end, 10);

    if (opt == 0 && (errno == EINVAL || end == temp))
        return -2;

    while (*end == ' ' || *end == '\t' || *end == '\r' || *end == '\n')
        end++;

    if (*end != '\0')
        return -3;

    if (opt < (unsigned long)min || opt > (unsigned long)max)
        return -4;

    return (int)opt;
}

int imp__input_matrix_dimension(uint32_t *size)
{
    char temp[256];
    if (fgets(temp, 256, stdin) != temp)
        return -1;

    char *end;
    *size = strtol(temp, &end, 10);

    if (*size == 0 && (errno == EINVAL || end == temp))
        return -2;

    while (*end == ' ' || *end == '\t' || *end == '\r' || *end == '\n')
        end++;

    if (*end != '\0')
        return -3;
    
    if (*size <= 0)
        return -4;

    return 0;
}

int imp__input_matrix_element(mat_elem_t *value)
{
    char temp[256];
    if (fgets(temp, 256, stdin) != temp)
        return -1;

    char *end;
    *value = (int)strtol(temp, &end, 10);

    if (*value == 0 && (errno == EINVAL || end == temp))
        return -2;

    while (*end == ' ' || *end == '\t' || *end == '\r' || *end == '\n')
        end++;

    if (*end != '\0')
        return -3;

    return 0;
}

int con_input_matrix(sparse_matrix_t *matrix)
{
    uint32_t rows, cols;

    printf("Введите число строк матрицы: ");
    if (imp__input_matrix_dimension(&rows))
    {
        printf("Число строк матрицы должно быть положительным.\n");
        return -1;
    }

    printf("Введите число столбцов матрицы: ");
    if (imp__input_matrix_dimension(&cols))
    {
        printf("Число столбцов матрицы должно быть положительным.\n");
        return -2;
    }

    // init matrix here
    *matrix = sp_create(rows, cols);

    printf("Для окончания ввода элементов введите любую букву.\n");
    printf("Пример ввода пары чисел: 3 7\n");
    while (true)
    {
        uint32_t row, col;
        mat_elem_t value;

        printf("Введите пару чисел, строку и столбец для вставки элемента: ");
        int res = imp__get_indices_pair(rows, cols, &row, &col);
        if (res == -2)
            break;
        else if (res == -3)
        {
            printf("Введены некорректные числа. Повторите ввод.\n");
            continue;
        }

        printf("Введите сам элемент для вставки: ");
        if (imp__input_matrix_element(&value) != 0)
        {
            printf("Некорректный ввод. Повторите попытку.\n");
            continue;
        }

        if (value != 0)
            sp_set(matrix, row, col, value);
    }

    return 0;
}

int con_input_vector(mat_elem_t *vector, uint32_t size)
{
    printf("Приготовьтесь вводить %d элементов вектора!\n", size);

    for (uint32_t i = 0; i < size; i++)
    {
        printf("Введите %d-ый элемент: ", i + 1);
        if (imp__input_matrix_element(vector + i))
        {
            printf("Неверный ввод.\n");
            return -1;
        }
    }

    return 0;
}

#define TABLE_MAX_COLS 10

void con_print_table(uint32_t rows, uint32_t cols, const char* title, ...)
{
    va_list va;

    // define cols sizes
    uint32_t cols_sizes[TABLE_MAX_COLS] = { 0 };

    va_start(va, title);
    for (uint32_t row = 0; row < rows; row++)
    {
        for (uint32_t col = 0; col < cols; col++)
        {
            const char *str = va_arg(va, const char*);
            uint32_t size = strlen(str);
            if (size > cols_sizes[col])
                cols_sizes[col] = size;
        }
    }
    va_end(va);

    // get total size
    uint32_t total_size = cols * 3 + 1;
    for (uint32_t i = 0; cols_sizes[i] != 0; i++)
        total_size += cols_sizes[i];
    
    // print header and title
    printf("┌");
    for (uint32_t i = 0; i < total_size - 2; i++)
        printf("─");
    printf("┐\n");

    printf("│ %*s │\n", total_size - 4, title);

    printf("├");
    for (uint32_t i = 0; i < total_size - 2; i++)
        printf("─");
    printf("┤\n");

    va_start(va, title);
    for (uint32_t row = 0; row < rows; row++)
    {
        printf("│");
        for (uint32_t col = 0; col < cols; col++)
        {
            const char *str = va_arg(va, const char*);
            printf(" %*s │", cols_sizes[col], str);
        }
        printf("\n");
    }
    va_end(va);

    // print footer
    printf("└");
    for (uint32_t i = 0; i < total_size - 2; i++)
        printf("─");
    printf("┘\n");
}

void con_print_matrix(const sparse_matrix_t *matrix)
{
    printf("Матрица:\n\n");
    sp_print(matrix);
    printf("\n");
    sp_print_info(matrix);
}

void con_print_vector(const mat_elem_t *vector, uint32_t size)
{
    printf("Вектор:\n\n");
    for (uint32_t i = 0; i < size; i++)
        printf("  %d\n", vector[i]);
    printf("\n");
}
