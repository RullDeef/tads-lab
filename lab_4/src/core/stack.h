#ifndef __STACK_H_
#define __STACK_H_

#include "stack_arr.h"
#include "stack_lst.h"

typedef int __stack_imp_type;

#define STACK_TYPE_INVALID      0
#define STACK_TYPE_ARRAY        1
#define STACK_TYPE_LINKED_LIST  2

struct stack
{
    __stack_imp_type __type;
    union
    {
        struct stack_arr stack_arr;
        struct stack_lst stack_lst;
    } data;
};

struct stack st_create(__stack_imp_type type);
void st_destroy(struct stack *st);

int st_push(struct stack *st, int32_t value);
int st_pop(struct stack *st, int32_t *value);

uint32_t st_get_size(const struct stack *st);

int st_merge(struct stack *st_out, struct stack *st_a, struct stack *st_b);

#endif
