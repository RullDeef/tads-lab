#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "utils/logger.h"
#include "uki.h"
#include "menu.h"
#include "wrappers/bst/bst_wrapper.h"
#include "wrappers/avl/avl_wrapper.h"
#include "wrappers/hash_table/ht_wrapper.h"

static bool file_is_valid(FILE *data_file);
static int powered_main_menu(FILE *data_file);

int menu_main(void)
{
    int status = -1;

    char buffer[256] = "data.txt";
    FILE *data_file = NULL;

    printf("Введите имя файла для ввода данных: ");
    if (fgets(buffer, 256, stdin) != NULL)
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
        printf("Неверное содержание файла.\n");
    else
        status = powered_main_menu(data_file);

    fclose(data_file);
    return status;
}

static bool file_is_valid(FILE *data_file)
{
    int num;

    while (fscanf(data_file, "%d", &num) == 1)
        ;

    return fscanf(data_file, "%d", &num) == EOF;
}

static int powered_main_menu(FILE *data_file)
{
    uki_menu_t menu = uki_menu_create_cmd();

    if (!menu)
    {
        log_error("Не удалось создать главное меню");
        return -1;
    }

    // create BST from numbers in file
    struct bst_wrapper bstw;
    bstw_fscanf(data_file, &bstw);

    // print out
    log_info("BST:");
    bstw_fprintf(stdout, &bstw);

    struct avl_wrapper avlw;
    avlw_fscanf(data_file, &avlw);

    log_info("AVL:");
    avlw_fprintf(stdout, &avlw);

    log_info("loading hash table");

    struct ht_wrapper htw;
    htw_fscanf(data_file, &htw);

    log_info("HASH TABLE:");
    htw_fprintf(stdout, &htw);

    bstw_destroy(&bstw);
    avlw_destroy(&avlw);
    htw_destroy(&htw);

    uki_menu_destroy(menu);
    return 0;
}