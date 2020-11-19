#ifndef __STACK_LST_H_
#define __STACK_LST_H_

#include <stdint.h>

#define SL_MAX_ADDR_OFFSET 10U

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

struct stack_lst st_lst_create(void);
void st_lst_destroy(struct stack_lst *sl);

int st_lst_push(struct stack_lst *sl, int32_t value);
int st_lst_pop(struct stack_lst *sl, int32_t *value);

#endif
