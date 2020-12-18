#include <stdio.h>
#include <stdbool.h>
#include <assert.h>
#include "utils/colors.h"
#include "utils/logger.h"
#include "ht_wrapper.h"

#define PRN_WIDTH 80
#define PRN_ITEM_WIDTH 5

#define ITEMS_COUNT ((PRN_WIDTH - 7) / PRN_ITEM_WIDTH)

struct ht_wrapper htw_create(unsigned int size, unsigned int step, hash_func_t func)
{
    struct ht_wrapper htw;
    htw.table = ht_create(size, step, func);
    return htw;
}

void htw_destroy(struct ht_wrapper *htw)
{
    ht_destroy(&htw->table);
}

int htw_insert(struct ht_wrapper *htw, int key, int *cmp)
{
    return ht_insert(&htw->table, key, cmp);
}

int htw_shallow_insert(struct ht_wrapper *htw, int key)
{
    return ht_shallow_insert(&htw->table, key);
}

int htw_find(struct ht_wrapper *htw, int key, int *cmp)
{
    return ht_find(&htw->table, key, cmp);
}

static bool is_prime(unsigned int num)
{
    for (unsigned int i = 2; i < num / 2; i++)
        if (num % i == 0)
            return false;
    return true;
}

int htw_fscanf(FILE *file, struct ht_wrapper *htw)
{
    int status = 0;

    unsigned int numbers_count = 0U;
    int num;
    int cmp = 0;

    rewind(file);
    while (fscanf(file, "%d", &num) != EOF)
        numbers_count++;

    numbers_count *= 1.2f;
    while (!is_prime(numbers_count))
        numbers_count++;

    *htw = htw_create(numbers_count, numbers_count / 3, hash_func_1);

    rewind(file);
    while (fscanf(file, "%d", &num) != EOF)
        htw_insert(htw, num, &cmp);

    return status;
}

size_t __get_collisions_amount(struct ht_wrapper *htw)
{
    size_t collisions = 0UL;

    for (unsigned int i = 0; i < htw->table.size; i++)
    {
        if (htw->table.data[i].valid)
        {
            int key = htw->table.data[i].key;
            if (htw->table.func(key) % htw->table.size != i)
                collisions++;
        }
    }

    return collisions;
}

void __fprint_func_name(FILE *file, hash_func_t func)
{
    fprintf(file, "Используемая хеш-функция: ");

    if (func == hash_func_1)
        fprintf(file, CLR_BR_GREEN "сумма цифр числа" CLR_RESET ".\n");
    else if (func == hash_func_2)
        fprintf(file, CLR_BR_GREEN "сумма цифр числа c солью" CLR_RESET ".\n");
    else if (func == hash_func_3)
        fprintf(file, CLR_BR_GREEN "хеширование Фибоначчи" CLR_RESET ".\n");
    else
        fprintf(file, CLR_BR_RED "(описание не предоставлено)" CLR_RESET ".\n");
}

