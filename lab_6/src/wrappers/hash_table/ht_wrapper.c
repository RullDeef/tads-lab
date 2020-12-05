#include <stdio.h>
#include "utils/colors.h"
#include "utils/logger.h"
#include "ht_wrapper.h"

#define PRN_WIDTH 80
#define PRN_ITEM_WIDTH 4

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

int htw_insert(struct ht_wrapper *htw, int key)
{
    return ht_insert(&htw->table, key);
}

int htw_find(struct ht_wrapper *htw, int key)
{
    return ht_find(&htw->table, key);
}

int htw_fscanf(FILE *file, struct ht_wrapper *htw)
{
    int status = 0;

    unsigned int numbers_count = 0U;
    int num;

    rewind(file);
    while (fscanf(file, "%d", &num) != EOF)
        numbers_count++;

    if (numbers_count == 0U)
        status = -1;
    else
    {
        *htw = htw_create(numbers_count + 5U, 1U, hash_func_2);

        rewind(file);
        while (fscanf(file, "%d", &num) != EOF)
            htw_insert(htw, num);
    }

    return status;
}

void htw_fprintf(FILE *file, struct ht_wrapper *htw)
{
    struct hash_table ht = htw->table;

    unsigned int i = 0U;

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
}
