#include <stdio.h>
#include "uki.h"
#include "conio.h"
#include "table_sorts.h"
#include "sort_menu.h"

typedef struct
{
    flat_table_t *table;
    keys_table_t keys;
} sort_data_t;

static int show_table(void *data);
static int show_keys(void *data);
static int show_table_by_keys(void *data);
static int show_table_and_keys(void *data);
static int apply_sort(void *data);

int sort_menu(void *data)
{
    sort_data_t sort_data;
    sort_data.table = (flat_table_t *)data;
    sort_data.keys = ts_keys_create(sort_data.table);

    uki_menu_t menu = uki_menu_create("Меню сортировки", 6,
        "Показать исходную таблицу", show_table,
        "Показать таблицу ключей", show_keys,
        "Показать таблицу по ключам", show_table_by_keys,
        "Показать обе таблицы", show_table_and_keys,
        "Сортировать", apply_sort,
        "Выйти из меню", uki_menu_opt_exit
    );

    int result = uki_menu_run(&menu, &sort_data);
    if (result == UKI_OK)
    {
        // update original table using keys
        flat_table_t temp = ft_clone(sort_data.table);

        for (uint32_t i = 0; i < sort_data.table->size; i++)
            temp.flats_array[i] = sort_data.table->flats_array[sort_data.keys.keys[i].id];

        ft_free(sort_data.table);
        *(sort_data.table) = temp;
    }

    ts_keys_free(&sort_data.keys);
    return result;
}

static int show_table(void *data)
{
    flat_table_t *table = ((sort_data_t *)data)->table;

    printf("  Исходная таблица (не по ключам):\n\n"
           "┌──────┬────────────────────────────────┬─────────┬───────────────┬───────────┬───────────┬─────────┬──────────┬─────────────┬─────────────────┬────────────────┐\n"
           "│  ID  │ Адрес квартиры                 │ Площадь │ Кол-во комнат │ Стоимость │ Тип       │ Отделка │ Животные │ Построено   │ Кол-во собст-ов │ Кол-во жильцов │\n"
           "├──────┼────────────────────────────────┼─────────┼───────────────┼───────────┼───────────┼─────────┼──────────┼─────────────┼─────────────────┼────────────────┤\n");

    for (uint32_t i = 0; i < table->size; i++)
    {
        cio_colorize_set(i);
        printf_flat(i, table->flats_array + i);
        cio_colorize_flush();
    }

    printf("└──────┴────────────────────────────────┴─────────┴───────────────┴───────────┴───────────┴─────────┴──────────┴─────────────┴─────────────────┴────────────────┘\n");
    return 0;
}

static int show_keys(void *data)
{
    keys_table_t *keys = &((sort_data_t *)data)->keys;

    printf("  Таблица ключей:\n\n"
           "┌──────┬───────────────┐\n"
           "│  ID  │ Кол-во комнат │\n"
           "├──────┼───────────────┤\n");

    for (uint32_t i = 0; i < keys->size; i++)
    {
        cio_colorize_set(i);
        printf("│ %04u │ %13d │\n", keys->keys[i].id, keys->keys[i].rooms_amount);
        cio_colorize_flush();
    }

    printf("└──────┴───────────────┘\n");
    return 0;
}

static int show_table_by_keys(void *data)
{
    flat_table_t *table = ((sort_data_t *)data)->table;
    keys_table_t *keys = &((sort_data_t *)data)->keys;

    printf("  Исходная таблица (по ключам):\n\n"
           "┌──────┬────────────────────────────────┬─────────┬───────────────┬───────────┬───────────┬─────────┬──────────┬─────────────┬─────────────────┬────────────────┐\n"
           "│  ID  │ Адрес квартиры                 │ Площадь │ Кол-во комнат │ Стоимость │ Тип       │ Отделка │ Животные │ Построено   │ Кол-во собст-ов │ Кол-во жильцов │\n"
           "├──────┼────────────────────────────────┼─────────┼───────────────┼───────────┼───────────┼─────────┼──────────┼─────────────┼─────────────────┼────────────────┤\n");

    for (uint32_t i = 0; i < table->size; i++)
    {
        cio_colorize_set(i);
        printf_flat(keys->keys[i].id, table->flats_array + keys->keys[i].id);
        cio_colorize_flush();
    }

    printf("└──────┴────────────────────────────────┴─────────┴───────────────┴───────────┴───────────┴─────────┴──────────┴─────────────┴─────────────────┴────────────────┘\n");
    return 0;
}

static int show_table_and_keys(void *data)
{
    flat_table_t *table = ((sort_data_t *)data)->table;
    keys_table_t *keys = &((sort_data_t *)data)->keys;

    printf("  Таблица ключей и исходная таблица:\n\n"
           "┌──────┬───────────────┐   ┌──────┬────────────────────────────────┬─────────┬───────────────┬───────────┬───────────┬─────────┬──────────┬─────────────┬─────────────────┬────────────────┐\n"
           "│  ID  │ Кол-во комнат │   │  ID  │ Адрес квартиры                 │ Площадь │ Кол-во комнат │ Стоимость │ Тип       │ Отделка │ Животные │ Построено   │ Кол-во собст-ов │ Кол-во жильцов │\n"
           "├──────┼───────────────┤   ├──────┼────────────────────────────────┼─────────┼───────────────┼───────────┼───────────┼─────────┼──────────┼─────────────┼─────────────────┼────────────────┤\n");

    for (uint32_t i = 0; i < table->size; i++)
    {
        cio_colorize_set(i);
        printf("│ %04u │ %13d │   ", keys->keys[i].id, keys->keys[i].rooms_amount);
        printf_flat(i, table->flats_array + i);
        cio_colorize_flush();
    }

    printf("└──────┴───────────────┘   └──────┴────────────────────────────────┴─────────┴───────────────┴───────────┴───────────┴─────────┴──────────┴─────────────┴─────────────────┴────────────────┘\n");
    return 0;
}

static int apply_sort(void *data)
{
    flat_table_t *table = ((sort_data_t *)data)->table;
    keys_table_t *keys = &((sort_data_t *)data)->keys;

    printf("  Выполняется сортировка...\n");
    sort_params_t sort_params = (sort_params_t){
        .ascending = true,
        .real_sort = false};
    uint32_t delta_a_fast = ts_sort_a_fast(table, keys, sort_params);
    uint32_t delta_b_fast = ts_sort_b_fast(table, keys, sort_params);
    uint32_t delta_a_slow = ts_sort_a_slow(table, keys, sort_params);
    uint32_t delta_b_slow = ts_sort_b_slow(table, keys, sort_params);

    sort_params.real_sort = true;
    ts_sort_b_fast(table, keys, sort_params);

    printf("  Сортировка завершена.\n"
           "  Результаты сортировки (по времени):\n\n"
           "┌──────────────────┬────────────┬──────────────────┐\n"
           "│  Тип сортировки  │ По ключам? │ Время сортировки │\n"
           "├──────────────────┼────────────┼──────────────────┤\n"
           "│  Сорт. обменом   │        нет │ %13u tc │\n"
           "│  Сорт. обменом   │         да │ %13u tc │\n"
           "│  Сорт. слиянием  │        нет │ %13u tc │\n"
           "│  Сорт. слиянием  │         да │ %13u tc │\n"
           "└──────────────────┴────────────┴──────────────────┘\n",
           delta_a_slow, delta_b_slow, delta_a_fast, delta_b_fast);

#ifdef _PERF_TEST
    fprintf(stderr, "%lu %lu %lu %lu\n", delta_a_slow, delta_b_slow, delta_a_fast, delta_b_fast);
#endif
    return 0;
}
