#ifndef __STACK_ARR_H_
#define __STACK_ARR_H_

#include <stdint.h>

#define SA_INIT_CAPACITY 10U
#define SA_REALLOC_MULTIPLIER 1.2

struct stack_arr
{
    uint32_t size;
    uint32_t __capacity;
    int32_t *__data;
    int32_t *top;
};

struct stack_arr st_arr_create(void);
void st_arr_destroy(struct stack_arr *sa);

int st_arr_push(struct stack_arr *sa, int32_t value);
int st_arr_pop(struct stack_arr *sa, int32_t *value);

#endif
