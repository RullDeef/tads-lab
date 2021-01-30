#include <string.h>
#include "utils/logger.h"
#include "str_stack/str_stack.h"

#define CALLOCS_REDEFINE
#include "callocs.h"

struct str_stack
{
    struct str_stack *next;
    const char *str;
};

str_stack_t sst_create()
{
    return NULL;
}

void sst_insert(str_stack_t *stack, const char *str)
{
    str_stack_t top = malloc(sizeof(struct str_stack) + strlen(str) + 1U);
    if (top == NULL)
        log_error("bad top in sst_insert. Abort?))");
    top->next = *stack;
    top->str = (char *)(top + 1);
    strcpy((char *)top->str, str);
    *stack = top;
}

size_t sst_size(const str_stack_t *stack)
{
    size_t size = 0UL;

    for (str_stack_t node = *stack; node != NULL; node = node->next)
        size++;

    return size;
}

void sst_destroy(str_stack_t *stack)
{
    while (*stack != NULL)
    {
        str_stack_t next = (*stack)->next;
        free(*stack);
        *stack = next;
    }
}
