#ifndef __FLAT_TABLE_H_
#define __FLAT_TABLE_H_

#include <stdio.h>
#include <stdbool.h>
#include <assert.h>
#include "flat.h"

#ifndef NDEBUG
#define assert_flat_table(table)              \
    {                                         \
        assert((table) != NULL);              \
        assert((table)->flats_array != NULL); \
        assert((table)->size > 0);            \
    }
#else
#define assert_flat_table(table)
#endif

typedef struct
{
    size_t size;
    flat_t *flats_array;
} flat_table_t;

typedef enum
{
    ADDRESS,
    AREA,
    ROOMS_AMOUNT
} sort_key_t;

typedef struct
{
    sort_key_t key;
    bool ascending;
    bool real_sort;
} sort_params_t;

typedef void (*search_callback_t)(flat_t *);
typedef size_t (*sort_fn_t)(flat_table_t *, flat_t **, sort_params_t);

flat_table_t ft_create();
flat_table_t ft_clone(flat_table_t *original);
void ft_free(flat_table_t *flat_table);

int ft_read(FILE *file, flat_table_t *flat_table);
int ft_write(FILE *file, flat_table_t *flat_table);

int ft_append_flat(flat_table_t *flat_table, flat_t *flat);
int ft_delete_flat(flat_table_t *table, unsigned int id, flat_t *deleted_flat);

bool ft_flat_satisfies(flat_t *flat, float price_1, float price_2);

void ft_gen_keys(flat_table_t *table, flat_t **keys);

size_t ft_sort_a_fast(flat_table_t *table, flat_t **keys, sort_params_t params);
size_t ft_sort_a_slow(flat_table_t *table, flat_t **keys, sort_params_t params);
size_t ft_sort_b_fast(flat_table_t *table, flat_t **keys, sort_params_t params);
size_t ft_sort_b_slow(flat_table_t *table, flat_t **keys, sort_params_t params);

#endif // __FLAT_TABLE_H_
