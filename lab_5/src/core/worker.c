#include "worker.h"

struct worker wk_create(queue_imp_t imp_qu1, queue_imp_t imp_qu2, uint32_t qu_capacity)
{
    struct worker wk = { .time = 0.0f };

    wk.qu1 = qu_create(imp_qu1, qu_capacity);
    wk.qu2 = qu_create(imp_qu2, qu_capacity);
    wk.params = wk_default_params();

    return wk;
}

void wk_destroy(struct worker *wk);

worker_params_t wk_default_params(void)
{
    worker_params_t params;

    params.t_in1.begin = 1.0f;
    params.t_in1.end = 5.0f;

    params.t_in2.begin = 0.0f;
    params.t_in2.end = 3.0f;

    params.t_out1.begin = 0.0f;
    params.t_out1.end = 4.0f;

    params.t_out2.begin = 0.0f;
    params.t_out2.end = 1.0f;

    return params;
}

// Моделирует ОА до времени полной обработки requests_amount заявок первого типа.
void wk_model_run(struct worker *wk, uint32_t requests_amount);
