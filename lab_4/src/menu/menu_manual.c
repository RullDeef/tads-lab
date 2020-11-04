#include <stdio.h>
#include <assert.h>
#include "utils/status_codes.h"
#include "utils/str_parser.h"
#include "wrappers/stack_wrapper.h"
#include "menu/menu.h"

#define CMD_SHOW_HELP   "Показать информацию о стеке."
#define CMD_PUSH_HELP   "Вставить число в стек. Первый аргумент - вставляемое число"
#define CMD_POP_HELP    "Извлечь число из вершины стека."
#define CMD_MERGE_HELP  "Выполнить слияние двух стеков в третий."

static struct stack_wrapper sw_a;
static struct stack_wrapper sw_b;

static struct stack_wrapper sw_c;

static struct stack_wrapper *get_stack_by_char(const char *str)
{
    if (strlen(str) != 1)
        return NULL;

    switch (str[0])
    {
        case 'A': case 'a': case '1': return &sw_a;
        case 'B': case 'b': case '2': return &sw_b;
        default: return NULL;
    }
}

static int manual_push(cmdf_arglist *arglist)
{
    if (!arglist)
        printf("Используйте параметры команды чтобы вставить число в стек.\n\n");
    else if (arglist->count == 1)
        printf("Введите букву стека и последовательность чисел для вставки через пробел.\n\n");
    else
    {
        struct stack_wrapper *sw = get_stack_by_char(arglist->args[0]);

        if (sw == NULL)
            printf("Первым аргументом необходимо указать букву стека.\nНапример A или B.\n\n");
        else if (!parse_all_int32(arglist->args + 1, arglist->count - 1))
            printf("Не все аргументы после буквы стека являются числами.\n\n");
        else
        {
            int32_t value = 0;
            for (uint32_t i = 1; i < arglist->count; i++)
            {
                if (parse_int32(arglist->args[i], &value) == EXIT_FAILURE)
                    assert(0);
                else
                    sw_push(sw, value, NULL);
            }
            printf("Введёные числа добавлены в стек '%s'.\n\n", sw->name);
        }
    }

    return EXIT_SUCCESS;
}

static int manual_pop(cmdf_arglist *arglist)
{
    if (!arglist)
        printf("Для данной команды нужен один параметр - буква стека.\n\n");
    else if (arglist->count != 1)
        printf("Кроме бувкы стека в качестве аргументов ничего не требуется.\n\n");
    else
    {
        struct stack_wrapper *sw = get_stack_by_char(arglist->args[0]);

        if (sw == NULL)
            printf("Первым аргументом необходимо указать букву стека.\nНапример A или B.\n\n");
        else
        {
            int32_t value = 0;
            if (sw_pop(sw, &value, NULL) == EXIT_SUCCESS)
                printf("Из стека '%s' извлечено число %d.\n\n", sw->name, value);
            else
                printf("стек '%s' пуст. Нечего извлекать.\n\n", sw->name);
        }
    }

    return EXIT_SUCCESS;
}

static int manual_show(cmdf_arglist *arglist)
{
    if (!arglist)
    {
        sw_show(&sw_a);
        sw_show(&sw_b);
    }
    else if (arglist->count > 1)
        printf("Кроме бувкы стека в качестве аргументов ничего не требуется.\n\n");
    else
    {
        struct stack_wrapper *sw = get_stack_by_char(arglist->args[0]);

        if (sw == NULL)
            printf("Единственным аргументом необходимо указать букву стека.\nНапример A или B.\n\n");
        else
            sw_show(sw);
    }
    
    return EXIT_SUCCESS;
}

static int manual_merge(cmdf_arglist *arglist)
{
    if (arglist)
        printf("Для этой команды не нужны аргументы.\n\n");
    else
    {
        size_t time;
        if (sw_merge(&sw_c, &sw_a, &sw_b, &time) != EXIT_SUCCESS)
            printf("Не удалось выполнить слияние стеков.\n\n");
        else
        {
            printf("Слияние стеков выполнено успешно! (время: %lu тиков)\n", time);
            sw_show(&sw_c);
        }
    }

    return EXIT_SUCCESS;
}

static int manual_back(cmdf_arglist *arglist)
{
    printf("\n    Выход из ручного режима.\n\n");
    cmdf__default_do_exit(arglist);
    return EXIT_SUCCESS;
}

static __stack_imp_type get_type(const char *arg)
{
    if (strcmp(arg, "A") == 0 || strcmp(arg, "a") == 0)
        return STACK_TYPE_ARRAY;
    else if (strcmp(arg, "L") == 0 || strcmp(arg, "l") == 0)
        return STACK_TYPE_LINKED_LIST;
    else
        return STACK_TYPE_INVALID;
}

static bool all_types_valid(char **argv)
{
    for (int i = 0; i < 3; i++)
        if (get_type(argv[i]) == STACK_TYPE_INVALID)
            return false;
    return true;
}

int menu_manual(cmdf_arglist *arglist)
{
    if (!arglist || arglist->count != 3 || !all_types_valid(arglist->args))
        printf("Введите \033[37;1;4m\"manual X X X\"\033[0m, где X - A или L - реализация стека.\n");
    else
    {
        printf("\n    Переход в режим ручного тестирования...");

        // init module variables
        sw_a = sw_wrap("\033[37;1;4mA\033[0m", st_create(get_type(arglist->args[0])));
        sw_b = sw_wrap("\033[37;1;4mB\033[0m", st_create(get_type(arglist->args[1])));
        sw_c = sw_wrap("\033[37;1;4mрезультат\033[0m", st_create(get_type(arglist->args[2])));

        cmdf_init("\033[96;1;4mmain/manual>\033[0m ", NULL, DOC_HEADER, UNDOC_HEADER, '~', 0);

        cmdf_register_command(manual_back, "back", CMD_BACK_HELP);
        cmdf_register_command(manual_push, "push", CMD_PUSH_HELP);
        cmdf_register_command(manual_pop, "pop", CMD_POP_HELP);
        cmdf_register_command(manual_show, "show", CMD_SHOW_HELP);
        cmdf_register_command(manual_merge, "merge", CMD_MERGE_HELP);

        cmdf_commandloop();

        // destroy module variables
        sw_destroy(&sw_a);
        sw_destroy(&sw_b);
        sw_destroy(&sw_c);
    }

    return EXIT_SUCCESS;
}
