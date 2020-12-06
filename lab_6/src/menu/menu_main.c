#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "utils/timing.h"
#include "utils/logger.h"
#include "uki.h"
#include "menu.h"

#include "wrappers/bst/bst_wrapper.h"
#include "wrappers/avl/avl_wrapper.h"
#include "wrappers/hash_table/ht_wrapper.h"
#include "wrappers/file/file_wrapper.h"

#define INPUT_BUFFER_SIZE 256U

static bool file_is_valid(FILE *data_file);
static int powered_main_menu(const char *data_file_name);

int menu_main(void)
{
    int status = -1;

    char buffer[INPUT_BUFFER_SIZE];
    FILE *data_file = NULL;

    printf("Введите имя файла для ввода данных: ");
    if (fgets(buffer, INPUT_BUFFER_SIZE, stdin) != NULL)
    {
        while (buffer[strlen(buffer) - 1] == '\n' || buffer[strlen(buffer) - 1] == '\r')
            buffer[strlen(buffer) - 1] = '\0';
        log_info("Файл: \"%s\"", buffer);
        printf("\n");
        data_file = fopen(buffer, "rt");
    }

    if (data_file == NULL)
        printf("Неверное имя файла.\n");
    else if (!file_is_valid(data_file))
    {
        printf("Неверное содержание файла.\n");
        fclose(data_file);
    }
    else
    {
        fclose(data_file);
        status = powered_main_menu(buffer);
    }

    return status;
}

static bool file_is_valid(FILE *data_file)
{
    int num;

    while (fscanf(data_file, "%d", &num) == 1)
        ;

    return fscanf(data_file, "%d", &num) == EOF;
}

static int powered_main_menu(const char *data_file_name)
{
    int status = 0;

    // create file wrapper
    struct file_wrapper fw = fw_create(data_file_name);

    struct bst_wrapper bstw;
    struct avl_wrapper avlw;
    struct ht_wrapper htw;

    BEGIN_TIMER;
    status = bstw_fscanf(fw.file, &bstw);
    END_TIMER;

    if (status != 0)
    {
        log_error("Невозможно создать ДДП.");
        status = -1;
    }
    else
    {
        printf("Загружено ДДП         за %llu тактов и %.2f нс.\n", TIMER_TICKS, TIMER_NANOSECONDS);

        BEGIN_TIMER;
        status = avlw_fscanf(fw.file, &avlw);
        END_TIMER;

        if (status != 0)
        {
            log_error("Невозможно создать AVL дерево.");
            status = -2;
        }
        else
        {
            printf("Загружено AVL дерево  за %llu тактов и %.2f нс.\n", TIMER_TICKS, TIMER_NANOSECONDS);

            BEGIN_TIMER;
            status = htw_fscanf(fw.file, &htw);
            END_TIMER;

            if (status != 0)
            {
                log_error("Невозможно создать хеш-таблицу.");
                status = -2;
            }
            else
            {
                printf("Загружена хеш-таблица за %llu тактов и %.2f нс.\n", TIMER_TICKS, TIMER_NANOSECONDS);

                log_info("Все структуры созданы успешно.");
                status = menu_worker(&bstw, &avlw, &htw, &fw);
                htw_destroy(&htw);
            }
            avlw_destroy(&avlw);
        }
        bstw_destroy(&bstw);
    }

    fw_destroy(&fw);
    return status;
}