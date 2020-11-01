#include <stdio.h>
#include <stdlib.h>
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

int st_merge(struct stack *st_out, struct stack *st_a, struct stack *st_b)
{
#define BR(func) \
    {                               \
        int status = (func);        \
        if (status != EXIT_SUCCESS) \
            return status;          \
    }

    int32_t value;
    int32_t value_a;
    int32_t value_b;

    // clear out stack
    while (st_pop(st_out, &value) == EXIT_SUCCESS);

    uint32_t size_a = st_get_size(st_a);
    uint32_t size_b = st_get_size(st_b);

    uint32_t size_out = 0;

    while (size_a != 0 || size_b != 0)
    {
        // pick non empty stack
        if (size_a != 0 && size_b != 0)
        {
            BR(st_pop(st_a, &value_a));
            BR(st_pop(st_b, &value_b));

            if (size_out == 0)
            {
                // pick lower value and push in out
                if (value_a <= value_b)
                {
                    BR(st_push(st_out, value_a));
                    BR(st_push(st_b, value_b));
                    value = value_a;
                    size_a--;
                }
                else
                {
                    BR(st_push(st_out, value_b));
                    BR(st_push(st_a, value_a));
                    value = value_b;
                    size_b--;
                }
                size_out++;
            }
            else
            {
                // value contains last pushed value in out
                if (value_a <= value_b)
                {
                    uint32_t popped = 0;

                    BR(st_push(st_b, value_b));
                    while (size_out > 0 && value_a < value)
                    {
                        BR(st_pop(st_out, &value));
                        BR(st_push(st_b, value));
                        size_out--;
                        popped++;
                        if (size_out > 0) // lookup
                        {
                            BR(st_pop(st_out, &value));
                            BR(st_push(st_out, value));
                        }
                    }

                    BR(st_push(st_out, value_a));
                    size_out++;
                    size_a--;

                    while (popped > 0)
                    {
                        BR(st_pop(st_b, &value));
                        BR(st_push(st_out, value));
                        size_out++;
                        popped--;
                    }
                }
                else
                {
                    uint32_t popped = 0;

                    BR(st_push(st_a, value_a));
                    while (size_out > 0 && value_b < value)
                    {
                        BR(st_pop(st_out, &value));
                        BR(st_push(st_a, value));
                        size_out--;
                        popped++;
                        if (size_out > 0) // lookup
                        {
                            BR(st_pop(st_out, &value));
                            BR(st_push(st_out, value));
                        }
                    }

                    BR(st_push(st_out, value_b));
                    size_out++;
                    size_b--;

                    while (popped > 0)
                    {
                        BR(st_pop(st_a, &value));
                        BR(st_push(st_out, value));
                        size_out++;
                        popped--;
                    }
                }
            }
        }
        else if (size_a != 0)
        {
            uint32_t popped = 0;

            BR(st_pop(st_a, &value_a));
            while (size_out > 0 && value_a < value)
            {
                BR(st_pop(st_out, &value));
                BR(st_push(st_b, value));
                size_out--;
                popped++;
                if (size_out > 0) // lookup
                {
                    BR(st_pop(st_out, &value));
                    BR(st_push(st_out, value));
                }
            }

            BR(st_push(st_out, value_a));
            value = value_a;
            size_out++;
            size_a--;

            while (popped > 0)
            {
                BR(st_pop(st_b, &value));
                BR(st_push(st_out, value));
                size_out++;
                popped--;
            }
        }
        else
        {
            uint32_t popped = 0;
            
            BR(st_pop(st_b, &value_b));
            while (size_out > 0 && value_b < value)
            {
                BR(st_pop(st_out, &value));
                BR(st_push(st_a, value));
                size_out--;
                popped++;
                if (size_out > 0) // lookup
                {
                    BR(st_pop(st_out, &value));
                    BR(st_push(st_out, value));
                }
            }

            BR(st_push(st_out, value_b));
            value = value_b;
            size_out++;
            size_b--;

            while (popped > 0)
            {
                BR(st_pop(st_a, &value));
                BR(st_push(st_out, value));
                size_out++;
                popped--;
            }
        }
    }

    return EXIT_SUCCESS;
#undef BR
}
