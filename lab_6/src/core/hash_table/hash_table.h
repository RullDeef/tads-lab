#ifndef __HASH_TABLE_H_
#define __HASH_TABLE_H_

#include <stdbool.h>

typedef unsigned int (*hash_func_t)(int);

typedef struct ht_data
{
    int key;
    bool valid;
} ht_data_t;

struct hash_table
{
    unsigned int size;
    unsigned int step;
    ht_data_t *data;
    hash_func_t func;
};

struct hash_table ht_create(unsigned int size, unsigned int step, hash_func_t func);
void ht_destroy(struct hash_table *ht);

// 0 - ок, -1 - своб. место не найдено
int ht_insert(struct hash_table *ht, int key);

// 0 - ок, есть. -1 - ключа нет
int ht_find(struct hash_table *ht, int key);

// несколько хеш-функций
unsigned int hash_func_1(int key);
unsigned int hash_func_2(int key);
unsigned int hash_func_3(int key);

#endif // __HASH_TABLE_H_
