#include <stdio.h>
#include "utils/status_codes.h"
#include "utils/str_parser.h"
#include "wrappers/stack_wrapper.h"
#include "menu/menu.h"

#define CMD_SHOW_HELP   "Показать информацию о стэке."
#define CMD_PUSH_HELP   "Вставить число в стэк. Первый аргумент - вставляемое число"
#define CMD_POP_HELP    "Извлечь число из вершины стэка."

static struct stack_wrapper sw;


static int manual_push(cmdf_arglist *arglist)
{
    if (!arglist)
        printf("Используйте параметры команды чтобы вставить число в стэк.\n\n");
    else if (arglist->count != 1)
        printf("Введите только одно число для вставки.\n\n");
    else
    {
        int32_t value = 0;
        if (parse_int32(arglist->args[0], &value) == EXIT_FAILURE)
            printf("Введено неверное число.\n\n");
        else
        {
            sw_push(&sw, value, NULL);
            printf("В стэк добавлено число %d.\n\n", value);
        }
    }

    return EXIT_SUCCESS;
}

static int manual_pop(cmdf_arglist *arglist)
{
    if (arglist)
        printf("Для данной команды не нужны параметры.\n\n");
    else
    {
        int32_t value = 0;
        if (sw_pop(&sw, &value, NULL) == EXIT_SUCCESS)
            printf("Извлечено число %d из стэка.\n\n", value);
        else
            printf("Стэк пуст. Нечего извлекать.\n\n");
    }

    return EXIT_SUCCESS;
}

static int manual_show(cmdf_arglist *arglist)
{
    (void) arglist;
    sw_show(&sw);
    
    return EXIT_SUCCESS;
}

static int manual_back(cmdf_arglist *arglist)
{
    printf("\n    Выход из ручного режима.\n\n");
    cmdf__default_do_exit(arglist);
    return EXIT_SUCCESS;
}

int menu_manual(cmdf_arglist *arglist)
{
    arglist = arglist;
    printf("\n    Переход в режим ручного тестирования...");

    // init module variables
    extern struct stack_wrapper sw;
    sw = sw_wrap("на массиве", st_create(STACK_TYPE_LINKED_LIST));

    cmdf_init("main/manual> ", NULL, DOC_HEADER, UNDOC_HEADER, '~', 0);

    cmdf_register_command(manual_back, "back", CMD_BACK_HELP);
    cmdf_register_command(manual_push, "push", CMD_PUSH_HELP);
    cmdf_register_command(manual_pop, "pop", CMD_POP_HELP);
    cmdf_register_command(manual_show, "show", CMD_SHOW_HELP);

    cmdf_commandloop();

    // destroy module variables
    sw_destroy(&sw);

    return EXIT_SUCCESS;
}
