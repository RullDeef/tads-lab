#define MASURE_RDTSC 0

#include <stdlib.h>
#include "sorters.h"
#include "flat_table.h"
#include "table_sorts.h"

#include <stdint.h>

#if MASURE_RDTSC == 1
#   ifdef _MSC_VER
#       include <intrin.h>
#   else
#       include <x86intrin.h>
#   endif
#else
#   include <sys/time.h>
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

#if MASURE_RDTSC == 1                                                               // Замеры времени двумя способами:
        unsigned long long __start_time = __rdtsc();                                //
#else                                                                               // __rdtsc();
        struct timeval tv1, tv2;                                                    //      и
        gettimeofday(&tv1, NULL);                                                   // gettimeofday(...);
#endif                                                                              //

        method(clone.flats_array, clone.size, sizeof(flat_t), imp__flat_comp, assign_flat);

#if MASURE_RDTSC == 1                                                               // Замеры времени двумя способами:
        result = __rdtsc() - __start_time;                                          //
#else                                                                               // __rdtsc();
        gettimeofday(&tv2, NULL);                                                   //      и
        result = 1000000L * (tv2.tv_sec - tv1.tv_sec) + tv2.tv_usec - tv1.tv_usec;  // gettimeofday(...);
#endif                                                                              //

        ft_free(&clone);
    }
    else
        method(table->flats_array, table->size, sizeof(flat_t), imp__flat_comp, assign_flat);

    return result;
}

static uint32_t imp__apply_sort_method_key(flat_table_t *table, keys_table_t *keys, sort_params_t params, sort_method_fn_t method)
{
    uint32_t result = 0UL;

    if (!params.real_sort)
    {
        keys_table_t keys_clone = ts_keys_create(table);

#if MASURE_RDTSC == 1                                                               // Замеры времени двумя способами:
        unsigned long long __start_time = __rdtsc();                                //
#else                                                                               // __rdtsc();
        struct timeval tv1, tv2;                                                    //      и
        gettimeofday(&tv1, NULL);                                                   // gettimeofday(...);
#endif                                                                              //

        method(keys_clone.keys, keys_clone.size, sizeof(flat_key_t), imp__flat_key_comp, assign_flat_key);

#if MASURE_RDTSC == 1                                                               // Замеры времени двумя способами:
        result = __rdtsc() - __start_time;                                          //
#else                                                                               // __rdtsc();
        gettimeofday(&tv2, NULL);                                                   //      и
        result = 1000000L * (tv2.tv_sec - tv1.tv_sec) + tv2.tv_usec - tv1.tv_usec;  // gettimeofday(...);
#endif                                                                              //

        ts_keys_free(&keys_clone);
    }
    else
        method(keys->keys, table->size, sizeof(flat_key_t), imp__flat_key_comp, assign_flat_key);

    return result;
}

keys_table_t ts_keys_create(flat_table_t *table)
{
    keys_table_t keys;
    keys.size = table->size;
    keys.keys = malloc(keys.size * sizeof(flat_key_t));

    for (uint32_t i = 0; i < table->size; i++)
    {
        keys.keys[i].id = i;
        keys.keys[i].rooms_amount = table->flats_array[i].rooms_amount;
    }

    return keys;
}

void ts_keys_free(keys_table_t *keys)
{
    free(keys->keys);
    keys->keys = NULL;
    keys->size = 0;
}

uint32_t ts_sort_a_fast(flat_table_t *table, keys_table_t *keys, sort_params_t params)
{
    return imp__apply_sort_method(table, keys, params, merge_sort);
}

uint32_t ts_sort_a_slow(flat_table_t *table, keys_table_t *keys, sort_params_t params)
{
    return imp__apply_sort_method(table, keys, params, insertion_sort);
}

uint32_t ts_sort_b_fast(flat_table_t *table, keys_table_t *keys, sort_params_t params)
{
    return imp__apply_sort_method_key(table, keys, params, merge_sort);
}

uint32_t ts_sort_b_slow(flat_table_t *table, keys_table_t *keys, sort_params_t params)
{
    return imp__apply_sort_method_key(table, keys, params, insertion_sort);
}
