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

/*
    Working pipeline:

    1. Request flat table file name
    2. Read flat table
    3. [Operate with table] (menu loop)
    4. Request output file name (if "", use original file)
    5. Save data to output file
*/

/*
static bool imp__confirm(const char *msg)
{
    printf("%s\nВы уверены? [y/N] ", msg);
    char temp[256];
    fgets(temp, 256, stdin);
    while (strlen(temp) > 0 && (temp[strlen(temp) - 1] == '\r' || temp[strlen(temp) - 1] == '\n'))
        temp[strlen(temp) - 1] = '\0';
    return strlen(temp) == 1 && (temp[0] == 'y' || temp[0] == 'Y');
}
*/

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
                imp__pause();
                break;

            case 3:
                system("clear");
                output_flat_table(&state);
                imp__pause();
                break;
            
            case 4:
                status = search_flat(&state);
                imp__pause();
                break;
            
            case 5:
                status = delete_flat(&state);
                imp__pause();
                break;
        }
    }

    free_app_state(&state);
    return status;
}


int test(void)
{
    // TEST
    const char *input_filename = "in.txt";

    FILE *input_file = fopen(input_filename, "rt");
    if (input_file == NULL)
    {
        printf("cant open file \"%s\"\n", input_filename);
        return -1;
    }

    // try to read flat table
    flat_table_t table = create_flat_table();

    int status = fread_flat_table(input_file, &table);
    if (status != 0)
    {
        printf("bad read table status = %d\n", status);
    }
    else
    {
        printf("unsorted table:\n");
        fwrite_flat_table(stdout, &table);
        printf("sorted table:\n");
        //sort_flat_table(&table, ROOMS_AMOUNT, true);
        fwrite_flat_table(stdout, &table);
    }

    free_flat_table(&table);
    fclose(input_file);
    return 0;
}


int main(void)
{
    return run_menu_loop();
}
