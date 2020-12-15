#ifndef __STR_STACK_H_
#define __STR_STACK_H_

typedef struct str_stack *str_stack_t;

str_stack_t sst_create();
void sst_insert(str_stack_t *stack, const char *str);
size_t sst_size(const str_stack_t *stack);

void sst_destroy(str_stack_t *stack);

#endif // __STR_STACK_H_
