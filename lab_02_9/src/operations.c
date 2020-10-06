#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <time.h>
#include <stdbool.h>
#include <assert.h>
#include "flat_table.h"
#include "conio.h"
#include "operations.h"

#define MIN_POSSIBLE_YEAR 1880
#define MAX_POSSIBLE_YEAR 2020

static int imp__input_option()
{
    char temp[256];
    if (fgets(temp, 256, stdin) != temp)
        return -1;

    while (strlen(temp) > 0 && (temp[strlen(temp) - 1] == '\n' || temp[strlen(temp) - 1] == '\r'))
        temp[strlen(temp) - 1] = '\0';

    if (strcmp(temp, "0") == 0)
        return 0;
    else if (strcmp(temp, "1") == 0)
        return 1;
    else
        return -2;
}

static int imp__request_flat_data(flat_t *flat)
{
    assert(flat != NULL);

    int status_code = 0;

    printf("                                               ┌────────────────────────────┐\n"
           "Введите адрес квартиры (не более 30 символов): ");
    status_code = cio_read_line(flat->address, MAX_ADDRESS_SIZE);
    if (status_code == 0)
    {
        printf("Введите площадь квартиры (целое или вещественное положительное число): ");
        status_code = cio_read_float(&flat->area);
    }

    if (status_code == 0)
    {
        printf("Введите количество комнат (натуральное число): ");
        status_code = cio_read_uchar(&flat->rooms_amount);
    }

    if (status_code == 0)
    {
        printf("Введите стоимость за квадратный метр (целое или вещественное положительное число): ");
        status_code = cio_read_float(&flat->price_per_m2);
    }

    if (status_code == 0)
    {
        printf("Введите тип жилья (0 - первичное, 1 - вторичное): ");
        int opt = imp__input_option();
        if (opt < 0)
            status_code = -6;
        else if (opt == 0)
            flat->type = PRIMARY;
        else
            flat->type = SECONDARY;
    }

    if (status_code == 0 && flat->type == PRIMARY)
    {
        printf("Квартира с отделкой? (0 - без отделки, 1 - с отделкой): ");
        int opt = imp__input_option();
        if (opt < 0)
            status_code = -6;
        else
            flat->type_data.has_trim = opt;
    }

    if (status_code == 0 && flat->type == SECONDARY)
    {
        printf("Были ли животные? (0 - нет, 1 - да): ");
        int opt = imp__input_option();
        if (opt < 0)
            status_code = -7;
        else
            flat->type_data.was_pets = opt;
    }

    if (status_code == 0 && flat->type == SECONDARY)
    {
        printf("Введите дату постройки в формате ДД.ММ.ГГГГ: ");
        status_code = cio_read_time(&flat->build_time);
        if (status_code != 0)
        {
            printf("Вы ввели неверную дату. Переповерьте ввод.\n");
            // printf("Дата не должна быть раньше 01.01.%d или позже 31.12.%d\n", MIN_POSSIBLE_YEAR, MAX_POSSIBLE_YEAR);
        }
    }

    if (status_code == 0 && flat->type == SECONDARY)
    {
        printf("Введите количество предыдущих владельцев (от 1 до 100): ");
        status_code = cio_read_uchar(&flat->prev_owners_amount);
        if (status_code == -4 || flat->prev_owners_amount > 100)
        {
            printf("Количество предыдущих владельцев не соответствует требованиям.\n");
            status_code = -4;
        }
    }

    if (status_code == 0 && flat->type == SECONDARY)
    {
        printf("Введите количество предыдущих жильцов (от 1 до 100): ");
        status_code = cio_read_uchar(&flat->prev_lodgers_amount);
        if (status_code == -4 || flat->prev_lodgers_amount > 100)
        {
            printf("Количество предыдущих жильцов не соответствует требованиям.\n");
            status_code = -4;
        }
    }

    return status_code;
}

app_state_t create_app_state()
{
    return (app_state_t)
    {
        .input_filename = "",
        .table = create_flat_table()
    };
}

int request_input_filename(app_state_t *state)
{
    assert(state != NULL);

    printf("Введите имя входного файла: ");
    return cio_read_line(state->input_filename, MAX_FILE_NAME_LENGTH);
}

