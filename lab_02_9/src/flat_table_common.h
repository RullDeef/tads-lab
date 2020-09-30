#ifndef __FLAT_TABLE_COMMON_H_
#define __FLAT_TABLE_COMMON_H_

#include <stdio.h>
#include <stdbool.h>
#include "flat.h"

#ifndef _VAR_A
#include "flat_table_b.h"
#else
#include "flat_table_a.h"
#endif

typedef enum
{
    ADDRESS,
    AREA,
    ROOMS_AMOUNT
} sort_key_t;

flat_table_t create_flat_table();
flat_table_t clone_flat_table(flat_table_t *original);

int fread_flat_table(FILE *file, flat_table_t *flat_table);
int fwrite_flat_table(FILE *file, flat_table_t *flat_table);

int append_flat_table(flat_table_t *flat_table, flat_t *flat);
void sort_flat_table(flat_table_t *flat_table, sort_key_t key, bool ascending);

void search_flat_table(flat_table_t *table, bit_t field_type, bit_t field_value, flat_t **founded_flat);

void free_flat_table(flat_table_t *flat_table);

#endif // __FLAT_TABLE_COMMON_H_
