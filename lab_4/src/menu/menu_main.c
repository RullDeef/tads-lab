#include "utils/colors.h"
#include "menu/menu.h"

#define PROG_INTRO      "    " CLR_BR_GREEN_U "Лабораторная работа №4 по типам и структурам данных." CLR_RESET "\n" \
                        "\n" \
                        "Список доступных комманд:\n" \
                        "    manual     ручной режим\n" \
                        "    auto       автоматический режим\n" \
                        "    exit       выход из программы\n\n" \
                        "Более подробно можно узнать о команде, набрав " CLR_EMPH "'help <название команды>'" CLR_RESET "."

#define CMD_MANUAL_HELP "Перейти в режим тестирования стека в ручную " \
                        "с возможностью просматривать содержимое стека."

#define CMD_AUTO_HELP   "Перейти в режим автоматического тестирования."


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
    cmdf_register_command(menu_manual, "manual", CMD_MANUAL_HELP);
    cmdf_register_command(menu_auto, "auto", CMD_AUTO_HELP);

    cmdf_commandloop();
    return EXIT_SUCCESS;
}
