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

struct stack_arr sa_create(void);
void sa_destroy(struct stack_arr *sa);

int sa_push(struct stack_arr *sa, int32_t value);
int sa_pop(struct stack_arr *sa, int32_t *value);

#endif
