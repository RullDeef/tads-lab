#include <string.h>
#include "utils/logger.h"
#include "str_queue/str_queue.h"

#define CALLOCS_REDEFINE
#include "callocs.h"

struct str_queue
{
    struct str_queue *next;
    const char *str;
};

str_queue_t squ_create()
{
    return NULL;
}

void squ_insert(str_queue_t *queue, const char *str)
{
    str_queue_t top = malloc(sizeof(struct str_queue) + strlen(str) + 1U);
    if (top == NULL)
        log_error("bad top in squ_insert. Abort?))");
    top->next = *queue;
    top->str = (char *)(top + 1);
    strcpy((char *)top->str, str);
    *queue = top;
}

const char *squ_pop(str_queue_t *queue, char *str_buf, size_t n)
{
    if (*queue != NULL)
    {
        struct str_queue **back = queue;

        if ((*queue)->next != NULL)
        {
            struct str_queue *bef_back = *queue;
            while (bef_back->next->next != NULL)
                bef_back = bef_back->next;

            back = &(bef_back->next);
        }

        strncpy(str_buf, (*back)->str, n);
        free(*back);
        *back = NULL;
    }
    else
        return NULL;

    return str_buf;
}

bool squ_has(str_queue_t queue, const char *str)
{
    for (struct str_queue *node = queue; node != NULL; node = node->next)
        if (strcmp(node->str, str) == 0)
            return true;
    return false;
}

size_t squ_size(const str_queue_t *queue)
{
    size_t size = 0UL;

    for (str_queue_t node = *queue; node != NULL; node = node->next)
        size++;

    return size;
}

void squ_destroy(str_queue_t *queue)
{
    while (*queue != NULL)
    {
        str_queue_t next = (*queue)->next;
        free(*queue);
        *queue = next;
    }
}
