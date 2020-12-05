#include <stdio.h>
#include <string.h>
#include "uki.h"
#include "./menu.h"

static struct bst_wrapper *bst_wp;
static struct avl_wrapper *avl_wp;
static struct ht_wrapper *ht_wp;
static struct file_wrapper *f_wp;

static int structs_show(int argc, const char **argv);

int menu_worker(struct bst_wrapper *bstw, struct avl_wrapper *avlw, struct ht_wrapper *htw, struct file_wrapper *fw)
{
    bst_wp = bstw;
    avl_wp = avlw;
    ht_wp = htw;
    f_wp = fw;

    uki_menu_t menu = uki_menu_create_cmd();
    uki_menu_cmd_info_set(menu, PROG_INTRO);

    uki_menu_cmd_opt_add(menu, "show [bst|avl|ht|file]", structs_show, "Вывести структуру на экран (или все сразу).");
    uki_menu_cmd_opt_add(menu, "exit", uki_default_cmd_exit, "Выход из программы.");

    uki_menu_cmd_run(menu);

    uki_menu_destroy(menu);
    return 0;
}

static int structs_show(int argc, const char **argv)
{
    printf("argc = %d, argv = %s", argc, argv[0]);
    bool printed = false;

    if (argc == 1 || (argc == 2 && strcmp(argv[0], "bst") == 0))
    {
        bstw_fprintf(stdout, bst_wp);
        printed = true;
    }

    if (argc == 1 || (argc == 2 && strcmp(argv[0], "avl") == 0))
    {
        avlw_fprintf(stdout, avl_wp);
        printed = true;
    }

    if (argc == 1 || (argc == 2 && strcmp(argv[0], "ht") == 0))
    {
        htw_fprintf(stdout, ht_wp);
        printed = true;
    }

    if (argc == 1 || (argc == 2 && strcmp(argv[0], "file") == 0))
    {
        fw_fprintf(stdout, f_wp);
        printed = true;
    }

    if (!printed)
        printf("Использование: show [bst|avl|ht|file]\n");

    return 0;
}
