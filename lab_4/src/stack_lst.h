#ifndef __STACK_LST_H_
#define __STACK_LST_H_

#include <stdint.h>

struct __st_lst_node
{
    int32_t data;
    struct __st_lst_node *prev;
};

struct stack_lst
{
    uint32_t size;
    struct __st_lst_node *top;
};

struct stack_lst sl_create(void);
void sl_destroy(struct stack_lst *sl);

int sl_push(struct stack_lst *sl, int32_t value);
int sl_pop(struct stack_lst *sl, int32_t *value);

#endif
