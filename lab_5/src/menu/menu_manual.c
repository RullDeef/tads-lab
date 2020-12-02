#include <stdio.h>
#include <assert.h>
#include "utils/colors.h"
#include "utils/timing.h"
#include "utils/str_parser.h"

#define __QDATA_DEFINED
typedef int qdata_t;
#include "core/queue/queue.h"

#include "menu/menu.h"

#define MANUAL_INTRO    "    " CLR_EMPH "Переход в ручной режим работы с очередями." CLR_RESET "\n" \
                        "\n" \
                        "Список доступных комманд:\n" \
                        "    show       показать очереди\n" \
                        "    push       добавить элементы в очереди\n" \
                        "    pop        удалить элемент из очередей\n" \
                        "    reset      очистить очереди\n" \
                        "    back       выход из меню\n\n" \
                        "Более подробно можно узнать о команде, набрав " CLR_EMPH "'help <название команды>'" CLR_RESET "."

#define CMD_RESET_HELP  "Привести обе очереди в исходное состояние."
#define CMD_PUSH_HELP   "Добавить элементы в очереди."
#define CMD_POP_HELP    "Удалить элемент из очередей."
#define CMD_SHOW_HELP   "Показать очереди."

#define INIT_QUEUE_CAPACITY 10

static struct queue_arr qu_arr;
static struct queue_lst qu_lst;

static int manual_push(cmdf_arglist *arglist)
{
    int data;
    if (arglist == NULL || arglist->count != 1 || parse_int32(arglist->args[0], &data) != EXIT_SUCCESS)
    {
        printf("Использование: " CLR_EMPH "push [число]" CLR_RESET ", где\n");
        printf("    число - целое число для вставки в очереди.\n");
    }
    else
    {
        if (qu_arr.size == INIT_QUEUE_CAPACITY)
            printf("Превышен максимальный размер очередей.\n");
        else
        {
            unsigned long long time_arr, time_lst;

            {
                BEGIN_TIMER;
                if (qu_arr.size < qu_arr.capacity)
                {
                    *(qu_arr.last) = data;
                    qu_arr.last++;

                    if (qu_arr.last == qu_arr.end)
                        qu_arr.last = qu_arr.begin;

                    qu_arr.size++;
                }
                END_TIMER;
                time_arr = TIMER_TICKS;
            }
            {
                BEGIN_TIMER;
                if (qu_lst.size == 0U)
                {
                    qu_lst.first = malloc(sizeof(struct __qu_lst_node));
                    qu_lst.first->next = NULL;
                    qu_lst.first->data = data;
                    qu_lst.last = &(qu_lst.first->next);
                    qu_lst.size++;
                }
                else
                {
                    *qu_lst.last = malloc(sizeof(struct __qu_lst_node));
                    (*qu_lst.last)->next = NULL;
                    (*qu_lst.last)->data = data;
                    qu_lst.last = &((*qu_lst.last)->next);
                    qu_lst.size++;
                }
                END_TIMER;
                time_lst = TIMER_TICKS;
            }

            printf("Добавлен элемент: %d\n", data);
            printf("Время очереди на массиве: %llu тактов\n", time_arr);
            printf("Время очереди на списке:  %llu тактов\n", time_lst);

            if (time_lst != 0U)
                printf("Эффективность по времени у массива: %.2f %%\n", (float)(100.0f * ((long double)time_lst - time_arr) / time_lst));
        }
    }

    return EXIT_SUCCESS;
}

static int manual_pop(cmdf_arglist *arglist)
{
    (void)arglist;
    if (qu_arr.size == 0U)
        printf("Очереди пусты. Нечего извлекать.\n");
    else
    {
        int data = 0;
        unsigned long long time_arr, time_lst;

        {
            BEGIN_TIMER;
            if (qu_arr.size > 0U)
            {
                data = *(qu_arr.first);
                qu_arr.first++;

                if (qu_arr.first == qu_arr.end)
                    qu_arr.first = qu_arr.begin;

                qu_arr.size--;
            }
            END_TIMER;
            time_arr = TIMER_TICKS;
        }
        {
            BEGIN_TIMER;
            if (qu_lst.size != 0)
            {
                struct __qu_lst_node *next = qu_lst.first->next;
                data = qu_lst.first->data;
                free(qu_lst.first);
                qu_lst.first = next;

                qu_lst.size--;

                if (qu_lst.size == 0U)
                    qu_lst.last = NULL;
            }
            END_TIMER;
            time_lst = TIMER_TICKS;
        }

        printf("Удален элемент: %d\n", data);
        printf("Время очереди на массиве: %llu тактов\n", time_arr);
        printf("Время очереди на списке:  %llu тактов\n", time_lst);

        if (time_lst != 0U)
            printf("Эффективность по времени у массива: %.2f %%\n", (float)(100.0f * ((long double)time_lst - time_arr) / time_lst));
    }

    return EXIT_SUCCESS;
}

static int manual_show(cmdf_arglist *arglist)
{
    (void)arglist;
    printf("\n  Очередь на массиве:\n");

    if (qu_arr.size == 0U)
        printf("Очередь пуста.\n");
    else
    {
        printf("Элементы очереди:");
        for (uint32_t i = 0; i < qu_arr.size; i++)
        {
            int data;
            if (qu_arr.first + i < qu_arr.end)
                data = qu_arr.first[i];
            else
                data = *((qu_arr.first + i) - (qu_arr.end - qu_arr.begin));
            printf("%s %d", (i == 0 ? "" : ","), data);
        }
        printf("\n");
    }

    printf("Объём занимаемой памяти: %lu байт.\n", sizeof(struct queue_arr) + qu_arr.capacity * sizeof(qdata_t));

    printf("\n  Очередь на списке:\n");

    if (qu_lst.size == 0U)
        printf("Очередь пуста.\n");
    else
    {
        printf("Элементы очереди:");
        for (struct __qu_lst_node *node = qu_lst.first; node != NULL; node = node->next)
            printf(" %d%s", node->data, (node->next == NULL ? "" : " ->"));
        printf("\n");
    }

    printf("Объём занимаемой памяти: %lu байт.\n", sizeof(struct queue_lst) + qu_lst.size * sizeof(struct __qu_lst_node));

    return EXIT_SUCCESS;
}

static int manual_reset(cmdf_arglist *arglist)
{
    (void)arglist;
    qua_destroy(&qu_arr);
    qul_destroy(&qu_lst);

    qu_arr = qua_create(INIT_QUEUE_CAPACITY);
    qu_lst = qul_create();

    printf("Очереди успешно очищены.\n");
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
    if (arglist)
        printf("Для данной команды не нужны дополнительные аргументы.\n");
    else
    {
        cmdf_init(CLR_BR_CYAN_U "main/manual>" CLR_RESET " ", MANUAL_INTRO, DOC_HEADER, UNDOC_HEADER, '~', 0);

        cmdf_register_command(manual_back, "back", CMD_BACK_HELP);
        cmdf_register_command(manual_show, "show", CMD_SHOW_HELP);
        cmdf_register_command(manual_reset, "reset", CMD_RESET_HELP);
        cmdf_register_command(manual_push, "push", CMD_PUSH_HELP);
        cmdf_register_command(manual_pop, "pop", CMD_POP_HELP);

        // setup queues
        qu_arr = qua_create(INIT_QUEUE_CAPACITY);
        qu_lst = qul_create();

        cmdf_commandloop();
    }

    return EXIT_SUCCESS;
}
