#include <stdio.h>
#include <string.h>
#include "callocs.h"
#include "uki.h"
#include "wrappers/bst/bst_wrapper.h"
#include "wrappers/avl/avl_wrapper.h"

int main_menu(void);
int powered_main_menu(FILE *data_file);

int main(void)
{
    if (callocs_init(20U * 1024U))
    {
        fprintf(stderr, "Не удалось инициализировать пул памяти.\n");
        return -1;
    }

    int status = uki_init();

    if (status)
        fprintf(stderr, "Не удалось инициализировать ядро.\n");
    else
    {
        status = main_menu();
        printf("done.\n");
        uki_destroy();
    }

    callocs_destroy();
    return status;
}

int main_menu(void)
{
    int status = 0;
    uki_menu_t menu = uki_menu_create_ctx();

    if (!menu)
    {
        fprintf(stderr, "Не удалось создать главное меню.\n");
        status = -1;
    }
    else
    {
        char buffer[256] = "data.txt";
        FILE *data_file = NULL;

        // printf("Введите имя файла для ввода данных: ");
        // if (fgets(buffer, 256, stdin) != NULL)
        // {
        //     while (buffer[strlen(buffer) - 1] == '\n' || buffer[strlen(buffer) - 1] == '\r')
        //         buffer[strlen(buffer) - 1] = '\0';
        //     printf("%s\n", buffer);
               data_file = fopen(buffer, "rt");
        // }

        if (data_file == NULL)
        {
            fprintf(stderr, "Неверное имя файла.\n");
            status = -2;
        }
        else
            status = powered_main_menu(data_file);

        fclose(data_file);
    }

    return status;
}

static struct bst *load_bst(FILE *file)
{
    struct bst *tree = bst_create();

    int number;
    while (fscanf(file, "%d", &number) == 1)
    {
        if (bst_insert(&tree, number) != 0)
        {
            bst_destroy(&tree);
            break;
        }
    }

    return tree;
}

static struct avl *load_avl(FILE *file)
{
    struct avl *tree = avl_create();

    rewind(file);

    int number;
    while (fscanf(file, "%d", &number) == 1)
    {
        if (avl_insert(&tree, number) != 0)
        {
            avl_destroy(&tree);
            break;
        }
    }

    return tree;
}

int powered_main_menu(FILE *data_file)
{
    // create BST from numbers in file
    struct bst_wrapper bstw;
    bstw.root = load_bst(data_file);

    // print out
    printf("BST:\n");
    bstw_fprintf(stdout, &bstw);

    struct avl_wrapper avlw;
    avlw.root = load_avl(data_file);

    printf("AVL:\n");
    avlw_fprintf(stdout, &avlw);

    return 0;
}
