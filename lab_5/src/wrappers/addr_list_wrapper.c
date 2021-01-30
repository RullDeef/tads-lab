#include "addr_list_wrapper.h"
#include <stdio.h>
#include <assert.h>

struct addr_list_wrapper alw_create(void)
{
    struct addr_list_wrapper alw;
    alw.list = al_create();
    return alw;
}

void alw_destroy(struct addr_list_wrapper *alw)
{
    al_destroy(&alw->list);
}

void alw_handle_push(struct addr_list_wrapper *alw, struct queue *qu)
{
    if (qu->type == QUEUE_LINKED_LIST)
    {
        void *addr = qu->imp.list.last;
        if (al_contains(&alw->list, addr))
            al_get(&alw->list, addr)->reused = true;
        else
        {
            al_insert(&alw->list, addr);
            al_get(&alw->list, addr)->used = true;
        }
    }
}

void alw_handle_pop(struct addr_list_wrapper *alw, struct queue *qu)
{
    if (qu->type == QUEUE_LINKED_LIST)
    {
        void *addr = &(qu->imp.list.first->next);

        assert(al_contains(&alw->list, addr));

        struct __a_lst_node *node = al_get(&alw->list, addr);
        assert(node); // could not get
        node->used = false;
        node->reused = false;
    }
}

void alw_handle_clear(struct addr_list_wrapper *alw)
{
    alw->list.size = 0;
}

static void print_addrs(void *addr_1, void *addr_2, bool used, bool reused, uint32_t same)
{
    int int_addr_1 = (int64_t)addr_1 & 0xFFFF;
    int int_addr_2 = (int64_t)addr_2 & 0xFFFF;

    if (used)
        printf("\033[31;1;1m");
    else if (reused)
        printf("\033[33;1;1m");
    else
        printf("\033[32;1;1m");

    if (same == 1)
        printf(" %s 0x%04x\n", used ? ">" : " ", int_addr_1);
    else
        printf(" %s 0x%04x ... 0x%04x (x%u)\n", (used || reused) ? ">" : " ", int_addr_1, int_addr_2, same);
    
    printf("\033[0m");
}

void alw_show(struct addr_list_wrapper *alw)
{
    if (alw->list.size == 0U)
        return;

    printf("Список используемых адресов:\n");

    for (uint32_t i = 0; i < alw->list.size;)
    {
        uint32_t same = 1;
        while (i + same < alw->list.size && alw->list.data[i].used == alw->list.data[i + same].used &&
               alw->list.data[i].reused == alw->list.data[i + same].reused)
            same++;

        print_addrs(alw->list.data[i].addr, alw->list.data[i + same - 1].addr, alw->list.data[i].used, alw->list.data[i].reused, same);
        i += same;
    }

    printf(" ┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅\n");
    printf(" \033[31;1;1mxxx\033[0m - Адрес занят.\n");
    printf(" \033[32;1;1mxxx\033[0m - Адрес свободен.\n");
    printf(" \033[33;1;1mxxx\033[0m - Адрес используется повторно.\n");
    printf("\n");
}
