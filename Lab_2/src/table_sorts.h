#ifndef __TABLE_SORTS_H_
#define __TABLE_SORTS_H_

#include "flat_table.h"

typedef struct
{
    uint32_t size;
    flat_key_t *keys;
} keys_table_t;

typedef struct
{
    bool ascending;
    bool real_sort;
} sort_params_t;

typedef uint32_t (*sort_fn_t)(flat_table_t *, keys_table_t *, sort_params_t);

keys_table_t ts_keys_create(flat_table_t *table);
void ts_keys_free(keys_table_t *keys);

uint32_t ts_sort_a_fast(flat_table_t *table, keys_table_t *keys, sort_params_t params);
uint32_t ts_sort_a_slow(flat_table_t *table, keys_table_t *keys, sort_params_t params);
uint32_t ts_sort_b_fast(flat_table_t *table, keys_table_t *keys, sort_params_t params);
uint32_t ts_sort_b_slow(flat_table_t *table, keys_table_t *keys, sort_params_t params);

#endif // __TABLE_SORTS_H_
