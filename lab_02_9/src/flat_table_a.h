#ifndef __FLAT_TABLE_H_
#define __FLAT_TABLE_H_

#include "flat.h"
#include <assert.h>

/*
    Использование обычного массива (динамического).
    Без хранения ключей для сортировки (вариант А).
*/

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
    flat_t *flats_array;
    unsigned int size;
} flat_table_t;

#endif // __FLAT_TABLE_H_
