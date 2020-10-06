#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include "operations.h"

typedef enum
{
    REQUEST_INPUT_FILE_NAME,
    REQUEST_OUTPUT_FILE_NAME,
    RELOAD_TABLE
} menu_option_t;

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
        " 3. Вывести часть таблицы на экран\n"
        " 4. Произвести поиск квартиры\n"
        " 5. Удалить запись\n"
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

int run_menu_loop()
{
    app_state_t state = create_app_state();
    int status = 0;
    bool need_exit = false;

    status = request_input_filename(&state);
    if (status == 0)
    {
        status = read_table_from_file(&state);
        if (status != 0)
            printf("Не удалось считать данные из файла.\n");
    }
    else
        printf("Некорректное имя файла.\n");

    while (!need_exit && status == 0)
    {
        int opt = get_menu_opt();

        switch (opt)
        {
            default:
            case -1:
                printf("Вы неправильно ввели номер опции. Повторите попытку.\n");
                imp__pause();
                break;
            
            case 0:
                printf("Выход из программы...");
                need_exit = true;
                break;
            
            case 1:
                status = append_flat_to_table(&state);
                if (status == 0)
                    printf("Успешно добавлена новая запись в таблицу.\n");
                else
                    printf("При обработки ввода произошла ошибка.\n");
                status = 0;
                imp__pause();
                break;

            case 2:
                printf("Выполяется сортировка таблицы...\n");
                status = sort_table(&state);
                if (status == 0)
                {
                    printf("Сортировка выполнена!\n\n");
                    output_flat_table(&state);
                }
                else
                    printf("При сортировке возникли ошибки.\n\n");
                status = 0;
                imp__pause();
                break;

            case 3:
                system("clear");
                output_flat_table(&state);
                imp__pause();
                break;
            
            case 4:
                search_flat(&state);
                imp__pause();
                break;
            
            case 5:
                delete_flat(&state);
                imp__pause();
                break;
        }
    }

    free_app_state(&state);
    return status;
}

int main(void)
{
    return run_menu_loop();
}
