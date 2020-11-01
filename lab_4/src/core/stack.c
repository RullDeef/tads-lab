#include <assert.h>
#include "stack.h"
#include "utils/status_codes.h"

struct stack st_create(__stack_imp_type type)
{
    struct stack st = {.__type = type};

    if (type == STACK_TYPE_ARRAY)
        st.data.stack_arr = st_arr_create();
    else if (type == STACK_TYPE_LINKED_LIST)
        st.data.stack_lst = st_lst_create();
    else
        assert(0);

    return st;
}

void st_destroy(struct stack *st)
{
    if (st->__type == STACK_TYPE_ARRAY)
        st_arr_destroy(&st->data.stack_arr);
    else if (st->__type == STACK_TYPE_LINKED_LIST)
        st_lst_destroy(&st->data.stack_lst);
    else
        assert(0);
}

int st_push(struct stack *st, int32_t value)
{
    if (st->__type == STACK_TYPE_ARRAY)
        return st_arr_push(&st->data.stack_arr, value);
    else if (st->__type == STACK_TYPE_LINKED_LIST)
        return st_lst_push(&st->data.stack_lst, value);
    else
        return STACK_INVALID;
}

int st_pop(struct stack *st, int32_t *value)
{
    if (st->__type == STACK_TYPE_ARRAY)
        return st_arr_pop(&st->data.stack_arr, value);
    else if (st->__type == STACK_TYPE_LINKED_LIST)
        return st_lst_pop(&st->data.stack_lst, value);
    else
        return STACK_INVALID;
}

uint32_t st_get_size(const struct stack *st)
{
    if (st->__type == STACK_TYPE_ARRAY)
        return st->data.stack_arr.size;
    else if (st->__type == STACK_TYPE_LINKED_LIST)
        return st->data.stack_lst.size;
    else
        assert(0);
}