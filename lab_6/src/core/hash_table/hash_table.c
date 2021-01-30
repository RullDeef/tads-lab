#include <math.h>
#include <limits.h>
#include "hash_table.h"
#include "utils/logger.h"
#define CALLOCS_REDEFINE
#include "callocs.h"

struct hash_table ht_create(unsigned int size, unsigned int step, hash_func_t func)
{
    struct hash_table ht = { .size = 0U };

    ht.data = calloc(size, sizeof(ht_data_t));

    if (ht.data != NULL)
    {
        ht.size = size;
        ht.step = step;
        ht.func = func;
    }
    else
        log_error("ht_create: невозможно выделить %u байт памяти", size * sizeof(ht_data_t));

    return ht;
}

void ht_destroy(struct hash_table *ht)
{
    if (ht->data != NULL)
    {
        free(ht->data);
        ht->data = NULL;
    }
}

// 0 - ок, -1 - своб. место не найдено
int ht_insert(struct hash_table *ht, int key, int *cmp)
{
    int status = -1;
    unsigned int hash = ht->func(key) % ht->size;

    (*cmp)++;
    for (unsigned int i = 0; i < ht->size && ht->data[hash].valid; i++)
    {
        hash = (hash + ht->step) % ht->size;
        (*cmp)++;
    }

    if (!ht->data[hash].valid)
    {
        ht->data[hash].key = key;
        ht->data[hash].valid = true;
        status = 0;
    }

    return status;
}

int ht_shallow_insert(struct hash_table *ht, int key)
{
    int status = -1;
    unsigned int hash = ht->func(key) % ht->size;

    for (unsigned int i = 0; i < ht->size && ht->data[hash].valid; i++)
        hash = (hash + ht->step) % ht->size;

    if (!ht->data[hash].valid)
        status = 0;

    return status;
}

// 0 - ок, есть. -1 - ключа нет
int ht_find(struct hash_table *ht, int key, int *cmp)
{
    int status = -1;
    unsigned int hash = ht->func(key) % ht->size;

    (*cmp)++;
    for (unsigned int i = 0; i < ht->size && ht->data[hash].valid && ht->data[hash].key != key; i++)
    {
        hash = (hash + ht->step) % ht->size;
        (*cmp)++;
    }

    if (key == ht->data[hash].key)
        status = 0;

    return status;
}

unsigned int hash_func_1(int key)
{
    unsigned int sum = 0;

    if (key < 0)
        key *= -1;

    while (key != 0)
    {
        sum += key % 10;
        key /= 10;
    }

    return sum;
}

unsigned int hash_func_2(int key)
{
    unsigned int sum = 0;

    if (key < 0)
        key *= -1;

    key *= 59;
    while (key != 0)
    {
        sum += key % 10;
        key /= 10;
    }

    return sum;
}

unsigned int hash_func_3(int key)
{
    const long double golden_ratio = 0.618033988749894848204586834365638118;

    long double value = key * golden_ratio;
    value -= floorl(value);

    return UINT_MAX * value;
}
