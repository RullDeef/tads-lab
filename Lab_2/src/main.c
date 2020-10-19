#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include "uki.h"
#include "sort_menu.h"
#include "main_menu.h"

#define MAX_FILE_NAME_LENGTH 128

int main()
{
    char ifname[MAX_FILE_NAME_LENGTH];
    flat_table_t table;

    if (!uki_input_str("Введите имя файла: ", "Неверное имя файла.\n", ifname, MAX_FILE_NAME_LENGTH))
        return -1;

    if (read_table_from_file(ifname, &table))
    {
        printf("Не удалось считать данные из файла.\n");
        return -2;
    }

    uki_menu_t menu = uki_menu_create("Меню", 6,
        "Показать таблицу", show_table,
        "Добавить квартиру", append_flat,
        "Удалить квартиру", delete_flat,
        "Найти квартиру", search_flat,
        "Сотрировать таблицу", sort_menu,
        "Выход", uki_menu_opt_exit
    );

    return uki_menu_run(&menu, (void*)&table);
}
