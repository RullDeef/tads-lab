#include "addr_list.h"
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

struct addr_list al_create(void)
{
    return (struct addr_list) { .size = 0U, .head = NULL, .tail = NULL };
}

void al_destroy(struct addr_list *al)
{
    if (al->size != 0U)
    {
        for (struct __a_lst_node *node = al->head, *next = al->head->next;
                node;
                node = next, next = (next == NULL ? NULL : next->next))
            free(node);
        al->size = 0U;
    }
}

bool al_contains(struct addr_list *al, void *addr)
{
    for (struct __a_lst_node *node = al->head; node; node = node->next)
        if (node->addr == addr)
            return true;
    return false;
}

void al_insert(struct addr_list *al, void *addr)
{
    if (al->size == 0U)
    {
        struct __a_lst_node *new_node = calloc(1, sizeof(struct __a_lst_node));
        new_node->addr = addr;
        al->head = new_node;
        al->tail = new_node;
        al->size++;
    }
    else
    {
        bool inserted = false;
        for (struct __a_lst_node *node = al->head; node; node = node->next)
        {
            if (node->addr > addr)
            {
                struct __a_lst_node *new_node = calloc(1, sizeof(struct __a_lst_node));
                new_node->addr = addr;
                new_node->next = node;
                new_node->prev = node->prev;

                if (node == al->head)
                    al->head = new_node;
                else
                    node->prev->next = new_node;
                node->prev = new_node;

                al->size++;
                inserted = true;
                break;
            }
        }

        if (!inserted) // push back
        {
            struct __a_lst_node *new_node = calloc(1, sizeof(struct __a_lst_node));
            new_node->addr = addr;
            // new_node->next = NULL;
            new_node->prev = al->tail;

            al->tail->next = new_node;
            al->tail = new_node;
            al->size++;
        }
    }
}

void al_remove(struct addr_list *al, void *addr)
{
    for (struct __a_lst_node *node = al->head; node; node = node->next)
    {
        if (node->addr == addr)
        {
            if (node == al->head)
                al->head = node->next;
            else
                node->prev->next = node->next;
            
            if (node == al->tail)
                al->tail = node->prev;
            else
                node->next->prev = node->prev;

            free(node);
            al->size--;
            break;
        }
    }
}

struct __a_lst_node *al_get(struct addr_list *al, void *addr)
{
    for (struct __a_lst_node *node = al->head; node; node = node->next)
        if (node->addr == addr)
            return node;

    assert(0);
    return NULL;
}
