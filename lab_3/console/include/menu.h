#ifndef __MENU_H_
#define __MENU_H_

#include <sys/types.h>

#define MAX_MENU_OPT_NAME_LEN 128
#define MAX_MENU_OPT_COUNT 10

typedef int (*menu_opt_fn_t)(void *data);

typedef struct
{
    char name[MAX_MENU_OPT_NAME_LEN];
    menu_opt_fn_t func;
} menu_opt_t;

typedef struct
{
    size_t opt_count;
    menu_opt_t opts[MAX_MENU_OPT_COUNT];
} menu_t;

menu_t create_menu(size_t opts_count, ...);
void run_menu_loop(menu_t *menu, void *data);

#endif
