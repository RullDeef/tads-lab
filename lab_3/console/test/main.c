#include "../include/menu.h"
#include <stdio.h>
#include <locale.h>

int func(void *data)
{
    printf("opt!\n");
    return 0;
}

int exit_func(void *data)
{
    printf("exiting...\n");
    return 1;
}

int main(void)
{
    setlocale(LC_ALL, "");

    menu_t menu = create_menu(2,
        "main func!", func,
        "выход", exit_func
    );

    run_menu_loop(&menu, NULL);
    return 0;
}
