#include "addr_list.h"
#include <stdlib.h>
#include <stdio.h>
#include <memory.h>
#include <assert.h>

#define INIT_ALLOC 20U
#define ALLOC_DELTA 10U

struct addr_list al_create(void)
{
    struct addr_list al = { .size = 0U };
    al.data = malloc(INIT_ALLOC * sizeof(struct __a_lst_node));
    al.capacity = INIT_ALLOC;
    return al;
}

void al_destroy(struct addr_list *al)
{
    free(al->data);
    al->size = 0U;
    al->capacity = 0U;
}

bool al_contains(struct addr_list *al, void *addr)
{
    for (uint32_t i = 0; i < al->size; i++)
        if (al->data[i].addr == addr)
            return true;
    return false;
}

void al_insert(struct addr_list *al, void *addr)
{
    if (al->size == al->capacity)
    {
        al->capacity += ALLOC_DELTA;
        al->data = realloc(al->data, al->capacity * sizeof(struct __a_lst_node));
    }

    uint32_t i;
    for (i = al->size; i > 0 && al->data[i - 1].addr > addr; i--)
        al->data[i] = al->data[i - 1];
    
    memset(al->data + i, 0, sizeof(struct __a_lst_node));
    al->data[i].addr = addr;
    al->size++;
}

struct __a_lst_node *al_get(struct addr_list *al, void *addr)
{
    for (uint32_t i = 0; i < al->size; i++)
        if (al->data[i].addr == addr)
            return al->data + i;

    assert(0);
    return NULL;
}
