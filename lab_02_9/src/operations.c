#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <stdbool.h>
#include <assert.h>
#include "flat_table.h"
#include "operations.h"

static int imp__read_line(char *str, unsigned int max_length)
{
    if (fgets(str, max_length, stdin) != str)
        return -1;

    while (strlen(str) > 0 && (str[strlen(str) - 1] == '\n' || str[strlen(str) - 1] == '\r'))
        str[strlen(str) - 1] = '\0';

    if (strlen(str) == 0)
        return -2;

    return 0;
}

static int imp__read_integer(unsigned char *num)
{
    char temp[256];
    if (fgets(temp, 256, stdin) != temp)
        return -1;

    char *end;
    unsigned long val = strtoul(temp, &end, 10);

    if (val == 0 && (errno == EINVAL || end == temp))
        return -2;

    while (*end == ' ' || *end == '\t' || *end == '\r' || *end == '\n')
        end++;

    if (*end != '\0')
        return -3;

    if (val > 255UL)
        return -4;

    *num = (unsigned char)val;
    return 0;
}

static int imp__read_float(float *num)
{
    char temp[256];
    if (fgets(temp, 256, stdin) != temp)
        return -1;

    char *end;
    *num = strtof(temp, &end);

    if (*num == 0.0f && errno == EINVAL)
        return -2;

    while (*end == ' ' || *end == '\t' || *end == '\r' || *end == '\n')
        end++;

    if (*end != '\0')
        return -3;

    return 0;
}

static int imp__read_time(long *time)
{
    char temp[256];
    if (fgets(temp, 256, stdin) != temp)
        return -1;

    char *end;
    *time = strtol(temp, &end, 10);

    if (*time == 0 && errno == EINVAL)
        return -2;

    while (*end == ' ' || *end == '\t' || *end == '\r' || *end == '\n')
        end++;

    if (*end != '\0')
        return -3;

    return 0;
}

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

    printf("Введите адрес квартиры (желательно не более 200 символов):\n");
    status_code = imp__read_line(flat->address, MAX_ADDRESS_SIZE);
    if (status_code == 0)
    {
        printf("Введите площадь квартиры (целое или вещественное положительное число): ");
        status_code = imp__read_float(&flat->area);
    }

    if (status_code == 0)
    {
        printf("Введите количество комнат (натуральное число): ");
        status_code = imp__read_integer(&flat->rooms_amount);
    }

    if (status_code == 0)
    {
        printf("Введите стоимость за квадратный метр (целое или вещественное положительное число): ");
        status_code = imp__read_float(&flat->price_per_m2);
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
        printf("Введите время постройки в миллисекундах: ");
        status_code = imp__read_time(&flat->build_time);
    }

    if (status_code == 0 && flat->type == SECONDARY)
    {
        printf("Введите количество предыдущих владельцев: ");
        status_code = imp__read_integer(&flat->prev_owners_amount);
    }

    if (status_code == 0 && flat->type == SECONDARY)
    {
        printf("Введите количество предыдущих жильцов: ");
        status_code = imp__read_integer(&flat->prev_lodgers_amount);
    }

    return status_code;
}

app_state_t create_app_state()
{
    return (app_state_t){
        .input_filename = "",
        .output_filename = "",
        .table = create_flat_table()};
}

int request_input_filename(app_state_t *state)
{
    assert(state != NULL);

    printf("Введите имя входного файла: ");
    return imp__read_line(state->input_filename, MAX_FILE_NAME_LENGTH);
}

int request_output_filename(app_state_t *state)
{
    assert(state != NULL);

    printf("Введите имя выходного файла: ");
    return imp__read_line(state->output_filename, MAX_FILE_NAME_LENGTH);
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

static void imp__print_table_title()
{
    printf("| ID | Адрес квартиры                 | Площадь | Кол-во комнат | Стоимость  | Тип       | Отделка | Животные | Построено   | Кол-во собст-ов | Кол-во жильцов |\n");
    for (int i = 0; i < 160; i++)
        printf("-");
    printf("\n");
}

int output_flat_table(app_state_t *state)
{
    assert(state != NULL);
    assert_flat_table(&state->table);

    imp__print_table_title();

    for (int i = 0; i < state->table.size; i++)
        printf_flat(state->table.flats_array + i);

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

    if (status_code != 0)
        printf("Что-то вы ввели не по инструкции :с\n");

    return status_code;
}

static int imp__request_sort_type(sort_fn_t *func)
{
    unsigned char temp;

    printf("Выберите тип сортировки:\n\n"
           "  1. Вставками (без доп. массива)\n"
           "  2. Вставками (с доп. массивом)\n"
           "  3. Слиянием (без доп. массива)\n"
           "  4. Слиянием (с доп. массивом)\n"
           "\n[Ваш выбор:] >>> ");

    int status_code = imp__read_integer(&temp);
    if (status_code == 0)
    {
        switch (temp)
        {
        case 1:
            *func = sort_flat_table_a_slow;
            break;
        case 2:
            *func = sort_flat_table_b_slow;
            break;
        case 3:
            *func = sort_flat_table_a_fast;
            break;
        case 4:
            *func = sort_flat_table_b_fast;
            break;
        default:
            status_code = -1;
            break;
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

    int status_code = imp__read_integer(&temp);
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

    printf("Введите минимальную стоимость квадратного метра: ");

    status_code = imp__read_float(price_1);
    if (status_code == 0)
    {
        printf("Введите максимальную стоимость квадратного метра: ");
        status_code = imp__read_float(price_2);

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

    status_code = imp__request_search_params(&price_1, &price_2);
    if (status_code != 0)
        return status_code;

    int founded = search_flat_table(&state->table, price_1, price_2, printf_flat);

    if (founded == 0)
        printf("По заданным параметром не было найдено ни одной квартиры.\n");

    return status_code;
}

int delete_flat(app_state_t *state)
{
    assert(state != NULL);
    assert_flat_table(&state->table);

    unsigned char id;

    printf("Введите ID удаляемой записи: ");
    int status_code = imp__read_integer(&id);

    if (status_code == 0)
    {
        if (id < 0 || id >= state->table.size)
        {
            printf("Неверный ID записи.\n");
            status_code = -1;
        }
        else
        {
            flat_t flat = delete_flat_table(&state->table, id);

            printf("Удалена следующая запись:\n");
            imp__print_table_title();
            printf_flat(&flat);
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
