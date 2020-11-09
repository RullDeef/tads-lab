#include <stdio.h>
#include <stdlib.h>
#include "utils/str_parser.h"
#include "wrappers/stack_wrapper.h"
#include "menu/menu.h"

static int get_random()
{
    return rand() % 201 - 100;
}

static void apply_test(int lower_bound, int upper_bound)
{
    struct stack_wrapper sw_a = sw_wrap("\033[37;1;4mна массиве\033[0m", st_create(STACK_TYPE_ARRAY));
    struct stack_wrapper sw_l = sw_wrap("\033[37;1;4mна списке\033[0m", st_create(STACK_TYPE_LINKED_LIST));

    time_t delta_a = 0;
    time_t delta_l = 0;
    time_t dtime;

    while (st_get_size(&sw_a.stack) < (unsigned int)lower_bound)
    {
        int32_t value = get_random();
        sw_push(&sw_a, value, NULL);
        sw_push(&sw_l, value, NULL);
    }

    (void)delta_a;
    (void)delta_l;
    (void)dtime;
    (void)upper_bound;

    sw_destroy(&sw_a);
    sw_destroy(&sw_l);
}

static int auto_back(cmdf_arglist *arglist)
{
    printf("\n    Выход из автоматизированного режима.\n\n");
    cmdf__default_do_exit(arglist);
    return EXIT_SUCCESS;
}

static int recieve_test_bounds(cmdf_arglist *arglist, int *lower, int *upper)
{
    int status = EXIT_FAILURE;

    if (arglist->count == 1)
    {
        *lower = 0;
        status = parse_int32(arglist->args[0], upper) && *upper > 0;
    }
    else if (arglist->count == 2)
    {
        status = parse_int32(arglist->args[0], lower) && *lower >= 0
             &&  parse_int32(arglist->args[1], upper) && *upper > *lower;
    }

    return status;
}

static int auto_test(cmdf_arglist *arglist)
{
    if (!arglist)
        printf("Для данной команды укажите, в каких пределах Вы хотите тестировать.\n"
            "Например \033[37;1;4m\"test 10 40\"\033[0m протестирует работу со стеком от 10 до 40 элементов в нем.\n");
    else
    {
        int lower, upper;

        if (recieve_test_bounds(arglist, &lower, &upper) != EXIT_SUCCESS)
            printf("Введены некорректные аргументы.\n"
                "Воспользуйтесь командой \033[37;1;4m\"help auto\"\033[0m для получения информации о команде.\n");
        else
            apply_test(lower, upper);
    }

    return EXIT_SUCCESS;
}

int menu_auto(cmdf_arglist *arglist)
{
    if (arglist)
        printf("Для данной команды не нужны агрументы.\n");
    else
    {
        cmdf_init("\033[96;1;4mmain/auto>\033[0m ", NULL, DOC_HEADER, UNDOC_HEADER, '~', 0);

        cmdf_register_command(auto_back, "back", CMD_BACK_HELP);
        cmdf_register_command(auto_test, "test", "");
        cmdf_commandloop();
    }

    return EXIT_SUCCESS;
}
