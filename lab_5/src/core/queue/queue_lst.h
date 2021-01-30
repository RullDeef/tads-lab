#ifndef __QUEUE_LST_H_
#define __QUEUE_LST_H_

#include <stdint.h>
#include <ctype.h>

#ifndef __QDATA_DEFINED
#define __QDATA_DEFINED
typedef uint32_t qdata_t;
#endif

struct __qu_lst_node
{
    struct __qu_lst_node *next;
    qdata_t data;
};

struct queue_lst
{
    uint32_t size;
    struct __qu_lst_node *first; // указатель на первый элемент в очереди
    struct __qu_lst_node **last; // указатель на поле <next> в последнем элементе очереди
};

struct queue_lst qul_create(void);
void qul_destroy(struct queue_lst *qul);

int qul_push_back(struct queue_lst *qul, qdata_t value);
int qul_pop_front(struct queue_lst *qul, qdata_t *value);

#endif
