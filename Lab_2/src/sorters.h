#ifndef __SORTERS_H_
#define __SORTERS_H_

#include <ctype.h>
#include <stdbool.h>

typedef int (*__comp_fn_t)(void *, void *);
typedef void (*__assign_fn_t)(void *, void *);
typedef void (*__swap_fn_t)(void *, void *);
typedef void (*sort_method_fn_t)(void *, size_t, size_t, __comp_fn_t, __assign_fn_t);

void insertion_sort(void *a, size_t count, size_t size, __comp_fn_t comp, __swap_fn_t swap);
void merge_sort(void *a, size_t count, size_t size, __comp_fn_t comp, __assign_fn_t assign);


#endif
