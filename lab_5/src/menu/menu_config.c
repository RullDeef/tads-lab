#include <stdio.h>
#include <assert.h>
#include "utils/colors.h"
#include "core/worker.h"
#include "menu/menu.h"

#define CONFIG_INTRO    "    " CLR_EMPH "Переход в режим настройки параметров обслуживающего аппарата." CLR_RESET "\n" \
                        "\n" \
                        "Список доступных комманд:\n" \
                        "    show       показать текущие параметры обслуживающего аппарата\n" \
                        "    set        установить конкретный параметр\n" \
                        "    back       выход из меню\n\n" \
                        "Более подробно можно узнать о команде, набрав " CLR_EMPH "'help <название команды>'" CLR_RESET "."

#define CMD_RESET_HELP   "Восстановить параметры по-умолчанию."
#define CMD_SET_HELP    "Установить временные рамки заданного параметра ОА."
#define CMD_MERGE_HELP  "Выполнить слияние двух стеков в третий."
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

static int parse_arglist(cmdf_arglist *arglist)
{
    (void)arglist;
    return EXIT_SUCCESS;
}

static int config_set(cmdf_arglist *arglist)
{
    if (!arglist || arglist->count != 3)
    {
        printf("Использование: " CLR_EMPH "set [имя параметра] [начало] [конец]" CLR_RESET ", где\n");
        printf("    имя параметра - одно из ...,\n");
        printf("    начало и конец - вещественные числа в единицах времени.\n");
    }
    else
    {
        int parse_result = parse_arglist(arglist);
        (void)parse_result;
    }

    return EXIT_SUCCESS;
}

static int config_show(cmdf_arglist *arglist)
{
    if (arglist)
        printf("Для данной команды не нужны дополнительные аргументы.\n");
    else
    {

    }
    
    return EXIT_SUCCESS;
}

static int config_back(cmdf_arglist *arglist)
{
    printf("\n    Выход из режима настройки.\n\n");
    cmdf__default_do_exit(arglist);
    return EXIT_SUCCESS;
}

int menu_config(cmdf_arglist *arglist)
{
    if (arglist)
        printf("Для данной команды не нужны дополнительные аргументы.\n");
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