int read_table_from_file(app_state_t *state)
{
    assert(state != NULL);
    assert(strlen(state->input_filename) > 0);

    FILE *file = fopen(state->input_filename, "rt");
    if (file == NULL)
    {
        printf("Нет такого файла.\n");
        return -1;
    }

    int status_code = fread_flat_table(file, &state->table);
    if (status_code != 0)
        printf("Ошибки при чтении файла.\n");

    fclose(file);
    return status_code;
}

/*
int write_table_to_file(app_state_t *state)
{
    assert(state != NULL);
    assert(strlen(state->output_filename) > 0);

    FILE *file = fopen(state->output_filename, "wt");
    if (file == NULL)
        return -1;
    
    int status_code = fwrite_flat_table(file, &state->table);

    fclose(file);
    return status_code;
}
*/

static inline void imp__print_table_header()
{
    printf("┌──────┬────────────────────────────────┬─────────┬───────────────┬───────────┬───────────┬─────────┬──────────┬─────────────┬─────────────────┬────────────────┐\n"
           "│  ID  │ Адрес квартиры                 │ Площадь │ Кол-во комнат │ Стоимость │ Тип       │ Отделка │ Животные │ Построено   │ Кол-во собст-ов │ Кол-во жильцов │\n"
           "├──────┼────────────────────────────────┼─────────┼───────────────┼───────────┼───────────┼─────────┼──────────┼─────────────┼─────────────────┼────────────────┤\n");
}

static void imp__print_table_footer()
{
    printf("└──────┴────────────────────────────────┴─────────┴───────────────┴───────────┴───────────┴─────────┴──────────┴─────────────┴─────────────────┴────────────────┘\n");
}

static void imp__colorize_output(int i)
{
    if (i == -1)
        printf("\033[0m");
    else if (i % 6 == 1)
        printf("\033[0;32m");
    else if (i % 6 == 2)
        printf("\033[0;33m");
    else if (i % 6 == 3)
        printf("\033[0;32m");
    else if (i % 6 == 5)
        printf("\033[0;36m");
}

int output_flat_table(app_state_t *state)
{
    assert(state != NULL);
    assert_flat_table(&state->table);

    imp__print_table_header();

    for (int i = 0; i < state->table.size; i++)
    {
        imp__colorize_output(i);
        printf_flat(*(state->table.flat_ptrs + i));
        imp__colorize_output(-1);
    }

    imp__print_table_footer();

    return 0;
}

int append_flat_to_table(app_state_t *state)
{
    assert(state != NULL);
    assert_flat_table(&state->table);

    // read flat data to new element and insert in to table
    flat_t flat = create_flat();

    int status_code = imp__request_flat_data(&flat);
    if (status_code == 0)
        status_code = append_flat_table(&state->table, &flat);
    else
        printf("Не все данные были введены корректно.\n");

    return status_code;
}

static inline void imp__print_sort_opts(void)
{
    printf("Выберите тип сортировки:\n\n"
           "  1. Вставками (без доп. массива)\n"
           "  2. Вставками (с доп. массивом)\n"
           "  3. Слиянием (без доп. массива)\n"
           "  4. Слиянием (с доп. массивом)\n"
           "\n[Ваш выбор:] >>> ");
}

static int imp__request_sort_type(sort_fn_t *func)
{
    imp__print_sort_opts();

    unsigned char temp;
    int status_code = cio_read_uchar(&temp);
    if (status_code == 0)
    {
        if (temp < 1 || temp > 4)
            status_code = -1;
        else
        {
            *func = (sort_fn_t[]) {
                sort_flat_table_a_slow,
                sort_flat_table_b_slow,
                sort_flat_table_a_fast,
                sort_flat_table_b_fast
            }[temp - 1];
        }
    }

    return status_code;
}

static int imp__request_sort_key(sort_key_t *key)
{
    unsigned char temp;

    printf("Введите номер поля, по которому необходимо отсортировать таблицу:\n\n"
           "  1. Адрес\n"
           "  2. Общая площадь комнат\n"
           "  3. Количество комнат\n"
           "\n[Ваш выбор:] >>> ");

    int status_code = cio_read_uchar(&temp);
    if (status_code == 0 && (temp < 1 || temp > 3))
        status_code = -1;

    if (status_code == 0)
        *key = temp - 1;

    return status_code;
}