void htw_fprintf(FILE *file, struct ht_wrapper *htw)
{
    struct hash_table ht = htw->table;

    unsigned int i = 0U;

    fprintf(file, "Хеш-таблица:\n");

    while (i < ht.size)
    {
        printf("index: ");

        for (unsigned int j = i; j < ht.size && j < i + ITEMS_COUNT; j++)
            if (1 || ht.data[j].valid)
                fprintf(file, "|%*u|" + (j != i), PRN_ITEM_WIDTH, j);

        fprintf(file, "\n");
        printf("key:   ");

        for (unsigned int j = i; j < ht.size && j < i + ITEMS_COUNT; j++)
        {
            if (ht.data[j].valid)
                fprintf(file, "|%*d|" + (j != i), PRN_ITEM_WIDTH, ht.data[j].key);
            else
                fprintf(file, "|%*.s|" + (j != i), PRN_ITEM_WIDTH, "");
        }

        fprintf(file, "\n");
        printf("hash:  ");

        for (unsigned int j = i; j < ht.size && j < i + ITEMS_COUNT; j++)
        {
            if (ht.data[j].valid)
            {
                unsigned int hash = ht.func(ht.data[j].key) % ht.size;
                if (hash == j)
                    fprintf(file, "|" CLR_BR_GREEN "%*u" CLR_RESET "|" + (j != i), PRN_ITEM_WIDTH, hash);
                else
                    fprintf(file, "|" CLR_RED "%*u" CLR_RESET "|" + (j != i), PRN_ITEM_WIDTH, hash);
            }
            else
                fprintf(file, "|%*.s|" + (j != i), PRN_ITEM_WIDTH, "");
        }

        fprintf(file, "\n");

        i += ITEMS_COUNT;
        if (i < ht.size)
            fprintf(file, "\n");
    }

    fprintf(file, "Размер структуры: " CLR_BR_GREEN "%lu" CLR_RESET " байт.\n", htw_calc_size(htw));
    fprintf(file, "Число ключей в таблице: " CLR_BR_GREEN "%u" CLR_RESET "/%u.\n", htw->table.size - htw_calc_remain_space(htw), htw->table.size);
    fprintf(file, "Среднее число сравнений в структуре: " CLR_BR_GREEN "%.2f" CLR_RESET ".\n", htw_calc_mean_cmp_amount(htw));
    fprintf(file, "Число коллизий в таблице: " CLR_RED "%lu" CLR_RESET ".\n", __get_collisions_amount(htw));
    __fprint_func_name(file, htw->table.func);
    fprintf(file, "\n");
}

size_t htw_calc_size(struct ht_wrapper *htw)
{
    (void)htw;
    return sizeof(struct hash_table) + htw->table.size * sizeof(ht_data_t);
}

static int __get_cmp_amount(struct hash_table *ht, int key)
{
    int cmps = 1;
    unsigned int hash = ht->func(key) % ht->size;

    for (unsigned int i = 0; i < ht->size && ht->data[hash].valid && ht->data[hash].key != key; i++, cmps++)
        hash = (hash + ht->step) % ht->size;

    return cmps;
}

float htw_calc_mean_cmp_amount(struct ht_wrapper *htw)
{
    int keys = 0;
    int cmps = 0;

    for (unsigned int i = 0; i < htw->table.size; i++)
    {
        if (htw->table.data[i].valid)
        {
            keys++;
            cmps += __get_cmp_amount(&htw->table, htw->table.data[i].key);
        }
    }

    return keys == 0 ? 0.0f : (float)cmps / keys;
}

unsigned int htw_get_keys_amount(struct ht_wrapper *htw)
{
    unsigned int amount = 0U;

    for (unsigned int i = 0; i < htw->table.size; i++)
        if (htw->table.data[i].valid)
            amount++;

    return amount;
}

unsigned int htw_calc_remain_space(struct ht_wrapper *htw)
{
    unsigned int space = 0U;

    for (unsigned int i = 0; i < htw->table.size; i++)
        if (!htw->table.data[i].valid)
            space++;

    return space;
}

unsigned int htw_calc_best_size(struct ht_wrapper *htw)
{
    unsigned int size = 1U;

    for (unsigned int i = 0; i < htw->table.size; i++)
        if (htw->table.data[i].valid)
            size++;

    size = 1.2f * size;

    while (!is_prime(size))
        size++;

    return size;
}

void htw_restruct(struct ht_wrapper *htw, unsigned int new_size, hash_func_t new_func)
{
    int cmp = 0;
    struct hash_table new_ht = ht_create(new_size, htw->table.step, new_func);

    // copy old keys
    for (unsigned int i = 0U; i < htw->table.size; i++)
        if (htw->table.data[i].valid)
            assert(ht_insert(&new_ht, htw->table.data[i].key, &cmp) == 0);

    ht_destroy(&htw->table);
    htw->table = new_ht;
}
