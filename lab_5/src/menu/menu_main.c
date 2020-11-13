#include "utils/colors.h"
#include "menu/menu.h"

#define PROG_INTRO      "    " CLR_BR_GREEN_U "Лабораторная работа №5 по типам и структурам данных." CLR_RESET "\n" \
                        "\n" \
                        "Список доступных комманд:\n" \
                        "    config     Настроить временные параметры обслуживающего аппарата.\n" \
                        "    run        Запустить моделирование обслуживающего аппарата\n" \
                        "    exit       выход из программы\n\n" \
                        "Более подробно можно узнать о команде, набрав " CLR_EMPH "'help <название команды>'" CLR_RESET "."

#define CMD_CONFIG_HELP "Настроить временные параметры обслуживающего аппарата."

#define CMD_RUN_HELP    "Запустить моделирование обслуживающего аппарата."

worker_params_t wk_params;

static int __menu_exit(cmdf_arglist *arglist)
{
    printf("Выход из программы.\n");
    cmdf__default_do_exit(arglist);
    return EXIT_SUCCESS;
}

int menu_main(void)
{
    cmdf_init(CLR_BR_CYAN_U "main>" CLR_RESET " ", PROG_INTRO, DOC_HEADER, UNDOC_HEADER, '~', 0);

    /* Register our custom commands */
    cmdf_register_command(__menu_exit, "exit", CMD_EXIT_HELP);
    cmdf_register_command(menu_config, "config", CMD_CONFIG_HELP);
    cmdf_register_command(menu_run, "run", CMD_RUN_HELP);

    /* init global state */
    wk_params = wk_default_params();

    cmdf_commandloop();
    return EXIT_SUCCESS;
}
