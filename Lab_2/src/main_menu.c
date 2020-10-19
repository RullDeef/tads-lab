#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <time.h>
#include <stdbool.h>
#include <assert.h>
#include "uki.h"
#include "flat_table.h"
#include "conio.h"
#include "sorters.h"
#include "main_menu.h"

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
            printf("Дата не должна быть раньше 01.01.1880 или позже 31.12.2020\n");
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

static inline void imp__print_table_header()
{
    printf("┌──────┬────────────────────────────────┬─────────┬───────────────┬───────────┬───────────┬─────────┬──────────┬─────────────┬─────────────────┬────────────────┐\n"
           "│  ID  │ Адрес квартиры                 │ Площадь │ Кол-во комнат │ Стоимость │ Тип       │ Отделка │ Животные │ Построено   │ Кол-во собст-ов │ Кол-во жильцов │\n"
           "├──────┼────────────────────────────────┼─────────┼───────────────┼───────────┼───────────┼─────────┼──────────┼─────────────┼─────────────────┼────────────────┤\n");
}

static inline void imp__print_table_footer()
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

int output_flat_table(flat_table_t *table, keys_table_t *keys)
{
    imp__print_table_header();

    for (uint32_t i = 0; i < table->size; i++)
    {
        imp__colorize_output(i);
        if (keys != NULL)
            printf_flat(keys->keys[i].id, table->flats_array + keys->keys[i].id);
        else
            printf_flat(i, table->flats_array + i);
        imp__colorize_output(-1);
    }

    imp__print_table_footer();

    return 0;
}

int show_table(void *data)
{
    flat_table_t *table = (flat_table_t *)data;
    output_flat_table(table, NULL);
    return 0;
}

static void imp__print_init_table(flat_table_t *table)
{
    printf("  Исходная таблица (не по ключам):\n\n");
    output_flat_table(table, NULL);
}

