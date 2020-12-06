#ifndef __HT_WRAPPER_H_
#define __HT_WRAPPER_H_

#include <stdio.h>
#include "core/hash_table/hash_table.h"

struct ht_wrapper
{
    struct hash_table table;
};

struct ht_wrapper htw_create(unsigned int size, unsigned int step, hash_func_t func);
void htw_destroy(struct ht_wrapper *htw);

int htw_insert(struct ht_wrapper *htw, int key);

int htw_find(struct ht_wrapper *htw, int key);

int htw_fscanf(FILE *file, struct ht_wrapper *htw);
void htw_fprintf(FILE *file, struct ht_wrapper *htw);

size_t htw_calc_size(struct ht_wrapper *htw);

#endif // __HT_WRAPPER_H_
