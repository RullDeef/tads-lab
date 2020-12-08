#include <stdio.h>
#include <assert.h>
#include "utils/colors.h"
#include "utils/str_parser.h"
#include "core/worker.h"
#include "menu/menu.h"

#define CONFIG_INTRO    "    " CLR_EMPH "Переход в режим настройки параметров обслуживающего аппарата." CLR_RESET "\n" \
                        "\n" \
                        "Список доступных комманд:\n" \
                        "    show       показать текущие параметры обслуживающего аппарата\n" \
                        "    set        установить конкретный параметр\n" \
                        "    reset      восстановить параметры по-умолчанию\n" \
                        "    back       выход из меню\n\n" \
                        "Более подробно можно узнать о команде, набрав " CLR_EMPH "'help <название команды>'" CLR_RESET "."

#define CMD_RESET_HELP   "Восстановить параметры по-умолчанию."
#define CMD_SET_HELP    "Установить временные рамки заданного параметра ОА."
#define CMD_SHOW_HELP   "Показать информацию о параметрах ОА."

static int config_reset(cmdf_arglist *arglist)
{
    if (arglist)
        printf("Для данной команды не нужны дополнительные аргументы.\n");
    else
    {
        wk_params = wk_default_params();
        printf("Параметры обслуживающего аппарата установлены по-умолчанию.\n");
    }

    return EXIT_SUCCESS;
}

static int parse_offset(const char *str, size_t *offset)
{
    if (strcmp(str, "a") == 0 || strcmp(str, "A") == 0)
        *offset = 0;
    else if (strcmp(str, "b") == 0 || strcmp(str, "B") == 0)
        *offset = 1;
    else if (strcmp(str, "c") == 0 || strcmp(str, "C") == 0)
        *offset = 2;
    else if (strcmp(str, "d") == 0 || strcmp(str, "D") == 0)
        *offset = 3;
    else
        return EXIT_FAILURE;

    return EXIT_SUCCESS;
}

static int config_set(cmdf_arglist *arglist)
{
    size_t offset;
    float begin;
    float end;

    if (!arglist || arglist->count != 3 || parse_offset(arglist->args[0], &offset) ||
        parse_float(arglist->args[1], &begin) || parse_float(arglist->args[2], &end))
    {
        printf("Использование: " CLR_EMPH "set [имя параметра] [начало] [конец]" CLR_RESET ", где\n");
        printf("    имя параметра - одно из " CLR_BR_GREEN_U "a" CLR_RESET ", " CLR_BR_GREEN_U "b" CLR_RESET ", " CLR_BR_GREEN_U "c" CLR_RESET " или " CLR_BR_GREEN_U "d" CLR_RESET ",\n");
        printf("    начало и конец - вещественные числа в единицах времени.\n");
    }
    else if (begin < 0.0f || end < 0.0f)
        printf("Значения времени не могут быть отрицательными\n");
    else if (begin > end)
        printf("Начальное значение времени не может быть больше конечного.\n");
    else
    {
        time_interval_t *intv = &(wk_params.t_in1) + offset;
        intv->begin = begin;
        intv->end = end;
    }

    return EXIT_SUCCESS;
}

static int config_show(cmdf_arglist *arglist)
{
    if (arglist)
        printf("Для данной команды не нужны дополнительные аргументы.\n");
    else
    {
        printf("\n  " CLR_BR_YELLOW_U "Параметры моделирования:" CLR_RESET "\n");
        printf("    [a] " CLR_BR_CYAN_U "%g..%g" CLR_RESET " е.в. - время прихода заявки в первую очередь.\n",
            wk_params.t_in1.begin, wk_params.t_in1.end);
        printf("    [b] " CLR_BR_CYAN_U "%g..%g" CLR_RESET " е.в. - время прихода заявки во вторую очередь.\n",
            wk_params.t_in2.begin, wk_params.t_in2.end);
        printf("    [с] " CLR_BR_CYAN_U "%g..%g" CLR_RESET " е.в. - время обработки заявки из первой очереди.\n",
            wk_params.t_out1.begin, wk_params.t_out1.end);
        printf("    [d] " CLR_BR_CYAN_U "%g..%g" CLR_RESET " е.в. - время обработки заявки из второй очереди.\n\n",
            wk_params.t_out2.begin, wk_params.t_out2.end);
    }
    
    return EXIT_SUCCESS;
}

static int config_back(cmdf_arglist *arglist)
{
    printf("\n    Выход из режима настройки.\n\n");
    cmdf__default_do_exit(arglist);
    return EXIT_SUCCESS;
}

static void scanf_intv(time_interval_t *intv)
{
    scanf("%f%f", &intv->begin, &intv->end);
    char buf[2];
    fgets(buf, 2, stdin);
}

int menu_config(cmdf_arglist *arglist)
{
    if (arglist && arglist->count == 1 && strcmp(arglist->args[0], "fast") == 0)
    {
        printf("Время прихода заявки в первую очередь: ");
        scanf_intv(&wk_params.t_in1);

        printf("Время прихода заявки во вторую очередь: ");
        scanf_intv(&wk_params.t_in2);

        printf("Время обработки заявки из первой очереди: ");
        scanf_intv(&wk_params.t_out1);

        printf("Время обработки заявки из второй очереди: ");
        scanf_intv(&wk_params.t_out2);

        printf("\n");
        config_show(NULL);
    }
    else
    {
        cmdf_init(CLR_BR_CYAN_U "main/config>" CLR_RESET " ", CONFIG_INTRO, DOC_HEADER, UNDOC_HEADER, '~', 0);

        cmdf_register_command(config_back, "back", CMD_BACK_HELP);
        cmdf_register_command(config_reset, "reset", CMD_RESET_HELP);
        cmdf_register_command(config_set, "set", CMD_SET_HELP);
        cmdf_register_command(config_show, "show", CMD_SHOW_HELP);

        cmdf_commandloop();
    }

    return EXIT_SUCCESS;
}
