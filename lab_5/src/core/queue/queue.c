#include <assert.h>
#include "queue.h"

struct queue qu_create(queue_imp_t type, uint32_t capacity)
{
    struct queue qu = { .type = type };

    if (type == QUEUE_ARRAY)
        qu.imp.array = qua_create(capacity);
    else if (type == QUEUE_LINKED_LIST)
        qu.imp.list = qul_create();
    else
        assert(0 && "invalid queue imp type");

    return qu;
}

void qu_destroy(struct queue *qu)
{
    if (qu->type == QUEUE_ARRAY)
        qua_destroy(&qu->imp.array);
    else
        qul_destroy(&qu->imp.list);
}

uint32_t qu_get_size(struct queue *qu)
{
    if (qu->type == QUEUE_ARRAY)
        return qu->imp.array.size;
    else
        return qu->imp.list.size;
}

int qu_push_back(struct queue *qu, qdata_t value)
{
    if (qu->type == QUEUE_ARRAY)
        return qua_push_back(&qu->imp.array, value);
    else
        return qul_push_back(&qu->imp.list, value);
}

int qu_pop_front(struct queue *qu, qdata_t *value)
{
    if (qu->type == QUEUE_ARRAY)
        return qua_pop_front(&qu->imp.array, value);
    else
        return qul_pop_front(&qu->imp.list, value);
}
