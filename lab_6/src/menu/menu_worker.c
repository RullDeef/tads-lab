#include <stdio.h>
#include <stdlib.h>
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
static int structs_find(int argc, const char **argv);
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
    uki_menu_cmd_opt_add(menu, "find <num>", structs_find, "Найти ключ во всех структурах.");
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
    int cmp = 0;

    if (argc != 2 || parse_int(argv[1], &num) != EXIT_SUCCESS)
        printf("Использование: insert <num>\n");
    else if (avlw_find(avl_wp, num, &cmp) != NULL)
        printf("Ключ %d уже есть в структурах. Попробуйте добавить другой.\n", num);
    else
    {
        unsigned int space_bef = htw_calc_remain_space(ht_wp);
        if (space_bef == 0U)
            printf(CLR_BR_RED "Внимание." CLR_RESET " Хеш-таблица заполнена. Новое число не будет вставлено в хеш-таблицу.\n");

        float bst_time = 0, avl_time = 0, ht_time = 0, f_time = 0;
        int cmp_bst = 0, cmp_avl = 0, cmp_ht = 0;
        const size_t inserts_count = 50000UL;

        {
            BEGIN_TIMER;
            for (size_t n = 0UL; n < inserts_count; n++)
                bstw_shallow_insert(bst_wp, num);
            bstw_insert(bst_wp, num, &cmp_bst);
            END_TIMER;
            bst_time = (float) TIMER_MICROSECONDS / inserts_count;
        }

        {
            BEGIN_TIMER;
            for (size_t n = 0UL; n < inserts_count; n++)
                avlw_shallow_insert(avl_wp, num);
            avlw_insert(avl_wp, num, &cmp_avl);
            END_TIMER;
            avl_time = (float) TIMER_MICROSECONDS / inserts_count;
        }

        {
            BEGIN_TIMER;
            for (size_t n = 0UL; n < inserts_count; n++)
                htw_shallow_insert(ht_wp, num);
            htw_insert(ht_wp, num, &cmp_ht);
            END_TIMER;
            ht_time = (float) TIMER_MICROSECONDS / inserts_count;
        }

        {
            BEGIN_TIMER;
            for (size_t n = 0UL; n < inserts_count; n++)
                fw_shallow_insert(f_wp, num);
            fw_insert(f_wp, num);
            END_TIMER;
            f_time = (float) TIMER_MICROSECONDS / inserts_count;
        }

        printf("Добавлен ключ " CLR_CYAN "%d" CLR_RESET ".\n", num);

        unsigned int space = htw_calc_remain_space(ht_wp);
        if (space == 1U || space == 2U)
            printf(CLR_BR_YELLOW "Внимание." CLR_RESET " В таблице осталось место еще для " CLR_BR_YELLOW "%u" CLR_RESET " ключей.\n", space);
        else if (space == 0U)
            printf(CLR_BR_RED "Внимание." CLR_RESET " Хеш-таблица полностью заполнена.\n");

        printf("\n");
        printf("  Структура  |    Время вставки   | Размер структуры | Число сравнений при вставке\n");
        printf("     ДДП     |    %10.3f мкс  |    %5lu байт    |      %4d\n", bst_time, bstw_calc_size(bst_wp), cmp_bst);
        printf("     AVL     |    %10.3f мкс  |    %5lu байт    |      %4d\n", avl_time, avlw_calc_size(avl_wp), cmp_avl);
        if (space_bef != 0U)
            printf(" Хеш-таблица |    %10.3f мкс  |    %5lu байт    |      %4d\n", ht_time, htw_calc_size(ht_wp), cmp_ht);
        else
            printf(" Хеш-таблица |  ------------- |    %5lu байт    |      %4d\n", htw_calc_size(ht_wp), cmp_ht);
        printf("    Файл     |    %10.3f мкс  |    %5lu байт    |      -\n", f_time, fw_calc_size(f_wp));
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

static int structs_find(int argc, const char **argv)
{
    int num;

    if (argc != 2 || parse_int(argv[1], &num) != EXIT_SUCCESS)
        printf("Использование: find <num>\n");
    else
    {
        float bst_time, avl_time, ht_time, f_time;
        int cmp_bst = 0, cmp_avl = 0, cmp_ht = 0, cmp_f = 0;
        size_t finds_count = 50000UL;

        struct bst *bst_res;
        struct avl *avl_res;
        int ht_res;
        int f_res;

        {
            BEGIN_TIMER;
            for (size_t n = 0UL; n < finds_count; n++)
                bst_res = bstw_find(bst_wp, num, &cmp_bst);
            END_TIMER;
            bst_time = (float) TIMER_MICROSECONDS / finds_count;
        }

        {
            BEGIN_TIMER;
            for (size_t n = 0UL; n < finds_count; n++)
                avl_res = avlw_find(avl_wp, num, &cmp_avl);
            END_TIMER;
            avl_time = (float) TIMER_MICROSECONDS / finds_count;
        }

        {
            BEGIN_TIMER;
            for (size_t n = 0UL; n < finds_count; n++)
                ht_res = htw_find(ht_wp, num, &cmp_ht);
            END_TIMER;
            ht_time = (float) TIMER_MICROSECONDS / finds_count;
        }

        {
            BEGIN_TIMER;
            for (size_t n = 0UL; n < finds_count; n++)
                f_res = fw_find(f_wp, num, &cmp_f);
            END_TIMER;
            f_time = (float) TIMER_MICROSECONDS / finds_count;
        }

        printf("Результаты поиска ключа " CLR_CYAN "%d" CLR_RESET ".\n\n", num);

        printf("  Структура  | Ключ |    Время поиска    | Размер структуры | Число сравнений при поиске\n");
        printf("     ДДП     | %4s |    %10.3f мкс  |    %5lu байт    |      %4lu\n", (bst_res ? CLR_BR_GREEN "есть" CLR_RESET : CLR_RED " нет" CLR_RESET), bst_time, bstw_calc_size(bst_wp), cmp_bst / finds_count);
        printf("     AVL     | %4s |    %10.3f мкс  |    %5lu байт    |      %4lu\n", (avl_res ? CLR_BR_GREEN "есть" CLR_RESET : CLR_RED " нет" CLR_RESET), avl_time, avlw_calc_size(avl_wp), cmp_avl / finds_count);
        printf(" Хеш-таблица | %4s |    %10.3f мкс  |    %5lu байт    |      %4lu\n", (!ht_res ? CLR_BR_GREEN "есть" CLR_RESET : CLR_RED " нет" CLR_RESET), ht_time, htw_calc_size(ht_wp), cmp_ht / finds_count);
        printf("    Файл     | %4s |    %10.3f мкс  |    %5lu байт    |      %4lu\n", (!f_res ? CLR_BR_GREEN "есть" CLR_RESET : CLR_RED " нет" CLR_RESET), f_time, fw_calc_size(f_wp), cmp_f / finds_count);
    }

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
    // uki_menu_ctx_opt_add(menu, "сложение цифр числа с солью", (void *)hash_func_2);
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
