
#include "StringStackLib.h"

#include <stdlib.h>

struct str_stack_t strStCreate()
{
    struct str_stack_t stack = {
        .__alloc_sz = 0,
        .__curr_sz = 0,
        .__data = NULL,
        .top = NULL
    };

    return stack;
}

void strStFree(struct str_stack_t *stack)
{
    if (stack == NULL || stack->__data == NULL)
        return;

    free(stack->__data);
    stack->__data = NULL;
    stack->top = NULL;
    stack->__alloc_sz = 0;
    stack->__curr_sz = 0;
}

void strStResize(struct str_stack_t *stack, size_t size)
{
    if (stack == NULL)
        return;
    
    int was_null = stack->__data == NULL;
    
    stack->__alloc_sz = size * sizeof(char);
    stack->__data = realloc(stack->__data, stack->__alloc_sz);
    
    if (was_null)
    {
        stack->top = stack->__data;
        stack->__curr_sz = 0;
    }
}

int strStPush(struct str_stack_t* stack, char value)
{
    if (stack == NULL || stack->__data == NULL)
        return -1;

    if (stack->__curr_sz == stack->__alloc_sz)
        return -2;

    *(stack->top) = value;
    stack->top++;
    stack->__curr_sz++;
    return 0;
}

int strStPop(struct str_stack_t* stack, char* value)
{
    if (stack == NULL || stack->__data == NULL)
        return -1;

    if (stack->__curr_sz == 0)
        return -2;

    stack->top--;
    stack->__curr_sz--;
    *value = *(stack->top);
    return 0;
}

int strStRead(struct str_stack_t* stack, char* value)
{
    if (stack == NULL || stack->__data == NULL)
        return -1;

    if (stack->__curr_sz == 0)
        return -2;

    *value = *(stack->top - 1);
    return 0;
}

// entry point for dll
int main(void)
{
    return 0;
}
