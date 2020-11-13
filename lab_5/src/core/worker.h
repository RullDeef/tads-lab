#ifndef __WORKER_H_
#define __WORKER_H_

#include "core/queue/queue.h"

typedef struct
{
    float begin;
    float end;
} time_interval_t;

typedef struct
{
    time_interval_t t_in1;
    time_interval_t t_out1;
    time_interval_t t_in2;
    time_interval_t t_out2;

    time_interval_t t_proc;
} worker_params_t;

struct worker
{
    float time;

    struct queue qu1;
    struct queue qu2;

    worker_params_t params;
};

struct worker wk_create(queue_imp_t imp_qu1, queue_imp_t imp_qu2, uint32_t qu_capacity);
void wk_destroy(struct worker *wk);

worker_params_t wk_default_params(void);

// Моделирует ОА до времени полной обработки requests_amount заявок первого типа.
void wk_model_run(struct worker *wk, uint32_t requests_amount);

#endif
