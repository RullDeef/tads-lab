#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include "operations.h"

static void imp__pause()
{
    char temp[2];
    fgets(temp, 2, stdin);
}

int get_menu_opt()
{
    system("clear");
    printf("  [__Меню:__]\n\n"
        " 1. Добавить запись в таблицу\n"
        " 2. Отсортировать таблицу\n"
        " 3. Вывести таблицу на экран\n"
        " 4. Произвести поиск квартиры\n"
        " 5. Удалить запись\n"
        " 0. Выход\n\n"
        "[Ваш выбор]>>> ");
    char opt[256];
    fgets(opt, 256, stdin);
    while (strlen(opt) > 0 && (opt[strlen(opt) - 1] == '\r' || opt[strlen(opt) - 1] == '\n'))
        opt[strlen(opt) - 1] = '\0';
    if (strlen(opt) == 1)
        if (opt[0] >= '0' && opt[0] <= '5')
            return opt[0] - '0';
    return -1;
}

int main()
{
    int status = 0;
    char ifname[MAX_FILE_NAME_LENGTH];
    flat_table_t table;

    status = request_input_filename(ifname);
    if (status == 0)
    {
        status = read_table_from_file(ifname, &table);
        if (status != 0)
            printf("Не удалось считать данные из файла.\n");
    }
    else
        printf("Некорректное имя файла.\n");

    if (status != 0)
        return status;

    bool need_exit = false;
    while (!need_exit)
    {
        int opt = get_menu_opt();

        switch (opt)
        {
            default:
                printf("\nВы неправильно ввели номер опции. Повторите попытку.\n");
                imp__pause();
                system("clear");
                break;
            
            case 0:
                printf("\nВыход из программы...");
                need_exit = true;
                break;
            
            case 1:
                system("clear");
                append_flat_to_table(&table);
                imp__pause();
                break;

            case 2:
                system("clear");
                sort_table(&table);
                imp__pause();
                break;

            case 3:
                system("clear");
                output_flat_table(&table, NULL);
                imp__pause();
                break;
            
            case 4:
                system("clear");
                search_flat(&table);
                imp__pause();
                break;

            case 5:
                system("clear");
                output_flat_table(&table, NULL);
                delete_flat(&table);
                imp__pause();
                break;
        }
    }

    return status;
}
