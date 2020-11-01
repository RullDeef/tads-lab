#include <stdio.h>
#include <assert.h>
#include "utils/timing.h"
#include "stack_wrapper.h"

struct stack_wrapper sw_wrap(const char *name, struct stack st)
{
    return (struct stack_wrapper){.name = name, .stack = st};
}

void sw_destroy(struct stack_wrapper *sw)
{
    st_destroy(&sw->stack);
}

int sw_push(struct stack_wrapper *sw, int32_t value, size_t *dtime)
{
    int result;

    BEGIN_TIMER
    result = st_push(&sw->stack, value);
    END_TIMER

    if (dtime != NULL)
        *dtime = (size_t)TIMER_TICKS;
    return result;
}

int sw_pop(struct stack_wrapper *sw, int32_t *value, size_t *dtime)
{
    int result;

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

    BEGIN_TIMER
    result = st_merge(out, a, b);
    END_TIMER

    if (dtime != NULL)
        *dtime = (size_t)TIMER_TICKS;
    return result;
}

static void __imp_show_elements_arr(struct stack_arr *sa)
{
    struct stack_arr tmp = st_arr_create();
    uint32_t size = sa->size;
    uint32_t capacity = sa->__capacity;
    int32_t value;

    const unsigned int __print_size = 3;
    const unsigned int __print_len = (3U + __print_size) * capacity - 1U;

    while (st_arr_pop(sa, &value) == EXIT_SUCCESS)
        st_arr_push(&tmp, value);

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
        for (; i < size && st_arr_pop(&tmp, &value) == EXIT_SUCCESS; i++)
        {
            st_arr_push(sa, value);
            printf(" %*d │", __print_size, value);
        }
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

    st_arr_destroy(&tmp);
}

static void __imp_show_elements_lst(struct stack_lst *sl)
{
    struct stack_lst tmp = st_lst_create();
    uint32_t size = sl->size;
    int32_t value;

    const unsigned int __print_size = 3;
    /* const unsigned int __print_len = (5U + __print_size) * size - 1U; */

    while (st_lst_pop(sl, &value) == EXIT_SUCCESS)
        st_lst_push(&tmp, value);

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
        for (uint32_t i = 0U; i < size && st_lst_pop(&tmp, &value) == EXIT_SUCCESS; i++)
        {
            st_lst_push(sl, value);
            printf("<-║ %*d │", __print_size, value);
        }
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

    st_lst_destroy(&tmp);
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
    printf("\n");
}
