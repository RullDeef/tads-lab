#include "menu/menu.h"

#define PROG_INTRO      "    \033[32;1;4mЛабораторная работа №4 по типам и структурам данных.\033[0m\n\n" \
                        "Список доступных комманд:\n" \
                        "    manual     ручной режим\n" \
                        "    exit       выйти из программы\n\n" \
                        "Более подробно можно узнать о команде, набрав 'help <название команды>'."

#define CMD_MANUAL_HELP "Перейти в режим тестирования стека в ручную " \
                        "с возможностью просматривать содержимое стека."


static int __menu_exit(cmdf_arglist *arglist)
{
    printf("Выход из программы.\n");
    cmdf__default_do_exit(arglist);
    return EXIT_SUCCESS;
}

int menu_main(void)
{
    cmdf_init("\033[96;1;4mmain>\033[0m ", PROG_INTRO, DOC_HEADER, UNDOC_HEADER, '~', 0);

    /* Register our custom commands */
    cmdf_register_command(__menu_exit, "exit", CMD_EXIT_HELP);
    cmdf_register_command(menu_manual, "manual", CMD_MANUAL_HELP);

    cmdf_commandloop();
    return EXIT_SUCCESS;
}
