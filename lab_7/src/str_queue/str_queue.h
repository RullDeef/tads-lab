#ifndef __STR_QUEUE_H_
#define __STR_QUEUE_H_

#include <stdbool.h>

typedef struct str_queue *str_queue_t;

str_queue_t squ_create();

void squ_insert(str_queue_t *queue, const char *str);
const char *squ_pop(str_queue_t *queue, char *str_buf, size_t n);

bool squ_has(str_queue_t queue, const char *str);

size_t squ_size(const str_queue_t *queue);

void squ_destroy(str_queue_t *queue);

#endif // __STR_QUEUE_H_
