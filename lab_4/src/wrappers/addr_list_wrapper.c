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
    node->used = false;
    node->reused = false;
}

void alw_show(struct addr_list_wrapper *alw)
{
    uint32_t amount_to_print = 0U;
    for (struct __a_lst_node *node = alw->list.head; node; node = node->next)
        if (!node->used || node->reused)
            amount_to_print++;

    if (amount_to_print == 0U)
        return;

    printf("Список свободных адресов:\n");

    for (struct __a_lst_node *node = alw->list.head; node; node = node->next)
    {
        if (!node->used)
            printf("   %p (%d)\n", node->addr, ((struct __st_lst_node *)node->addr)->data);
        else if (node->reused)
            printf(" * %p (%d)\n", node->addr, ((struct __st_lst_node *)node->addr)->data);
    }

    printf("* - Адрес используется повторно.\n");
    printf("\n");
}
