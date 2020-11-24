#ifndef __WORKER_H_
#define __WORKER_H_

#include <stdbool.h>
#include "core/queue/queue.h"

typedef struct
{
    float begin;
    float end;
} time_interval_t;

typedef struct
{
    time_interval_t t_in1;
    time_interval_t t_in2;
    time_interval_t t_out1;
    time_interval_t t_out2;
} worker_params_t;

typedef struct
{
    float total_push_time;
    float total_pop_time;

    unsigned long long total_push_us;
    unsigned long long total_pop_us;

    float avg_push_time;
    float avg_pop_time;

    uint32_t total_push_amount;
    uint32_t total_pop_amount;
    uint32_t avg_push_ns;
    uint32_t avg_pop_ns;

    uint32_t max_size;
    uint32_t curr_size;

    float total_wait_time; // время пребывания заявок в очереди (заявок-е.в.)
    float avg_wait_time; // среднее время пребывания заявок в очереди
} queue_stats_t;

typedef struct
{
    float time;
    float work_time;
    uint32_t request_index;

    uint32_t requests_1;
    uint32_t requests_2;

    // сколько заявок из 2 очереди было выброшено
    uint32_t request_dismissed;

    queue_stats_t qu1;
    queue_stats_t qu2;
} worker_stats_t;

typedef struct
{
    bool initialized;

    float next_req1_time;
    float next_req2_time;

    qdata_t curr_request;
    float curr_proc_time;
} worker_interm_state_t;

struct worker
{
    struct queue qu1;
    struct queue qu2;

    worker_params_t params;
    worker_stats_t stats;
};

struct worker wk_create(queue_imp_t imp_qu1, queue_imp_t imp_qu2, uint32_t qu_capacity);
void wk_destroy(struct worker *wk);

worker_params_t wk_default_params(void);

// Моделирует ОА до времени полной обработки requests_amount заявок первого типа.
int wk_model_run(struct worker *wk, uint32_t requests_amount, worker_interm_state_t *state);

#endif
