#ifndef __ADDR_LIST_WRAPPER_H_
#define __ADDR_LIST_WRAPPER_H_

#include "core/stack.h"
#include "core/addr_list.h"

struct addr_list_wrapper
{
    struct addr_list list;
};

struct addr_list_wrapper alw_create(void);
void alw_destroy(struct addr_list_wrapper *alw);

void alw_handle_push(struct addr_list_wrapper *alw, struct stack *st);
void alw_handle_pop(struct addr_list_wrapper *alw, struct stack *st);

void alw_show(struct addr_list_wrapper *alw);

#endif
