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
int ht_insert(struct hash_table *ht, int key)
{
    int status = -1;
    unsigned int hash = ht->func(key) % ht->size;

    for (unsigned int i = 0; i < ht->size && ht->data[hash].valid; i++)
        hash = (hash + ht->step) % ht->size;

    if (!ht->data[hash].valid)
    {
        ht->data[hash].key = key;
        ht->data[hash].valid = true;
        status = 0;
    }

    return status;
}

// 0 - ок, есть. -1 - ключа нет
int ht_find(struct hash_table *ht, int key)
{
    int status = -1;
    unsigned int hash = ht->func(key) % ht->size;

    for (unsigned int i = 0; i < ht->size && ht->data[hash].valid && ht->data[hash].key != key; i++)
        hash = (hash + ht->step) % ht->size;
    
    if (key == ht->data[hash].key)
        status = 0;

    return status;
}

unsigned int hash_func_1(int key)
{
    return key + 200;
}

unsigned int hash_func_2(int key)
{
    unsigned int sum = 0U;

    sum += 54 * (key % 53);
    sum += 59 * (key % 59);
    sum += 61 * (key % 61);

    return sum;
}

unsigned int hash_func_3(int key)
{
    unsigned int sum = 0U;
    if (key < 0) key *= -1;

    while (key > 0)
    {
        sum += key * (key % 11);
        key = key / 11;
    }

    return sum;
}
