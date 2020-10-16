#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "flat_table.h"
#include <time.h>

#include <stdint.h>

#ifdef _MSC_VER
#include <intrin.h>
#else
#include <x86intrin.h>
#endif

#define FLAT_STRING_SIZE 256
#define PERF_TIMES 100

typedef int (*__comp_fn_t)(void *, void *);
typedef void (*__assign_fn_t)(void *, void *);
typedef void (*__swap_fn_t)(void *, void *);
typedef void (*sort_method_fn_t)(void *, size_t, size_t, __comp_fn_t, __assign_fn_t, bool);

int imp__fget_line(char *str, int size, FILE *file)
{
    assert(str != NULL);
    assert(size > 0);
    assert(file != NULL);

    if (fgets(str, size, file) != str)
        return -1; // bad file

    // remove trailing \n & \r
    while (str[strlen(str) - 1] == '\n' || str[strlen(str) - 1] == '\r')
        str[strlen(str) - 1] = '\0';

    return 0;
}

unsigned int imp__count_flats_amount(FILE *file)
{
    assert(file != NULL);

    unsigned int amount = 0;

    rewind(file);
    for (char chr; (chr = fgetc(file)) != '\0' && chr != EOF;)
        if (chr == '\n')
            amount++;

    rewind(file);
    return amount;
}

int imp__read_flat_table_from_file(FILE *file, flat_table_t *table)
{
    char line[FLAT_STRING_SIZE];
    int status = 0;

    for (unsigned int i = 0; i < table->size; i++)
    {
        if (imp__fget_line(line, FLAT_STRING_SIZE, file))
        {
            status = -3; // bad file
            printf("bad file!\n");
            break;
        }
        else if ((status = sread_flat(line, table->flats_array + i)) != 0)
        {
            printf("status was = %d. ", status);

            status = -4; // bad string
            printf("bad string! '%s'\n", line);
            break;
        }
    }

    return status;
}

flat_table_t ft_create(void)
{
    flat_table_t table = {
        .flats_array = NULL,
        .size = 0u
    };

    return table;
}

flat_table_t ft_clone(flat_table_t *original)
{
    flat_table_t copy = *original;

    copy.size = original->size;
    copy.flats_array = malloc(copy.size * sizeof(flat_t));

    for (int i = 0; i < copy.size; i++)
        copy.flats_array[i] = clone_flat(original->flats_array + i);

    return copy;
}

int ft_read(FILE *file, flat_table_t *table)
{
    // count flats amount
    unsigned int flats_amount = imp__count_flats_amount(file);
    if (flats_amount == 0)
        return -1; // no data was readed

    // allocate memory
    table->flats_array = (flat_t *)calloc(flats_amount, sizeof(flat_t));
    if (table->flats_array == NULL)
        return -2; // bad allocation

    table->size = flats_amount;

    // read file line by line
    if (imp__read_flat_table_from_file(file, table))
    {
        // could not read
        ft_free(table);
        return -3;
    }

    return 0;
}

int ft_append_flat(flat_table_t *table, flat_t *flat)
{
    // realloc array for new element
    table->size += 1;
    table->flats_array = (flat_t *)realloc(table->flats_array, table->size * sizeof(flat_t));
    table->flats_array[table->size - 1] = *flat;

    return 0;
}

static int imp__flat_comp(void *flat_1, void *flat_2)
{
    return ((flat_t*)flat_2)->rooms_amount - ((flat_t*)flat_1)->rooms_amount;
}

static int imp__flat_key_comp(void *flat_1p, void *flat_2p)
{
    return ((flat_key_t *)flat_2p)->rooms_amount - ((flat_key_t *)flat_1p)->rooms_amount;
}

static void imp__insertion(void *a, size_t count, size_t size, __comp_fn_t comp, __swap_fn_t swap, bool ascending)
{
    #define A(i) (void*)((char*)a + size * (i))

    for (size_t i = count; i > 0; i--)
    {
        for (size_t j = 0; j + 1 < i; j++)
        {
            bool res = comp(A(j), A(j + 1)) < 0;
            if ((res && ascending) || (!res && !ascending))
                swap(A(j), A(j + 1));
        }
    }

    #undef A
}

