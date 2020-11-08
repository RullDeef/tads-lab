#include "menu/menu.h"

static int auto_back(cmdf_arglist *arglist)
{
    printf("\n    Выход из автоматизированного режима.\n\n");
    cmdf__default_do_exit(arglist);
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
        cmdf_commandloop();
    }

    return EXIT_SUCCESS;
}
