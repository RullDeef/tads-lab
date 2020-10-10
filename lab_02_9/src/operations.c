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

int request_input_filename(char *fname)
{
    printf("Введите имя входного файла: ");
    return cio_read_line(fname, MAX_FILE_NAME_LENGTH);
}

int read_table_from_file(char *fname, flat_table_t *table)
{
    FILE *file = fopen(fname, "rt");
    if (file == NULL)
    {
        printf("Нет такого файла.\n");
        return -1;
    }

    int status_code = ft_read(file, table);
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
    else if (i % 5 == 1)
        printf("\033[1;32m");
    else if (i % 5 == 2)
        printf("\033[1;33m");
    else if (i % 5 == 3)
        printf("\033[1;35m");
    else if (i % 5 == 4)
        printf("\033[1;36m");
}

int output_flat_table(flat_table_t *table, flat_t **ptrs)
{
    imp__print_table_header();

    for (int i = 0; i < table->size; i++)
    {
        imp__colorize_output(i);
        if (ptrs != NULL)
            printf_flat(ptrs[i]);
        else
            printf_flat(table->flats_array + i);
        imp__colorize_output(-1);
    }

    imp__print_table_footer();

    return 0;
}

static void imp__print_init_table(flat_table_t *table)
{
    printf("  Исходная таблица (не по ключам):\n\n");

    imp__print_table_header();

    for (int i = 0; i < table->size; i++)
    {
        imp__colorize_output(i);
        printf_flat(table->flats_array + i);
        imp__colorize_output(-1);
    }

    imp__print_table_footer();
}

