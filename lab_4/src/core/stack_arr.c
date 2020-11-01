#include <stdlib.h>
#include <memory.h>
#include "utils/status_codes.h"
#include "core/stack_arr.h"

struct stack_arr st_arr_create(void)
{
    struct stack_arr sa = { .size = 0U };

    sa.__data = (int32_t *)malloc(SA_INIT_CAPACITY * sizeof(int32_t));
    sa.__capacity = sa.__data == NULL ? 0U : SA_INIT_CAPACITY;
    sa.top = sa.__data;

    return sa;
}

void st_arr_destroy(struct stack_arr *sa)
{
    if (sa->__data != NULL)
    {
        free(sa->__data);
        memset((void *)sa, 0, sizeof(struct stack_arr));
    }
}

int st_arr_push(struct stack_arr *sa, int32_t value)
{
    if (sa->__data == NULL || sa->top == NULL)
        return STACK_INVALID;
    
    if (sa->size + 1 > sa->__capacity) // resize
    {
        uint32_t new_capacity = (sa->size + 1) * SA_REALLOC_MULTIPLIER;
        int32_t *new_data = realloc(sa->__data, new_capacity * sizeof(int32_t));
        if (new_data == NULL)
        {
            // attempt to allocate exactly needed space
            new_capacity = sa->size + 1;
            new_data = realloc(sa->__data, new_capacity * sizeof(int32_t));
        }
        if (new_data == NULL)
            return STACK_OVERFLOW;
        
        sa->__capacity = new_capacity;
        sa->__data = new_data;
        sa->top = sa->__data + sa->size;
    }
    
    *(sa->top++) = value;
    sa->size++;
    return EXIT_SUCCESS;
}

int st_arr_pop(struct stack_arr *sa, int32_t *value)
{
    if (sa->size == 0)
        return STACK_EMPTY;
    else
    {
        *value = *--(sa->top);
        sa->size--;
        return EXIT_SUCCESS;
    }
}
