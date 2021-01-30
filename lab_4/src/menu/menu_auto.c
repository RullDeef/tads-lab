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

static void apply_test(void)
{
    struct stack_wrapper sw_a = sw_wrap(CLR_EMPH "на массиве" CLR_RESET, st_create(STACK_TYPE_ARRAY));
    struct stack_wrapper sw_l = sw_wrap(CLR_EMPH "на списке" CLR_RESET, st_create(STACK_TYPE_LINKED_LIST));

    size_t delta_a_push = 0;
    size_t delta_l_push = 0;
    size_t delta_a_pop = 0;
    size_t delta_l_pop = 0;
    size_t dtime;

    size_t iters = 3000;
    for (size_t iter = 0; iter < iters; iter++)
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

        float percent = 100.0f * (iter + 1) / iters;
        print_status_percent("Статус тестирования: выполнено", percent);
    }

    printf("\nВыполнено " CLR_CYAN_U "%lu" CLR_RESET " тестовых вставок и удалений из стеков.\n", iters);
    printf("\n");

    printf("Полное время работы стека на массиве: " CLR_GREEN_U "%.3f мс" CLR_RESET "\n", (delta_a_push + delta_a_pop) / 1e3);
    printf("Полное время работы стека на списке:  " CLR_YELLOW_U "%.3f мс" CLR_RESET "\n", (delta_l_push + delta_l_pop) / 1e3);
    printf("\n");

    printf("Среднее время вставки в стек на массиве: " CLR_GREEN_U "%lu мкс" CLR_RESET "\n", delta_a_push / iters);
    printf("Среднее время вставки в стек на списке:  " CLR_YELLOW_U "%lu мкс" CLR_RESET "\n", delta_l_push / iters);
    printf("\n");

    printf("Среднее время удаления из стека на массиве: " CLR_GREEN_U "%lu мкс" CLR_RESET "\n", delta_a_pop / iters);
    printf("Среднее время удаления из стека на списке:  " CLR_YELLOW_U "%lu мкс" CLR_RESET "\n", delta_l_pop / iters);
    printf("\n");

    printf("Объём памяти, занимаемый под один элемент в стеке на массиве: " CLR_GREEN_U "%lu байт" CLR_RESET "\n", sizeof(int32_t));
    printf("Объём памяти, занимаемый под один элемент в стеке на списке:  " CLR_YELLOW_U "%lu байт" CLR_RESET "\n", sizeof(struct __st_lst_node));

    sw_destroy(&sw_a);
    sw_destroy(&sw_l);
}

int menu_auto(cmdf_arglist *arglist)
{
    if (arglist)
        printf("Для данной команды не нужны дополнительные аргументы.\n");
    else
        apply_test();

    return EXIT_SUCCESS;
}
