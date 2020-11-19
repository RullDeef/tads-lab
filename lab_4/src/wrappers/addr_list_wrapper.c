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

void alw_handle_push(struct addr_list_wrapper *alw, struct stack *st)
{
    assert(st->data.stack_lst.size != 0U);

    void *addr = st->data.stack_lst.top;
    if (al_contains(&alw->list, addr))
        al_get(&alw->list, addr)->reused = true;
    else
    {
        al_insert(&alw->list, addr);
        al_get(&alw->list, addr)->used = true;
    }
}

void alw_handle_pop(struct addr_list_wrapper *alw, struct stack *st)
{
    assert(st->data.stack_lst.size != 0U);
    
    void *addr = st->data.stack_lst.top;

    assert(al_contains(&alw->list, addr));

    struct __a_lst_node *node = al_get(&alw->list, addr);
    assert(node); // could not get
    node->used = false;
    node->reused = false;
}

void alw_handle_clear(struct addr_list_wrapper *alw)
{
    alw->list.size = 0;
}

static void print_addr(void *addr, bool used, bool reused)
{
    int int_addr = (int64_t)addr & 0xFFFF;
    if (used)
        printf("\033[31;1;1m");
    else if (reused)
        printf("\033[33;1;1m");
    else
        printf("\033[32;1;1m");

    printf(" %s 0x%04x (%d)\n", used ? ">" : " ", int_addr, ((struct __st_lst_node *)addr)->data);
    printf("\033[0m");
}

void alw_show(struct addr_list_wrapper *alw)
{
    if (alw->list.size == 0U)
        return;

    printf("Список используемых адресов:\n");

    for (uint32_t i = 0; i < alw->list.size; i++)
        print_addr(alw->list.data[i].addr, alw->list.data[i].used, alw->list.data[i].reused);

    printf(" ┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅\n");
    printf(" \033[31;1;1mxxx\033[0m - Адрес занят.\n");
    printf(" \033[32;1;1mxxx\033[0m - Адрес свободен.\n");
    printf(" \033[33;1;1mxxx\033[0m - Адрес используется повторно.\n");
    printf("\n");
}
