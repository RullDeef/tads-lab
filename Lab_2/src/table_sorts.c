#include "sorters.h"
#include "flat_table.h"
#include <stdlib.h>

#include <stdint.h>

#ifdef _MSC_VER
#include <intrin.h>
#else
#include <x86intrin.h>
#endif

static int imp__flat_comp(void *flat_1, void *flat_2)
{
    return ((flat_t *)flat_2)->rooms_amount - ((flat_t *)flat_1)->rooms_amount;
}

static int imp__flat_key_comp(void *flat_1p, void *flat_2p)
{
    return ((flat_key_t *)flat_2p)->rooms_amount - ((flat_key_t *)flat_1p)->rooms_amount;
}

static uint32_t imp__apply_sort_method(flat_table_t *table, keys_table_t *keys, sort_params_t params, sort_method_fn_t method)
{
    keys = keys;
    uint32_t result = 0UL;

    if (!params.real_sort)
    {
        flat_table_t clone = ft_clone(table);
        unsigned long long __start_time = __rdtsc();
        method(clone.flats_array, clone.size, sizeof(flat_t), imp__flat_comp, assign_flat, params.ascending);
        result = __rdtsc() - __start_time;
        ft_free(&clone);
    }
    else
        method(table->flats_array, table->size, sizeof(flat_t), imp__flat_comp, assign_flat, params.ascending);

    return result;
}

static uint32_t imp__apply_sort_method_key(flat_table_t *table, keys_table_t *keys, sort_params_t params, sort_method_fn_t method)
{
    uint32_t result = 0UL;

    if (!params.real_sort)
    {
        keys_table_t keys_clone;
        keys_clone.size = table->size;
        keys_clone.keys = malloc(table->size * sizeof(flat_key_t));

        ft_gen_keys(table, &keys_clone);
        clock_t __start_time = __rdtsc();
        method(keys_clone.keys, keys_clone.size, sizeof(flat_key_t), imp__flat_key_comp, assign_flat_key, params.ascending);
        result += __rdtsc() - __start_time;
        free(keys_clone.keys);
    }
    else
        method(keys->keys, table->size, sizeof(flat_key_t), imp__flat_key_comp, assign_flat_key, params.ascending);

    return result;
}

uint32_t ft_sort_a_fast(flat_table_t *table, keys_table_t *keys, sort_params_t params)
{
    return imp__apply_sort_method(table, keys, params, merge_sort);
}

uint32_t ft_sort_a_slow(flat_table_t *table, keys_table_t *keys, sort_params_t params)
{
    return imp__apply_sort_method(table, keys, params, insertion_sort);
}

uint32_t ft_sort_b_fast(flat_table_t *table, keys_table_t *keys, sort_params_t params)
{
    return imp__apply_sort_method_key(table, keys, params, merge_sort);
}

uint32_t ft_sort_b_slow(flat_table_t *table, keys_table_t *keys, sort_params_t params)
{
    return imp__apply_sort_method_key(table, keys, params, insertion_sort);
}
