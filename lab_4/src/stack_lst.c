#include <stdlib.h>
#include "status_codes.h"
#include "stack_lst.h"

struct stack_lst sl_create(void)
{
    struct stack_lst lst = { .size = 0U, .top = NULL };
    return lst;
}

void sl_destroy(struct stack_lst *sl)
{
    while (sl->top != NULL)
    {
        struct __st_lst_node *prev = sl->top->prev;
        free(sl->top);
        sl->top = prev;
    }
    sl->size = 0U;
}

int sl_push(struct stack_lst *sl, int32_t value)
{
    struct __st_lst_node *new_top = malloc(sizeof(struct __st_lst_node));
    if (new_top == NULL)
        return STACK_OVERFLOW;
    new_top->data = value;
    new_top->prev = sl->top;
    sl->top = new_top;
    sl->size++;
    return EXIT_SUCCESS;
}

int sl_pop(struct stack_lst *sl, int32_t *value)
{
    if (sl->top == NULL)
        return STACK_EMPTY;
    *value = sl->top->data;
    struct __st_lst_node *prev = sl->top->prev;
    free(sl->top);
    sl->top = prev;
    sl->size--;
    return EXIT_SUCCESS;
}
