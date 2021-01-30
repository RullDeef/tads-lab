#ifndef __FLAT_TABLE_H_
#define __FLAT_TABLE_H_

#include <stdio.h>
#include <stdbool.h>
#include <assert.h>
#include <sys/types.h>
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
    uint32_t size;
    flat_t *flats_array;
} flat_table_t;

flat_table_t ft_create(void);
flat_table_t ft_clone(flat_table_t *original);
void ft_free(flat_table_t *flat_table);

int ft_read(FILE *file, flat_table_t *flat_table);

int ft_append_flat(flat_table_t *flat_table, flat_t *flat);
int ft_delete_flat(flat_table_t *table, unsigned int id, flat_t *deleted_flat);

bool ft_flat_satisfies(flat_t *flat, float price_1, float price_2);

#endif // __FLAT_TABLE_H_
