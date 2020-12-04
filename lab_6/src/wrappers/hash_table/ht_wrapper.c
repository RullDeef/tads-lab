#include "ht_wrapper.h"

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
        *htw = htw_create(numbers_count + 5U, 1U, hash_func_1);

        rewind(file);
        while (fscanf(file, "%d", &num) != EOF)
            htw_insert(htw, num);
    }

    return status;
}

void htw_fprintf(FILE *file, struct ht_wrapper *htw)
{
    struct hash_table ht = htw->table;

    for (unsigned int i = 0U; i < ht.size; i++)
        if (1 || ht.data[i].valid)
            fprintf(file, "|%4u|" + (i != 0), i);

    fprintf(file, "\n");

    for (unsigned int i = 0U; i < ht.size; i++)
    {
        if (ht.data[i].valid)
            fprintf(file, "|%4d|" + (i != 0), ht.data[i].key);
        else
            fprintf(file, "|    |" + (i != 0));
    }

    fprintf(file, "\n");
}
