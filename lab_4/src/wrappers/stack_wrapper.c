#include <stdio.h>
#include <stdbool.h>
#include <assert.h>
#include "utils/timing.h"
#include "stack_wrapper.h"

struct stack_wrapper sw_wrap(const char *name, struct stack st)
{
    struct stack_wrapper sw = {.name = name, .stack = st };
    if (st.__type == STACK_TYPE_LINKED_LIST)
        sw.alw = alw_create();
    return sw;
}

void sw_destroy(struct stack_wrapper *sw)
{
    if (sw->stack.__type == STACK_TYPE_LINKED_LIST)
        alw_destroy(&sw->alw);
    st_destroy(&sw->stack);
}

int sw_push(struct stack_wrapper *sw, int32_t value, size_t *dtime)
{
    int result;

    BEGIN_TIMER
    result = st_push(&sw->stack, value);
    END_TIMER

    // update free_mem array
    if (sw->stack.__type == STACK_TYPE_LINKED_LIST)
        alw_handle_push(&sw->alw, &sw->stack);

    if (dtime != NULL)
        *dtime = (size_t)TIMER_TICKS;
    return result;
}

int sw_pop(struct stack_wrapper *sw, int32_t *value, size_t *dtime)
{
    int result;

    // update free_mem array
    if (sw->stack.__type == STACK_TYPE_LINKED_LIST)
        alw_handle_pop(&sw->alw, &sw->stack);

    BEGIN_TIMER
    result = st_pop(&sw->stack, value);
    END_TIMER

    if (dtime != NULL)
        *dtime = (size_t)TIMER_TICKS;
    return result;
}

int sw_merge(struct stack_wrapper *sw_out, struct stack_wrapper *sw_a, struct stack_wrapper *sw_b, size_t *dtime)
{
    int result;

    struct stack *out = &sw_out->stack;
    struct stack *a = &sw_a->stack;
    struct stack *b = &sw_b->stack;

    if (sw_a->stack.__type == STACK_TYPE_LINKED_LIST)
        alw_handle_clear(&sw_a->alw);

    if (sw_b->stack.__type == STACK_TYPE_LINKED_LIST)
        alw_handle_clear(&sw_b->alw);

    BEGIN_TIMER
    result = st_merge(out, a, b);
    END_TIMER

    if (dtime != NULL)
        *dtime = (size_t)TIMER_TICKS;
    return result;
}

static void __imp_show_elements_arr(struct stack_arr *sa)
{
    uint32_t size = sa->size;
    uint32_t capacity = sa->__capacity;

    const unsigned int __print_size = 3;
    const unsigned int __print_len = (3U + __print_size) * capacity - 1U;

    printf("Вместимость стека: %d\n", capacity);

    printf("╓");
    if (capacity > 0U)
        for (uint32_t i = 0U; i < __print_len; i++)
            printf("─");
    printf("┐\n");

    printf("║");
    if (capacity == 0U)
        printf("│");
    else
    {
        uint32_t i = 0U;
        for (; i < size; i++)
            printf(" %*d │", __print_size, sa->__data[i]);
        for (; i < capacity; i++)
        {
            printf(" ");
            for (unsigned int j = 0; j < __print_size; j++)
                printf("_");
            printf(" │");
        }
    }

    printf("\n");
    printf("╙");
    if (capacity > 0U)
        for (uint32_t i = 0U; i < __print_len; i++)
            printf("─");
    printf("┘\n");
}

static void __imp_show_elements_lst(struct stack_lst *sl)
{
    uint32_t size = sl->size;
    struct __st_lst_node *nodes = NULL;
    if (size > 0U)
    {
        nodes = malloc(size * sizeof(struct __st_lst_node));
        uint32_t i = size - 1;
        for (struct __st_lst_node *node = sl->top; node; node = node->prev)
            nodes[i--] = *node;
    }

    const unsigned int __print_size = 3;

    if (size == 0U)
    {
        printf("\n");
        printf("NULL\n");
        printf("\n");
    }
    else
    {
        printf("    ");
        for (uint32_t i = 0U; i < size; i++)
        {
            printf("  ╓");
            for (uint32_t j = 0U; j < __print_size + 2U; j++)
                printf("─");
            printf("┐");
        }
        printf("\n");

        printf("NULL");
        for (uint32_t i = 0U; i < size; i++)
            printf("<-║ %*d │", __print_size, nodes[i].data);
        printf("\n");

        printf("    ");
        for (uint32_t i = 0U; i < size; i++)
        {
            printf("  ╙");
            for (uint32_t j = 0U; j < __print_size + 2U; j++)
                printf("─");
            printf("┘");
        }
        printf("\n");
    }

    if (size > 0U)
        free(nodes);
}

static void __imp_show_elements(struct stack *st)
{
    if (st->__type == STACK_TYPE_ARRAY)
        __imp_show_elements_arr(&st->data.stack_arr);
    else if (st->__type == STACK_TYPE_LINKED_LIST)
        __imp_show_elements_lst(&st->data.stack_lst);
    else
        assert(0);
}

void sw_show(struct stack_wrapper *sw)
{
    printf("стек '%s'.\n", sw->name);
    printf("Кол-во элементов в стеке: %u\n", st_get_size(&sw->stack));
    __imp_show_elements(&sw->stack);

    if (sw->stack.__type == STACK_TYPE_LINKED_LIST)
        alw_show(&sw->alw);

    printf("\n");
}
