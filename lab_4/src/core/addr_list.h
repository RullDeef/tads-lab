#ifndef __ADDR_LIST_H_
#define __ADDR_LIST_H_

#include <stdint.h>
#include <stdbool.h>

struct __a_lst_node
{
    void *addr;
    bool used;
    bool reused;
    struct __a_lst_node *prev;
    struct __a_lst_node *next;
};

struct addr_list
{
    uint32_t size;
    struct __a_lst_node *head;
    struct __a_lst_node *tail;
};

struct addr_list al_create(void);
void al_destroy(struct addr_list *al);

bool al_contains(struct addr_list *al, void *addr);

void al_insert(struct addr_list *al, void *addr);
void al_remove(struct addr_list *al, void *addr);

struct __a_lst_node *al_get(struct addr_list *al, void *addr);

#endif
