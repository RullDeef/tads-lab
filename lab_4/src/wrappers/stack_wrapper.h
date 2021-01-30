#ifndef __STACK_WRAPPER_H_
#define __STACK_WRAPPER_H_

#include <stddef.h>
#include "core/stack.h"
#include "addr_list_wrapper.h"

struct stack_wrapper
{
    const char *name;
    struct stack stack;

    struct addr_list_wrapper alw;
};

struct stack_wrapper sw_wrap(const char *name, struct stack st);
void sw_destroy(struct stack_wrapper *sw);

int sw_push(struct stack_wrapper *sw, int32_t value, size_t *dtime);
int sw_pop(struct stack_wrapper *sw, int32_t *value, size_t *dtime);

int sw_merge(struct stack_wrapper *sw_out, struct stack_wrapper *sw_a, struct stack_wrapper *sw_b, size_t *dtime);
void sw_show(struct stack_wrapper *sw);

#endif
