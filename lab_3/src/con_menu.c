#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <ctype.h>
#include <wchar.h>
#include "con_menu.h"

#define TEMP_LINE_LEN 128

static size_t imp__get_max_opt_name(menu_t *menu)
{
    size_t len = 0;

    for (size_t i = 0; i < menu->opt_count; i++)
    {
        
        size_t new_len = strlen(menu->opts[i].name);
        if (new_len > len)
            len = new_len;
    }

    return len;
}

static void imp__print_menu(menu_t *menu)
{
    size_t len = imp__get_max_opt_name(menu);

    printf("╒═");
    for (size_t i = 0; i < len + 3; i++)
        printf("═");
    printf("═╕\n");

    printf("│ Меню %*s │\n", len - 2, " ");
    printf("├");
    for (size_t i = 0; i < len + 5; i++)
        printf("─");
    printf("┘\n");

    for (size_t i = 0; i < menu->opt_count; i++)
        printf("│ %2d %s\n", i + 1, menu->opts[i].name);

    printf("┕━");
    for (size_t i = 0; i < len + 3; i++)
        printf("━");
    printf("━\n");
}

static int imp__get_menu_opt()
{
    printf("\nВыберите опцию меню: ");

    char line[TEMP_LINE_LEN];
    if (fgets(line, TEMP_LINE_LEN, stdin) == NULL)
        return -1;

    char *end = NULL;
    long opt = strtoul(line, &end, 10);

    if (opt == 0)
        return -2;

    while (*end != '\0' && isspace(*end))
        end++;

    if (*end != '\0')
        return -3;

    if (opt > MAX_MENU_OPT_COUNT)
        return -4;

    return (int)opt - 1;
}

menu_t create_menu(size_t opts_count, ...)
{
    menu_t menu = {.opt_count = opts_count};

    va_list va;
    va_start(va, opts_count);

    for (size_t i = 0; i < opts_count; i++)
    {
        strcpy(menu.opts[i].name, va_arg(va, char*));
        menu.opts[i].func = va_arg(va, menu_opt_fn_t);
    }

    va_end(va);
    return menu;
}

void run_menu_loop(menu_t *menu, void *data)
{
    bool need_exit = false;

    while (!need_exit)
    {
        imp__print_menu(menu);
        int opt = imp__get_menu_opt();

        if (opt >= 0)
            if (menu->opts[opt].func(data) != 0)
                need_exit = true;
    }
}
