#include <stdlib.h>
#include <assert.h>
#include "queue_lst.h"

struct queue_lst qul_create(void)
{
    struct queue_lst qul = { .size = 0U, .first = NULL, .last = NULL };
    return qul;
}

void qul_destroy(struct queue_lst *qul)
{
    while (qul->size > 0U)
    {
        qdata_t value;
        qul_pop_front(qul, &value);
    }
}

int qul_push_back(struct queue_lst *qul, qdata_t value)
{
    int status = EXIT_FAILURE;

    if (qul->size == 0U)
    {
        status = EXIT_SUCCESS;
        qul->first = malloc(sizeof(struct __qu_lst_node));
        assert(qul->first != NULL);
        qul->first->next = NULL;
        qul->first->data = value;
        qul->last = &(qul->first->next);
        qul->size++;
    }
    else
    {
        status = EXIT_SUCCESS;
        *(qul->last) = malloc(sizeof(struct __qu_lst_node));
        assert(*(qul->last) != NULL);
        (*(qul->last))->data = value;
        (*(qul->last))->next = NULL;
        qul->last = &((*qul->last)->next);
        qul->size++;
    }

    return status;
}

int qul_pop_front(struct queue_lst *qul, qdata_t *value)
{
    int status = EXIT_FAILURE;

    if (qul->size != 0)
    {
        status = EXIT_SUCCESS;

        struct __qu_lst_node *next = qul->first->next;
        *value = qul->first->data;
        free(qul->first);
        qul->first = next;

        qul->size--;

        if (qul->size == 0U)
            qul->last = NULL;
    }

    return status;
}
