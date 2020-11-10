#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "utils/str_parser.h"
#include "utils/colors.h"
#include "utils/timing.h"
#include "wrappers/stack_wrapper.h"
#include "menu/menu.h"

static int get_random()
{
    return rand() % 201 - 100;
}

static void print_status_percent(const char *str, float percent)
{
    if (percent > 0.0f)
        for (size_t i = 0; i < strlen(str) + 8; i++)
            printf("\b \b");
    printf("%s %5.2f%%", str, percent);
    fflush(stdout);
}

static void apply_test(int lower_bound, int upper_bound)
{
    struct stack_wrapper sw_a = sw_wrap(CLR_EMPH "на массиве" CLR_RESET, st_create(STACK_TYPE_ARRAY));
    struct stack_wrapper sw_l = sw_wrap(CLR_EMPH "на списке" CLR_RESET, st_create(STACK_TYPE_LINKED_LIST));

    size_t delta_a_push = 0;
    size_t delta_l_push = 0;
    size_t delta_a_pop = 0;
    size_t delta_l_pop = 0;
    size_t dtime;

    while (st_get_size(&sw_a.stack) < (unsigned int)lower_bound)
    {
        int32_t value = get_random();
        sw_push(&sw_a, value, NULL);
        sw_push(&sw_l, value, NULL);
    }

    size_t max_iterations = 1000;
    for (size_t iterations = 0; iterations < max_iterations; iterations++)
    {
        {
            int32_t value = get_random();
            sw_push(&sw_a, value, &dtime);
            delta_a_push += dtime;

            sw_pop(&sw_a, &value, &dtime);
            delta_a_pop += dtime;

            sw_push(&sw_l, value, &dtime);
            delta_l_push += dtime;

            sw_pop(&sw_l, &value, &dtime);
            delta_l_pop += dtime;
        }

        float percent = 100.0f * (iterations + 1) / max_iterations;
        print_status_percent("Статус тестирования: выполнено", percent);
    }

    printf("\n");
    printf("Полное время работы стека на массиве: " CLR_GREEN_U "%.3f мс" CLR_RESET "\n", (delta_a_push + delta_a_pop) / 1e6);
    printf("Полное время работы стека на списке:  " CLR_YELLOW_U "%.3f мс" CLR_RESET "\n", (delta_l_push + delta_l_pop) / 1e6);

    printf("\n");
    printf("Среднее время вставки в стек на массиве: " CLR_GREEN_U "%lu нс" CLR_RESET "\n", delta_a_push / max_iterations);
    printf("Среднее время вставки в стек на списке:  " CLR_YELLOW_U "%lu нс" CLR_RESET "\n", delta_l_push / max_iterations);

    printf("\n");
    printf("Среднее время удаления из стека на массиве: " CLR_GREEN_U "%lu нс" CLR_RESET "\n", delta_a_pop / max_iterations);
    printf("Среднее время удаления из стека на списке:  " CLR_YELLOW_U "%lu нс" CLR_RESET "\n", delta_l_pop / max_iterations);

    (void)upper_bound;

    sw_destroy(&sw_a);
    sw_destroy(&sw_l);
}

/*
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
            "Например " CLR_EMPH "\"test 10 40\"" CLR_RESET " протестирует работу со стеком от 10 до 40 элементов в нем.\n");
    else
    {
        int lower, upper;

        if (recieve_test_bounds(arglist, &lower, &upper) != EXIT_SUCCESS)
            printf("Введены некорректные аргументы.\n"
                "Воспользуйтесь командой " CLR_EMPH "\"help auto\"" CLR_RESET " для получения информации о команде.\n");
        else
            apply_test(lower, upper);
    }

    return EXIT_SUCCESS;
}
*/

int menu_auto(cmdf_arglist *arglist)
{
    (void)arglist;
    apply_test(10, 100);
    return EXIT_SUCCESS;

    // if (arglist)
    //     printf("Для данной команды не нужны агрументы.\n");
    // else
    // {
    //     cmdf_init(CLR_BR_CYAN_U "main/auto>" CLR_RESET " ", NULL, DOC_HEADER, UNDOC_HEADER, '~', 0);

    //     cmdf_register_command(auto_back, "back", CMD_BACK_HELP);
    //     cmdf_register_command(auto_test, "test", "");
    //     cmdf_commandloop();
    // }

    // return EXIT_SUCCESS;
}