static void imp__merge(void *a, size_t count, size_t size, __comp_fn_t comp, __assign_fn_t assign, bool ascending)
{
    #define A(ar,id) (void *)((char *)ar + id * size)

    int rght, rend;
    int i, j, m;

    void *b = malloc(count * size);

    for (int k = 1; k < count; k *= 2)
    {
        for (int left = 0; left + k < count; left += k * 2)
        {
            rght = left + k;
            rend = rght + k;
            if (rend > count)
                rend = count;
            m = left;
            i = left;
            j = rght;
            while (i < rght && j < rend)
            {
                bool cmp = comp(A(a, i), A(a, j)) < 0;
                if ((cmp || ascending) && (!cmp || !ascending))
                {
                    assign(A(b, m), A(a, i)); // b[m] = a[i];
                    i++;
                }
                else
                {
                    assign(A(b, m), A(a, j)); // b[m] = a[j];
                    j++;
                }
                m++;
            }
            while (i < rght)
            {
                assign(A(b, m), A(a, i)); // b[m] = a[i];
                i++;
                m++;
            }
            while (j < rend)
            {
                assign(A(b, m), A(a, j)); // b[m] = a[j];
                j++;
                m++;
            }
            for (m = left; m < rend; m++)
                assign(A(a, m), A(b, m)); // a[m] = b[m];
        }
    }

    free(b);

    #undef A
}

void ft_gen_keys(flat_table_t *table, keys_table_t *keys)
{
    for (size_t i = 0; i < table->size; i++)
    {
        keys->keys[i].id = i;
        keys->keys[i].rooms_amount = table->flats_array[i].rooms_amount;
    }
}

static size_t imp__apply_sort_method(flat_table_t *table, keys_table_t *keys, sort_params_t params, sort_method_fn_t method)
{
    size_t result = 0UL;

    if (!params.real_sort)
    {
        for (size_t i = 0; i < 1 + 0 * PERF_TIMES; i++)
        {
            flat_table_t clone = ft_clone(table);
            clock_t __start_time = __rdtsc();
            method(clone.flats_array, clone.size, sizeof(flat_t), imp__flat_comp, assign_flat, params.ascending);
            result += __rdtsc() - __start_time;
            ft_free(&clone);
        }
    }
    else
        method(table->flats_array, table->size, sizeof(flat_t), imp__flat_comp, assign_flat, params.ascending);

    return result;
}

static size_t imp__apply_sort_method_key(flat_table_t *table, keys_table_t *keys, sort_params_t params, sort_method_fn_t method)
{
    size_t result = 0UL;

    if (!params.real_sort)
    {
        keys_table_t keys_clone;
        keys_clone.size = table->size;
        keys_clone.keys = malloc(table->size * sizeof(flat_key_t));

        for (size_t i = 0; i < 1 + 0 * PERF_TIMES; i++)
        {
            ft_gen_keys(table, &keys_clone);
            clock_t __start_time = __rdtsc();
            method(keys_clone.keys, keys_clone.size, sizeof(flat_key_t), imp__flat_key_comp, assign_flat_key, params.ascending);
            result += __rdtsc() - __start_time;
        }
        free(keys_clone.keys);
    }
    else
        method(keys->keys, table->size, sizeof(flat_key_t), imp__flat_key_comp, assign_flat_key, params.ascending);

    return result;
}

size_t ft_sort_a_fast(flat_table_t *table, keys_table_t *keys, sort_params_t params)
{
    return imp__apply_sort_method(table, keys, params, imp__merge);
}

size_t ft_sort_a_slow(flat_table_t *table, keys_table_t *keys, sort_params_t params)
{
    return imp__apply_sort_method(table, keys, params, imp__insertion);
}

size_t ft_sort_b_fast(flat_table_t *table, keys_table_t *keys, sort_params_t params)
{
    return imp__apply_sort_method_key(table, keys, params, imp__merge);
}

size_t ft_sort_b_slow(flat_table_t *table, keys_table_t *keys, sort_params_t params)
{
    return imp__apply_sort_method_key(table, keys, params, imp__insertion);
}

bool ft_flat_satisfies(flat_t *flat, float price_1, float price_2)
{
    float price = flat->price_per_m2 * flat->area;
    return flat->type == SECONDARY && flat->rooms_amount == 2 && !flat->type_data.was_pets && price_1 <= price && price < price_2;
}

int ft_delete_flat(flat_table_t *table, unsigned int id, flat_t *deleted_flat)
{
    if (0 > id || id >= table->size)
        return -1;
    
    *deleted_flat = table->flats_array[id];

    while (id + 1 < table->size)
    {
        table->flats_array[id] = table->flats_array[id + 1];
        id++;
    }

    table->size--; // realloc ?
    return 0;
}

void ft_free(flat_table_t *table)
{
    if (table == NULL)
        return;

    if (table->flats_array != NULL)
    {
        free(table->flats_array);
        table->flats_array = NULL;
        table->size = 0;
    }
}
