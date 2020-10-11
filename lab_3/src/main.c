/*

    Добавить интерфейс пользователя.
    ImGUI.

    Добавить возможность вводить элементы руками.
    Добавить возможность случайного заполенения по данному проценту разреженности.

    Сделать замеры зависимости времени обработки разреженной матрицы
    от процента заполнения и сравнить с обработкой обычной плотной матрицы.
ы
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "../include/sparse_matrix.h"
#include "../include/conio.h"

void print_menu(void);
void mult_mat_vect(void);
int menu_loop(void);

int main(void)
{
    return menu_loop();

    sparse_matrix_t mat = sp_create(5, 5);
    sp_print(&mat);

    sp_print_info(&mat);

    printf("\nelement at row=3 col=0 : %d\n", sp_get(&mat, 3, 0));
    printf("set it to 8!\n\n");

    sp_set(&mat, 1, 0, 10);
    sp_set(&mat, 2, 2, 22);
    sp_set(&mat, 3, 0, 30);
    sp_print(&mat);

    sp_print_info(&mat);

    return 0;
}

void print_menu(void)
{
    con_print_menu_header();

    con_print_opt(1, "Умножить матрицу на вектор");
    con_print_opt(2, "Умножить матрицу на другую матрицу");
    con_print_opt(0, "Выйти");

    con_print_menu_footer();
}

int menu_loop(void)
{
    bool need_exit = false;

    while (!need_exit)
    {
        con_clear();
        print_menu();

        int opt = con_get_numeric_opt(0, 2);
        switch (opt)
        {
        default:
        case -1:
            printf("Вы ввели неверную опцию, пожалуйста, повторите попытку.\n");
            con_wait();
            break;
        case 0:
            if (con_ask("Вы уверены?", false))
                need_exit = true;
            printf("Закрываем программу...\n");
            need_exit = true;
            break;
        case 1:
            mult_mat_vect();
            break;
        }
    }

    return 0;
}

void mult_mat_vect(void)
{
    sparse_matrix_t matrix;
    mat_elem_t *vector = NULL;
    mat_elem_t *result = NULL;

    if (con_input_matrix(&matrix) != EXIT_SUCCESS)
    {
        printf("Произошла ошибка при вводе данных. Повторите попытку.\n");
        return;
    }

    // allocate memory for vector
    vector = malloc(matrix.cols_size * sizeof(mat_elem_t));
    if (vector == NULL)
    {
        printf("Ошибка при выделении памяти!\n");
        sp_free(&matrix);
        return;
    }

    if (con_input_vector(vector, matrix.cols_size) != EXIT_SUCCESS)
    {
        printf("Произошла ошибка при вводе данных. Повторите попытку.\n");
        sp_free(&matrix);
        return;
    }

    result = malloc(matrix.rows_size * sizeof(mat_elem_t));
    if (result == NULL)
    {
        printf("Произошла ошибка при выделении памяти. Выход.\n");
        sp_free(&matrix);
        free(vector);
        return;
    }

    if (sp_mult_by_vector(&matrix, vector, result) != EXIT_SUCCESS)
    {
        printf("Не получилось умножить. Что-то не так.\n");
        sp_free(&matrix);
        free(vector);
        free(result);
        return;
    }

    // print vector here and return
    printf("Результат умножения матрицы на вектор:\n");
    con_print_matrix(&matrix);
    con_print_vector(result, matrix.rows_size);
    con_wait();

    sp_free(&matrix);
    free(vector);
    free(result);
}