int append_flat_to_table(flat_table_t *table)
{
    // read flat data to new element and insert in to table
    flat_t flat = create_flat();

    int status_code = imp__request_flat_data(&flat);
    if (status_code == 0)
        status_code = ft_append_flat(table, &flat);
    else
        printf("Не все данные были введены корректно.\n");

    if (status_code == 0)
        printf("Успешно добавлена новая запись в таблицу.\n");
    else
        printf("При обработке ввода произошла ошибка.\n");

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

/*
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
                ft_sort_a_slow,
                ft_sort_b_slow,
                ft_sort_a_fast,
                ft_sort_b_fast
            }[temp - 1];
        }
    }

    return status_code;
}
*/

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

static void imp__better_sort(flat_table_t *table, flat_t **keys)
{
    sort_key_t key = 0;
    if (imp__request_sort_key(&key))
    {
        printf("Не выбрано поля сортировки. Выбираем по-умолчанию: 1\n");
        key = 0;
    }

    bool ascending;
    if (imp__request_sort_direction(&ascending))
    {
        printf("Не выбрано направление сортировки. Выбираем по-умолчанию: 0\n");
        ascending = false;
    }

    printf("  Выполняется сортировка...\n");
    sort_params_t sort_params = (sort_params_t) {
        .key = key,
        .ascending = ascending,
        .real_sort = false
    };
    double delta_a_fast = (double)ft_sort_a_fast(table, keys, sort_params) / CLK_TCK;
    double delta_b_fast = (double)ft_sort_b_fast(table, keys, sort_params) / CLK_TCK;
    double delta_a_slow = (double)ft_sort_a_slow(table, keys, sort_params) / CLK_TCK;
    double delta_b_slow = (double)ft_sort_b_slow(table, keys, sort_params) / CLK_TCK;

    sort_params.real_sort = true;
    ft_sort_b_fast(table, keys, sort_params);

    printf("  Сортировка завершена.\n"
           "  Результаты сортировки (по времени):\n\n");

    printf("┌──────────────────┬────────────┬──────────────────┐\n"
           "│  Тип сортировки  │ По ключам? │ Время сортировки │\n"
           "├──────────────────┼────────────┼──────────────────┤\n"
           "│  Сорт. обменом   │        нет │ %14.3lf с │\n"
           "│  Сорт. обменом   │         да │ %14.3lf с │\n"
           "│  Сорт. слиянием  │        нет │ %14.3lf с │\n"
           "│  Сорт. слиянием  │         да │ %14.3lf с │\n",
           delta_a_slow, delta_a_fast, delta_b_slow, delta_b_fast);
    printf("└──────────────────┴────────────┴──────────────────┘\n");
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

int search_flat(flat_table_t *table)
{
    int status_code = 0;

    float price_1, price_2;
    int founded = 0;

    status_code = imp__request_search_params(&price_1, &price_2);
    if (status_code != 0)
        return status_code;

    for (int i = 0; i < table->size; i++)
    {
        if (ft_flat_satisfies(table->flats_array + i, price_1, price_2))
        {
            if (founded == 0)
            {
                printf("\n        Результаты поиска:\n\n");
                imp__print_table_header();
            }

            imp__colorize_output(founded);
            printf_flat(table->flats_array + i);
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

static int imp__get_sort_menu_opt()
{
    system("clear");
    printf("  [__Меню сортировки:__]\n\n"
           " 1. Показать таблицу ключей\n"
           " 2. Показать исходную таблицу\n"
           " 3. Показать таблицу по ключам\n"
           " 4. Показывать обе таблицы (исходная не по ключам)\n"
           " 5. Сортировать\n"
           " 0. Выход\n\n"
           "[Ваш выбор]>>> ");
    char opt[256];
    fgets(opt, 256, stdin);
    while (strlen(opt) > 0 && (opt[strlen(opt) - 1] == '\r' || opt[strlen(opt) - 1] == '\n'))
        opt[strlen(opt) - 1] = '\0';
    system("clear");
    if (strlen(opt) == 1)
    {
        if (opt[0] >= '0' && opt[0] <= '5')
            return opt[0] - '0';
    }
    return -1;
}

static void imp__pause()
{
    char temp[2];
    fgets(temp, 2, stdin);
}

static void imp__print_keys_table(flat_table_t *table, flat_t **keys)
{
    system("clear");
    printf("  Индекс - позиция ключа в таблице ключей.\n"
           "  ID - уникальный идентификатор квартиры.\n\n"
           "┌──────────┬──────┐\n"
           "│  Индекс  │  ID  │\n"
           "├──────────┼──────┤\n");

    for (size_t i = 0; i < table->size; i++)
    {
        imp__colorize_output(i);
        printf("│ %8lu │ %04u │\n", i, keys[i]->id);
        imp__colorize_output(-1);
    }

    printf("└──────────┴──────┘\n");
}

static void imp__output_both_tables(flat_table_t *table, flat_t **keys)
{
    system("clear");
    printf("  Индекс - позиция ключа в таблице ключей.\n"
           "  ID - уникальный идентификатор квартиры.\n\n"
           "┌──────────┬──────┐   ┌──────┬────────────────────────────────┬─────────┬───────────────┬───────────┬───────────┬─────────┬──────────┬─────────────┬─────────────────┬────────────────┐\n"
           "│  Индекс  │  ID  │   │  ID  │ Адрес квартиры                 │ Площадь │ Кол-во комнат │ Стоимость │ Тип       │ Отделка │ Животные │ Построено   │ Кол-во собст-ов │ Кол-во жильцов │\n"
           "├──────────┼──────┤   ├──────┼────────────────────────────────┼─────────┼───────────────┼───────────┼───────────┼─────────┼──────────┼─────────────┼─────────────────┼────────────────┤\n");

    for (size_t i = 0; i < table->size; i++)
    {
        imp__colorize_output(i);
        printf("│ %8lu │ %04u │   ", i, keys[i]->id);
        printf_flat(table->flats_array + i);
        imp__colorize_output(-1);
    }

    printf("└──────────┴──────┘   └──────┴────────────────────────────────┴─────────┴───────────────┴───────────┴───────────┴─────────┴──────────┴─────────────┴─────────────────┴────────────────┘\n");
}

static flat_t **imp__gen_table_keys(flat_table_t *table)
{
    flat_t **keys = malloc(table->size * sizeof(flat_t*));

    for (size_t i = 0; i < table->size; i++)
        keys[i] = table->flats_array + i;

    return keys;
}

int sort_table(flat_table_t *table)
{
    int status = 0;
    bool need_exit = false;

    flat_t **keys = imp__gen_table_keys(table);

    while (!need_exit)
    {
        int opt = imp__get_sort_menu_opt();

        switch (opt)
        {
            default:
            case -1:
                printf("Вы неправильно ввели номер опции. Повторите попытку.\n");
                imp__pause();
                break;
            
            case 0:
                printf("Выход из меню сортировки...");
                need_exit = true;
                break;
            
            case 1:
                imp__print_keys_table(table, keys);
                imp__pause();
                break;
            
            case 2:
                imp__print_init_table(table);
                imp__pause();
                break;
            
            case 3:
                printf("  Таблица по ключам:\n\n");
                output_flat_table(table, keys);
                imp__pause();
                break;
            
            case 4:
                imp__output_both_tables(table, keys);
                imp__pause();
                break;
            
            case 5:
                imp__better_sort(table, keys);
                imp__pause();
                break;
        }
    }

    flat_table_t temp = ft_clone(table);

    for (size_t i = 0; i < table->size; i++)
        temp.flats_array[i] = *(keys[i]);

    ft_free(table);
    *table = temp;

    return status;
}

int delete_flat(flat_table_t *table)
{
    size_t id;

    printf("Введите ID удаляемой записи: ");
    int status_code = cio_read_ulong(&id);

    if (status_code == 0)
    {
        if (id >= table->size)
        {
            printf("Неверный ID записи.\n");
            status_code = -1;
        }
        else
        {
            flat_t flat;
            status_code = ft_delete_flat(table, id, &flat);

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
                printf("Запись с полем ID = %ld не была найдена.\n", id);
        }
    }
    else
        printf("Неверный ввод.\n");

    return status_code;
}
