#include <stdio.h>

#define CMDF_TAB_TO_SPACES 4
#define CMD_READLINE_SUPPORT
#define CMDF_MAX_COMMANDS 8
#define CMDF_MAX_SUBPROCESSES 3
#define LIBCMDF_IMPL
#include "../vendor/libcmdf/include/libcmdf.h"

#define PROG_INTRO      "Лабораторная работа №4 по типам и структурам данных.\n\n" \
                        "Список доступных комманд:\n" \
                        "    manual     ручной режим\n" \
                        "    exit       выйти из программы\n\n" \
                        "Более подробно можно узнать о команде, набрав 'help <название команды>'."

#define DOC_HEADER      "Документированные команды:"
#define UNDOC_HEADER    "Недокументированные команды:"

#define CMD_MANUAL_HELP "Перейти в режим тестирования стэка в ручную " \
                        "с возможностью просматривать содержимое стека."
#define CMD_EXIT_HELP   "Выйти из программы."

#define CMD_MAN_BACK_HELP   "Перейти в главное меню."

static int manual_back(cmdf_arglist *arglist)
{
    printf("\n    Выход из ручного режима.\n\n");
    cmdf__default_do_exit(arglist);
    return EXIT_SUCCESS;
}

static int menu_manual(cmdf_arglist *arglist)
{
    arglist = arglist;
    printf("\n    Переход в режим ручного тестирования...\n");

    cmdf_init("main/manual> ", NULL, DOC_HEADER, UNDOC_HEADER, '~', 0);

    cmdf_register_command(manual_back, "back", CMD_MAN_BACK_HELP);

    cmdf_commandloop();
    return EXIT_SUCCESS;
}

static int menu_exit(cmdf_arglist *arglist)
{
    printf("Выход из программы.\n");
    cmdf__default_do_exit(arglist);
    return EXIT_SUCCESS;
}

int main(void)
{
    cmdf_init("main> ", PROG_INTRO, DOC_HEADER, UNDOC_HEADER, '~', 0);

    /* Register our custom commands */
    cmdf_register_command(menu_exit, "exit", CMD_EXIT_HELP);
    cmdf_register_command(menu_manual, "manual", CMD_MANUAL_HELP);

    cmdf_commandloop();
    return EXIT_SUCCESS;
}
