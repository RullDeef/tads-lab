#ifndef __QUEUE_H_
#define __QUEUE_H_

#include <stdint.h>

#ifndef __QDATA_DEFINED
#define __QDATA_DEFINED
typedef uint32_t qdata_t;
#endif

#include "queue_arr.h"
#include "queue_lst.h"

typedef enum
{
    QUEUE_ARRAY,
    QUEUE_LINKED_LIST
} queue_imp_t;

struct queue
{
    queue_imp_t type;
    union
    {
        struct queue_arr array;
        struct queue_lst list;
    } imp;
};

struct queue qu_create(queue_imp_t type, uint32_t capacity);
void qu_destroy(struct queue *qu);

uint32_t qu_get_size(struct queue *qu);

int qu_push_back(struct queue *qu, qdata_t value);
int qu_pop_front(struct queue *qu, qdata_t *value);

#endif