int append_flat(void *data)
{
    flat_table_t *table = (flat_table_t *)data;

    // read flat data to new element and insert in to table
    flat_t flat = create_flat();

    if (!imp__request_flat_data(&flat))
    {
        if (!ft_append_flat(table, &flat))
            printf("Успешно добавлена новая запись в таблицу.\n");
        else
            printf("При вставке произошла ошибка.\n");
    }
    else
        printf("Не все данные были введены корректно.\n");

    return 0;
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

static void imp__better_sort(flat_table_t *table, keys_table_t keys)
{
    bool ascending;
    if (imp__request_sort_direction(&ascending))
    {
        printf("Не выбрано направление сортировки. Выбираем по-умолчанию: по убыванию.\n");
        ascending = false;
    }

    printf("  Выполняется сортировка...\n");
    sort_params_t sort_params = (sort_params_t) {
        .ascending = ascending,
        .real_sort = false
    };
    uint32_t delta_a_fast = ft_sort_a_fast(table, &keys, sort_params);
    uint32_t delta_b_fast = ft_sort_b_fast(table, &keys, sort_params);
    uint32_t delta_a_slow = ft_sort_a_slow(table, &keys, sort_params);
    uint32_t delta_b_slow = ft_sort_b_slow(table, &keys, sort_params);

    sort_params.real_sort = true;
    ft_sort_b_fast(table, &keys, sort_params);

    printf("  Сортировка завершена.\n"
           "  Результаты сортировки (по времени):\n\n");

    printf("┌──────────────────┬────────────┬──────────────────┐\n"
           "│  Тип сортировки  │ По ключам? │ Время сортировки │\n"
           "├──────────────────┼────────────┼──────────────────┤\n"
           "│  Сорт. обменом   │        нет │ %13u tc │\n"
           "│  Сорт. обменом   │         да │ %13u tc │\n"
           "│  Сорт. слиянием  │        нет │ %13u tc │\n"
           "│  Сорт. слиянием  │         да │ %13u tc │\n",
           delta_a_slow, delta_b_slow, delta_a_fast, delta_b_fast);
    printf("└──────────────────┴────────────┴──────────────────┘\n");

#ifdef _PERF_TEST
    fprintf(stderr, "%lu %lu %lu %lu\n", delta_a_slow, delta_b_slow, delta_a_fast, delta_b_fast);
#endif
}

int imp__request_search_params(float *price_1, float *price_2)
{
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

int search_flat(void *data)
{
    flat_table_t *table = (flat_table_t *)data;

    float price_1, price_2;
    int founded = 0;

    if (imp__request_search_params(&price_1, &price_2))
        return 0;

    for (uint32_t i = 0; i < table->size; i++)
    {
        if (ft_flat_satisfies(table->flats_array + i, price_1, price_2))
        {
            if (founded == 0)
            {
                printf("\n        Результаты поиска:\n\n");
                imp__print_table_header();
            }

            imp__colorize_output(founded);
            printf_flat(i, table->flats_array + i);
            imp__colorize_output(-1);
            founded++;
        }
    }

    if (founded != 0)
        imp__print_table_footer();
    else
        printf("\n  По заданным параметром не было найдено ни одной квартиры.\n");

    return 0;
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

void pause()
{
    char temp[2];
    fgets(temp, 2, stdin);
}

static void imp__print_keys_table(flat_table_t *table, keys_table_t keys)
{
    table = table;
    system("clear");
    printf("  Таблица ключей:\n\n"
           "┌──────┬───────────────┐\n"
           "│  ID  │ Кол-во комнат │\n"
           "├──────┼───────────────┤\n");

    for (uint32_t i = 0; i < keys.size; i++)
    {
        imp__colorize_output(i);
        printf("│ %04u │ %13d │\n", keys.keys[i].id, keys.keys[i].rooms_amount);
        imp__colorize_output(-1);
    }

    printf("└──────┴───────────────┘\n");
}

static void imp__output_both_tables(flat_table_t *table, keys_table_t keys)
{
    system("clear");
    printf("  Таблица ключей и исходная таблица:\n\n"
           "┌──────┬───────────────┐   ┌──────┬────────────────────────────────┬─────────┬───────────────┬───────────┬───────────┬─────────┬──────────┬─────────────┬─────────────────┬────────────────┐\n"
           "│  ID  │ Кол-во комнат │   │  ID  │ Адрес квартиры                 │ Площадь │ Кол-во комнат │ Стоимость │ Тип       │ Отделка │ Животные │ Построено   │ Кол-во собст-ов │ Кол-во жильцов │\n"
           "├──────┼───────────────┤   ├──────┼────────────────────────────────┼─────────┼───────────────┼───────────┼───────────┼─────────┼──────────┼─────────────┼─────────────────┼────────────────┤\n");

    for (uint32_t i = 0; i < table->size; i++)
    {
        imp__colorize_output(i);
        printf("│ %04u │ %13d │   ", keys.keys[i].id, keys.keys[i].rooms_amount);
        printf_flat(i, table->flats_array + i);
        imp__colorize_output(-1);
    }

    printf("└──────┴───────────────┘   └──────┴────────────────────────────────┴─────────┴───────────────┴───────────┴───────────┴─────────┴──────────┴─────────────┴─────────────────┴────────────────┘\n");
}

static keys_table_t imp__gen_table_keys(flat_table_t *table)
{
    keys_table_t keys;
    keys.size = table->size;
    keys.keys = malloc(keys.size * sizeof(flat_key_t));

    for (uint32_t i = 0; i < table->size; i++)
    {
        keys.keys[i].id = i;
        keys.keys[i].rooms_amount = table->flats_array[i].rooms_amount;
    }

    return keys;
}

int sort_table(void *data)
{
    flat_table_t *table = (flat_table_t *)data;

    int status = 0;
    bool need_exit = false;

    keys_table_t keys = imp__gen_table_keys(table);

    while (!need_exit)
    {
        int opt = imp__get_sort_menu_opt();

        switch (opt)
        {
            default:
            case -1:
                printf("Вы неправильно ввели номер опции. Повторите попытку.\n");
                pause();
                break;
            
            case 0:
                printf("Выход из меню сортировки...");
                need_exit = true;
                break;
            
            case 1:
                imp__print_keys_table(table, keys);
                pause();
                break;
            
            case 2:
                imp__print_init_table(table);
                pause();
                break;
            
            case 3:
                printf("  Таблица по ключам:\n\n");
                output_flat_table(table, &keys);
                pause();
                break;
            
            case 4:
                imp__output_both_tables(table, keys);
                pause();
                break;
            
            case 5:
                imp__better_sort(table, keys);
                pause();
                break;
        }
    }

    flat_table_t temp = ft_clone(table);

    for (uint32_t i = 0; i < table->size; i++)
        temp.flats_array[i] = table->flats_array[keys.keys[i].id];

    ft_free(table);
    *table = temp;

    free(keys.keys);
    return status;
}

int delete_flat(void *data)
{
    flat_table_t *table = (flat_table_t *)data;

    uint32_t id;
    if (uki_input_uint32_minmax("Введите ID удаляемой записи: ",
        "Неверный ID записи.\n", 0, table->size, &id))
    {
        flat_t flat;
        if (!ft_delete_flat(table, id, &flat))
        {
            printf("Удалена следующая запись:\n");
            imp__print_table_header();
            printf("\033[1;31m");
            printf_flat(id, &flat);
            imp__colorize_output(-1);
            imp__print_table_footer();
        }
        else
            printf("Запись с полем ID = %u не была найдена.\n", id);
    }

    return 0;
}
