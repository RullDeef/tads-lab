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
static int structs_diff(int argc, const char **argv);
static int ht_restruct(int argc, const char **argv);

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
    uki_menu_cmd_opt_add(menu, "diff", structs_diff, "Показать сравнительную таблицу.");
    uki_menu_cmd_opt_add(menu, "restruct", ht_restruct, "Реструктуризировать хеш-таблицу.");
    uki_menu_cmd_opt_add(menu, "help", uki_default_cmd_help, "Вывести эту памятку.");
    uki_menu_cmd_opt_add(menu, "exit", uki_default_cmd_exit, "Выход из программы.");

    uki_menu_cmd_run(menu);

    uki_menu_destroy(menu);
    return EXIT_SUCCESS;
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
        unsigned int space_bef = htw_calc_remain_space(ht_wp);
        if (space_bef == 0U)
            printf(CLR_BR_RED "Внимание." CLR_RESET " Хеш-таблица заполнена. Новое число не будет вставлено в хеш-таблицу.\n");

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

        printf("Добавлен ключ " CLR_CYAN "%d" CLR_RESET ".\n", num);

        unsigned int space = htw_calc_remain_space(ht_wp);
        if (space == 1U || space == 2U)
            printf(CLR_BR_YELLOW "Внимание." CLR_RESET " В таблице осталось место еще для " CLR_BR_YELLOW "%u" CLR_RESET " ключей.\n", space);
        else if (space == 0U)
            printf(CLR_BR_RED "Внимание." CLR_RESET " Хеш-таблица полностью заполнена.\n");

        printf("\n");
        printf("  Структура  |    Время вставки   | Размер структуры | Ср. число сравнений\n");
        printf("     ДДП     |  %10llu тактов |    %5lu байт    |      %4.1f\n", bst_time, bstw_calc_size(bst_wp), bstw_calc_mean_cmp_amount(bst_wp));
        printf("     AVL     |  %10llu тактов |    %5lu байт    |      %4.1f\n", avl_time, avlw_calc_size(avl_wp), avlw_calc_mean_cmp_amount(avl_wp));
        if (space_bef != 0U)
            printf(" Хеш-таблица |  %10llu тактов |    %5lu байт    |      %4.1f\n", ht_time, htw_calc_size(ht_wp), htw_calc_mean_cmp_amount(ht_wp));
        else
            printf(" Хеш-таблица |  ------------- |    %5lu байт    |      %4.1f\n", htw_calc_size(ht_wp), htw_calc_mean_cmp_amount(ht_wp));
        printf("    Файл     |  %10llu тактов |    %5lu байт    |      %4.1f\n", f_time, fw_calc_size(f_wp), fw_calc_mean_cmp_amount(f_wp));
    }

    return EXIT_SUCCESS;
}

static int structs_show(int argc, const char **argv)
{
    bool printed = false;

    if (argc == 1)
    {
        bstw_fprintf(stdout, bst_wp);
        avlw_fprintf(stdout, avl_wp);
        htw_fprintf(stdout, ht_wp);
        fw_fprintf(stdout, f_wp);
        printed = true;
    }

    for (int i = 1; i < argc; i++)
    {
        if (strcmp(argv[i], "bst") == 0)
        {
            bstw_fprintf(stdout, bst_wp);
            printed = true;
        }
        else if (strcmp(argv[i], "avl") == 0)
        {
            avlw_fprintf(stdout, avl_wp);
            printed = true;
        }
        else if (strcmp(argv[i], "ht") == 0)
        {
            htw_fprintf(stdout, ht_wp);
            printed = true;
        }
        else if (strcmp(argv[i], "file") == 0)
        {
            fw_fprintf(stdout, f_wp);
            printed = true;
        }
    }

    if (!printed)
        printf("Использование: show [bst|avl|ht|file]\n");

    return EXIT_SUCCESS;
}

static int structs_diff(int argc, const char **argv)
{
    (void)argc;
    (void)argv;

    printf("  Структура  | Размер структуры | Ср. число сравнений\n");
    printf("     ДДП     |    " CLR_BR_GREEN "%5lu" CLR_RESET " байт    |      " CLR_BR_GREEN "%4.1f" CLR_RESET "\n", bstw_calc_size(bst_wp), bstw_calc_mean_cmp_amount(bst_wp));
    printf("     AVL     |    " CLR_BR_GREEN "%5lu" CLR_RESET " байт    |      " CLR_BR_GREEN "%4.1f" CLR_RESET "\n", avlw_calc_size(avl_wp), avlw_calc_mean_cmp_amount(avl_wp));
    printf(" Хеш-таблица |    " CLR_BR_GREEN "%5lu" CLR_RESET " байт    |      " CLR_BR_GREEN "%4.1f" CLR_RESET "\n", htw_calc_size(ht_wp), htw_calc_mean_cmp_amount(ht_wp));
    printf("    Файл     |    " CLR_BR_GREEN "%5lu" CLR_RESET " байт    |      " CLR_BR_GREEN "%4.1f" CLR_RESET "\n", fw_calc_size(f_wp), fw_calc_mean_cmp_amount(f_wp));

    return EXIT_SUCCESS;
}

static int ht_restruct(int argc, const char **argv)
{
    (void)argc;
    (void)argv;
    htw_fprintf(stdout, ht_wp);

    printf("Укажите новый размер хеш-таблицы (0 - подобрать автоматически): ");
    unsigned int new_size = 0U;
    scanf("%u", &new_size);
    char buf[20];
    fgets(buf, 20, stdin);
    if (new_size == 0U)
    {
        new_size = htw_calc_best_size(ht_wp);
        printf("Размер, подобранный автоматически: %u\n", new_size);
    }
    else if (new_size < htw_get_keys_amount(ht_wp))
    {
        printf("Введеный размер меньше числа ключей в таблице. Используем число ключей в таблице.\n");
        new_size = htw_get_keys_amount(ht_wp);
        printf("Новый размер: %u\n", new_size);
    }

    uki_menu_t menu = uki_menu_create_ctx();
    uki_menu_ctx_info_set(menu, "Выберите новую хеш-функцию");

    uki_menu_ctx_opt_add(menu, "сложение цифр числа", (void *)hash_func_1);
    uki_menu_ctx_opt_add(menu, "сложение цифр числа с солью", (void *)hash_func_2);
    uki_menu_ctx_opt_add(menu, "хеширование Фибоначчи", (void *)hash_func_3);

    hash_func_t new_func = NULL;
    uki_menu_ctx_run(menu, (void **)&new_func);

    if (new_func != NULL)
    {
        htw_restruct(ht_wp, new_size, new_func);
        printf("Реструктуризация прошла успешно.\n");
    }
    else
        printf("Ошибка при выборе новой хеш-функции.\n");

    uki_menu_destroy(menu);

    return EXIT_SUCCESS;
}
