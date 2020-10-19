#include <stdio.h>
// #include <string.h>
#include <stdlib.h>
// #include <errno.h>
#include <time.h>
#include <stdbool.h>
// #include <assert.h>
#include "uki.h"
#include "conio.h"
#include "flat_table.h"
#include "main_menu.h"


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

int show_table(void *data)
{
    flat_table_t *table = (flat_table_t *)data;

    imp__print_table_header();
    for (uint32_t i = 0; i < table->size; i++)
    {
        cio_colorize_set(i);
        printf_flat(i, table->flats_array + i);
        cio_colorize_flush();
    }
    imp__print_table_footer();

    return 0;
}

int append_flat(void *data)
{
    flat_table_t *table = (flat_table_t *)data;

    // read flat data to new element and insert in to table
    flat_t flat = create_flat();

    if (!cio_request_flat_data(&flat))
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

            cio_colorize_set(founded);
            printf_flat(i, table->flats_array + i);
            cio_colorize_flush();
            founded++;
        }
    }

    if (founded != 0)
        imp__print_table_footer();
    else
        printf("\n  По заданным параметром не было найдено ни одной квартиры.\n");

    return 0;
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
            cio_colorize_flush();
            imp__print_table_footer();
        }
        else
            printf("Запись с полем ID = %u не была найдена.\n", id);
    }

    return 0;
}
