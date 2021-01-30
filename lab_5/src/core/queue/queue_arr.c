#include <stdlib.h>
#include <memory.h>
#include <assert.h>
#include "queue_arr.h"

struct queue_arr qua_create(uint32_t capacity)
{
    assert(capacity > 0U);

    struct queue_arr qua = { .capacity = capacity, .size = 0U };
    qua.begin = malloc(capacity * sizeof(qdata_t));
    qua.end = qua.begin + capacity;
    assert(qua.begin != NULL);

    qua.first = qua.begin;
    qua.last = qua.begin;

    return qua;
}

void qua_destroy(struct queue_arr *qua)
{
    if (qua->begin != NULL)
    {
        free(qua->begin);
        memset(qua, 0, sizeof(struct queue_arr));
    }
}

int qua_push_back(struct queue_arr *qua, qdata_t value)
{
    int status = EXIT_FAILURE;

    if (qua->size < qua->capacity)
    {
        status = EXIT_SUCCESS;

        *(qua->last) = value;
        qua->last++;

        if (qua->last == qua->end)
            qua->last = qua->begin;

        qua->size++;
    }

    return status;
}

int qua_pop_front(struct queue_arr *qua, qdata_t *value)
{
    int status = EXIT_FAILURE;

    if (qua->size > 0U)
    {
        status = EXIT_SUCCESS;

        *value = *(qua->first);
        qua->first++;

        if (qua->first == qua->end)
            qua->first = qua->begin;

        qua->size--;
    }

    return status;
}