static int imp__request_sort_direction(bool *ascending)
{
    printf("Выберите вариант сортировки:\n\n"
           "  0. Сортировка по убыванию\n"
           "  1. Сортировка по возрастанию\n"
           "\n[Ваш выбор:] >>> ");

    int opt = imp__input_option();
    if (opt >= 0)
    {
        *ascending = opt == 1;
        opt = 0;
    }

    return opt;
}

int sort_table(app_state_t *state)
{
    assert(state != NULL);
    assert_flat_table(&state->table);

    sort_fn_t sort_func;

    sort_key_t key = ADDRESS;
    bool ascending = true;
    int status_code = 0;

    status_code = imp__request_sort_type(&sort_func);
    if (status_code == 0)
    {
        status_code = imp__request_sort_key(&key);
        if (status_code == 0)
        {
            status_code = imp__request_sort_direction(&ascending);
            if (status_code == 0)
            {
                printf("sorting...\n");
                sort_func(&state->table, key, ascending);
            }
            else
                printf("Вы ввели неверное значение для направления сортировки.\n");
        }
        else
            printf("Вы ввели неверное имя ключа для сортировки.\n");
    }
    else
        printf("Вы не выбрали тип сортировки.\n");

    return status_code;
}

int imp__request_search_params(float *price_1, float *price_2)
{
    assert(price_1 != NULL);
    assert(price_2 != NULL);

    int status_code = 0;

    printf("Поиск вторичного 2-х комнатного жилья в указанном ценовом диапазоне без животных.\n\n"
           "  Введите минимальную стоимость квартиры: ");

    status_code = cio_read_float(price_1);
    if (status_code == 0)
    {
        printf("  Введите максимальную стоимость квартиры: ");
        status_code = cio_read_float(price_2);

        if (status_code != 0)
            printf("Неправильный ввод. Ожидается вещественное число.\n");
        else if (*price_1 > *price_2)
            printf("Максимальная стоимость не должна быть меньше минимальной.\n");
    }
    else
        printf("Неправильный ввод. Ожидается вещественное число.\n");

    return status_code;
}

int search_flat(app_state_t *state)
{
    assert(state != NULL);
    assert_flat_table(&state->table);

    int status_code = 0;

    float price_1, price_2;
    int founded = 0;

    status_code = imp__request_search_params(&price_1, &price_2);
    if (status_code != 0)
        return status_code;

    for (int i = 0; i < state->table.size; i++)
    {
        if (flat_satisfies(state->table.flat_ptrs[i], price_1, price_2))
        {
            if (founded == 0)
            {
                printf("\n        Результаты поиска:\n\n");
                imp__print_table_header();
            }

            imp__colorize_output(i);
            printf_flat(state->table.flat_ptrs[i]);
            imp__colorize_output(-1);
            founded++;
        }
    }

    if (founded != 0)
        imp__print_table_footer();
    else
        printf("\n  По заданным параметром не было найдено ни одной квартиры.\n");

    return status_code;
}

int delete_flat(app_state_t *state)
{
    assert(state != NULL);
    assert_flat_table(&state->table);

    size_t id;

    printf("Введите ID удаляемой записи: ");
    int status_code = cio_read_ulong(&id);

    if (status_code == 0)
    {
        if (id >= state->table.size)
        {
            printf("Неверный ID записи.\n");
            status_code = -1;
        }
        else
        {
            flat_t flat;
            status_code = delete_flat_table(&state->table, id, &flat);

            if (status_code == 0)
            {
                printf("Удалена следующая запись:\n");
                imp__print_table_header();
                printf("\033[1;31m");
                printf_flat(&flat);
                imp__colorize_output(-1);
                imp__print_table_footer();
            }
            else
            {
                printf("Запись с полем ID = %ld не была найдена.\n", id);
            }
        }
    }
    else
        printf("Неверный ввод.\n");

    return status_code;
}

void free_app_state(app_state_t *state)
{
    if (state == NULL)
        return;

    free_flat_table(&state->table);
}
