#include <stdio.h>
#include <string.h>
#include "uki.h"
#include "./menu.h"

#include "utils/timing.h"
#include "utils/str_parser.h"

static struct bst_wrapper *bst_wp;
static struct avl_wrapper *avl_wp;
static struct ht_wrapper *ht_wp;
static struct file_wrapper *f_wp;

static int structs_insert(int argc, const char **argv);
static int structs_show(int argc, const char **argv);

int menu_worker(struct bst_wrapper *bstw, struct avl_wrapper *avlw, struct ht_wrapper *htw, struct file_wrapper *fw)
{
    bst_wp = bstw;
    avl_wp = avlw;
    ht_wp = htw;
    f_wp = fw;

    uki_menu_t menu = uki_menu_create_cmd();
    uki_menu_cmd_info_set(menu, PROG_INTRO);

    uki_menu_cmd_opt_add(menu, "insert <num>", structs_insert, "Добавить ключ во все структуры.");
    uki_menu_cmd_opt_add(menu, "show [bst|avl|ht|file]", structs_show, "Вывести структуру на экран (или все сразу).");
    uki_menu_cmd_opt_add(menu, "exit", uki_default_cmd_exit, "Выход из программы.");

    uki_menu_cmd_run(menu);

    uki_menu_destroy(menu);
    return 0;
}

static int structs_insert(int argc, const char **argv)
{
    int num = 0;

    if (argc != 2 || parse_int(argv[1], &num) != EXIT_SUCCESS)
        printf("Использование: insert <num>\n");
    else if (avlw_find(avl_wp, num) != NULL)
        printf("Ключ %d уже есть в структурах. Попробуйте добавить другой.\n", num);
    else
    {
        unsigned long long bst_time, avl_time, ht_time, f_time;

        {
            BEGIN_TIMER;
            bstw_insert(bst_wp, num);
            END_TIMER;
            bst_time = TIMER_TICKS;
        }

        {
            BEGIN_TIMER;
            avlw_insert(avl_wp, num);
            END_TIMER;
            avl_time = TIMER_TICKS;
        }

        {
            BEGIN_TIMER;
            htw_insert(ht_wp, num);
            END_TIMER;
            ht_time = TIMER_TICKS;
        }

        {
            BEGIN_TIMER;
            fw_insert(f_wp, num);
            END_TIMER;
            f_time = TIMER_TICKS;
        }

        printf("Ключ %d успешно добавлен во все структуры.\n\n", num);

        uki_table_t table = uki_table_create(5, 3U);

        uki_table_write(table, 0U, 0U, " struct ");
        uki_table_write(table, 0U, 1U, " insert time ");
        uki_table_write(table, 0U, 2U, " struct size ");

        uki_table_write(table, 1U, 0U, " BST ");
        uki_table_write(table, 2U, 0U, " AVL ");
        uki_table_write(table, 3U, 0U, " hash table ");
        uki_table_write(table, 4U, 0U, " file ");

        const size_t buf_size = 64U;
        char buf[buf_size];

        snprintf(buf, buf_size, " %llu ticks ", bst_time);
        uki_table_write(table, 1U, 1U, buf);

        snprintf(buf, buf_size, " %llu ticks ", avl_time);
        uki_table_write(table, 2U, 1U, buf);

        snprintf(buf, buf_size, " %llu ticks ", ht_time);
        uki_table_write(table, 3U, 1U, buf);

        snprintf(buf, buf_size, " %llu ticks ", f_time);
        uki_table_write(table, 4U, 1U, buf);

        snprintf(buf, buf_size, " %lu Bytes ", bstw_calc_size(bst_wp));
        uki_table_write(table, 1U, 2U, buf);

        snprintf(buf, buf_size, " %lu Bytes ", avlw_calc_size(avl_wp));
        uki_table_write(table, 2U, 2U, buf);

        snprintf(buf, buf_size, " %lu Bytes ", htw_calc_size(ht_wp));
        uki_table_write(table, 3U, 2U, buf);

        snprintf(buf, buf_size, " %lu Bytes ", fw_calc_size(f_wp));
        uki_table_write(table, 4U, 2U, buf);

        uki_table_print(table, stdout);
        uki_table_destroy(table);
    }

    return 0;
}

static int structs_show(int argc, const char **argv)
{
    bool printed = false;

    if (argc == 1)
    {
        printf("ДДП:\n");
        bstw_fprintf(stdout, bst_wp);
        printf("AVL:\n");
        avlw_fprintf(stdout, avl_wp);
        printf("Хеш-таблица:\n");
        htw_fprintf(stdout, ht_wp);
        printf("Файл:\n");
        fw_fprintf(stdout, f_wp);
        printed = true;
    }

    for (int i = 1; i < argc; i++)
    {
        if (strcmp(argv[i], "bst") == 0)
        {
            printf("ДДП:\n");
            bstw_fprintf(stdout, bst_wp);
            printed = true;
        }
        else if (strcmp(argv[i], "avl") == 0)
        {
            printf("AVL:\n");
            avlw_fprintf(stdout, avl_wp);
            printed = true;
        }
        else if (strcmp(argv[i], "ht") == 0)
        {
            printf("Хеш-таблица:\n");
            htw_fprintf(stdout, ht_wp);
            printed = true;
        }
        else if (strcmp(argv[i], "file") == 0)
        {
            printf("Файл:\n");
            fw_fprintf(stdout, f_wp);
            printed = true;
        }
    }

    if (!printed)
        printf("Использование: show [bst|avl|ht|file]\n");

    return 0;
}
