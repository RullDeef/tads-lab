#ifndef __QUEUE_ARR_H_
#define __QUEUE_ARR_H_

#include <stdint.h>
#include <ctype.h>

#ifndef __QDATA_DEFINED
#define __QDATA_DEFINED
typedef uint32_t qdata_t;
#endif

struct queue_arr
{
    uint32_t capacity;
    uint32_t size;
    qdata_t *begin; // начало массива
    qdata_t *end;   // конец массива
    qdata_t *first; // указатель на первый элемент в очереди
    qdata_t *last;  // указатель за последним элементом в очереди
};

struct queue_arr qua_create(uint32_t capacity);
void qua_destroy(struct queue_arr *qua);

int qua_push_back(struct queue_arr *qua, qdata_t value);
int qua_pop_front(struct queue_arr *qua, qdata_t *value);

#endif
