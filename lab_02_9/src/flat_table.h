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
        assert((table)->flat_ptrs != NULL);   \
        assert((table)->size > 0);            \
    }
#else
#define assert_flat_table(table)
#endif

typedef struct
{
    flat_t *flats_array;
    unsigned int size;
    flat_t **flat_ptrs;
} flat_table_t;

typedef enum
{
    ADDRESS,
    AREA,
    ROOMS_AMOUNT
} sort_key_t;

typedef void (*search_callback_t)(flat_t *);
typedef void (*sort_fn_t)(flat_table_t *, sort_key_t, bool);

flat_table_t create_flat_table();
flat_table_t clone_flat_table(flat_table_t *original);

int fread_flat_table(FILE *file, flat_table_t *flat_table);
int fwrite_flat_table(FILE *file, flat_table_t *flat_table);

int append_flat_table(flat_table_t *flat_table, flat_t *flat);
int search_flat_table(flat_table_t *table, float price_1, float price_2, search_callback_t callback);

void sort_flat_table_a_fast(flat_table_t *flat_table, sort_key_t key, bool ascending);
void sort_flat_table_a_slow(flat_table_t *flat_table, sort_key_t key, bool ascending);
void sort_flat_table_b_fast(flat_table_t *flat_table, sort_key_t key, bool ascending);
void sort_flat_table_b_slow(flat_table_t *flat_table, sort_key_t key, bool ascending);

flat_t delete_flat_table(flat_table_t *table, unsigned int id);

void free_flat_table(flat_table_t *flat_table);

#endif // __FLAT_TABLE_H_
