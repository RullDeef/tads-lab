#include <memory.h>
#include <math.h>
#include "utils/timing.h"
#include "worker.h"

static float get_random_time(time_interval_t tv)
{
    float t = (rand() % 1001) / 1000.0f;
    return tv.begin + (tv.end - tv.begin) * t;
}

static queue_stats_t qu_start_stats(void)
{
    queue_stats_t stats;
    memset(&stats, 0, sizeof(queue_stats_t));
    return stats;
}

static worker_stats_t wk_start_stats(void)
{
    worker_stats_t stats;

    stats.time = 0.0f;
    stats.work_time = 0.0f;
    stats.request_index = 0U;
    stats.requests_1 = 0U;
    stats.requests_2 = 0U;
    stats.request_dismissed = 0U;
    stats.qu1 = qu_start_stats();
    stats.qu2 = qu_start_stats();

    return stats;
}

struct worker wk_create(queue_imp_t imp_qu1, queue_imp_t imp_qu2, uint32_t qu_capacity)
{
    struct worker wk;

    wk.qu1 = qu_create(imp_qu1, qu_capacity);
    wk.qu2 = qu_create(imp_qu2, qu_capacity);
    wk.params = wk_default_params();
    wk.stats = wk_start_stats();

    return wk;
}

void wk_destroy(struct worker *wk)
{
    qu_destroy(&wk->qu1);
    qu_destroy(&wk->qu2);
}

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

static int model_req1_arrive(struct worker *wk)
{
    int status;

    BEGIN_TIMER;
    status = qu_push_back(&wk->qu1, wk->stats.request_index);
    END_TIMER;

    wk->stats.qu1.total_push_amount++;
    wk->stats.qu1.total_push_us += TIMER_MICROSECONDS;
    wk->stats.qu1.curr_size = qu_get_size(&wk->qu1);
    if (wk->stats.qu1.max_size < wk->stats.qu1.curr_size)
        wk->stats.qu1.max_size = wk->stats.qu1.curr_size;
    wk->stats.request_index++;
    wk->stats.requests_1++;

    return status;
}

static int model_req2_arrive(struct worker *wk)
{
    int status;
    
    BEGIN_TIMER;
    status = qu_push_back(&wk->qu2, wk->stats.request_index);
    END_TIMER;

    wk->stats.qu2.total_push_amount++;
    wk->stats.qu2.total_push_us += TIMER_MICROSECONDS;
    wk->stats.qu2.curr_size = qu_get_size(&wk->qu2);
    if (wk->stats.qu2.max_size < wk->stats.qu2.curr_size)
        wk->stats.qu2.max_size = wk->stats.qu2.curr_size;
    wk->stats.request_index++;
    wk->stats.requests_2++;

    return status;
}

static int model_proc_next_req(struct worker *wk, qdata_t *value)
{
    int status = EXIT_SUCCESS;

    if (qu_get_size(&wk->qu1) > 0U)
    {
        BEGIN_TIMER;
        status = qu_pop_front(&wk->qu1, value);
        END_TIMER;
        wk->stats.qu1.total_pop_amount++;
        wk->stats.qu1.total_pop_us += TIMER_MICROSECONDS;
        *value = 1U;
    }
    else if (qu_get_size(&wk->qu2) > 0U)
    {
        BEGIN_TIMER;
        status = qu_pop_front(&wk->qu2, value);
        END_TIMER;
        wk->stats.qu2.total_pop_amount++;
        wk->stats.qu2.total_pop_us += TIMER_MICROSECONDS;
        *value = 2U;
    }
    else
        *value = 0U;

    return status;
}

// Моделирует ОА до времени полной обработки requests_amount заявок первого типа.
int wk_model_run(struct worker *wk, uint32_t requests_amount)
{
    int status = EXIT_SUCCESS;

    float next_req1_time = get_random_time(wk->params.t_in1);
    float next_req2_time = get_random_time(wk->params.t_in2);

    // время на обработку текущего элемента из непустой очереди
    // элемент в данный момент уже находится в ОА, поэтому удалять
    // из очереди ничего не нужно.
    float curr_proc_time = 0.0f;
    qdata_t curr_request = 0U; // 1 - из первой очереди, 2 - из второй

    while (status == EXIT_SUCCESS && requests_amount > 0U)
    {
        // определиться с текущим событиями и обработать их
        if (next_req1_time < 1.0e-3)
        {
            status = model_req1_arrive(wk);
            next_req1_time = get_random_time(wk->params.t_in1);
            wk->stats.qu1.total_push_time += next_req1_time;

            if (curr_request == 2U)
            {
                curr_proc_time = 0U;
                wk->stats.request_dismissed++;
            }
        }
        if (next_req2_time < 1.0e-3)
        {
            status = model_req2_arrive(wk);
            next_req2_time = get_random_time(wk->params.t_in2);
            wk->stats.qu2.total_push_time += next_req2_time;
        }
        if (curr_proc_time < 1.0e-3)
        {
            status = model_proc_next_req(wk, &curr_request);
            if (curr_request == 1U)
            {
                curr_proc_time = get_random_time(wk->params.t_out1);
                wk->stats.qu1.total_pop_time += curr_proc_time;
                requests_amount--;
            }
            else if (curr_request == 2U)
            {
                curr_proc_time = get_random_time(wk->params.t_out2);
                wk->stats.qu2.total_pop_time += curr_proc_time;
            }
            wk->stats.work_time += curr_proc_time;
        }

        // рассчитать смещение во времени до следующего ближайшего события
        float delta = fmin(next_req1_time, next_req2_time);
        if (curr_proc_time > 1.0e-3)
            delta =  fmin(delta, curr_proc_time);

        next_req1_time -= delta;
        next_req2_time -= delta;
        curr_proc_time -= delta;

        wk->stats.time += delta;
    }

    wk->stats.work_time -= curr_proc_time;
    if (curr_request == 1U)
        wk->stats.time += curr_proc_time;

    // обновить средние статистические показатели
    if (wk->stats.qu1.total_push_amount != 0.0f)
        wk->stats.qu1.avg_push_time = wk->stats.qu1.total_push_time / wk->stats.qu1.total_push_amount;
    if (wk->stats.qu1.total_pop_amount != 0.0f)
        wk->stats.qu1.avg_pop_time = wk->stats.qu1.total_pop_time / wk->stats.qu1.total_pop_amount;

    if (wk->stats.qu2.total_push_amount != 0.0f)
        wk->stats.qu2.avg_push_time = wk->stats.qu2.total_push_time / wk->stats.qu2.total_push_amount;
    if (wk->stats.qu2.total_pop_amount != 0.0f)
        wk->stats.qu2.avg_pop_time = wk->stats.qu2.total_pop_time / wk->stats.qu2.total_pop_amount;

    wk->stats.qu1.curr_size = qu_get_size(&wk->qu1);

    if (wk->stats.qu1.total_push_amount != 0.0f)
        wk->stats.qu1.avg_push_ns = 1000U * wk->stats.qu1.total_push_us / wk->stats.qu1.total_push_amount;
    if (wk->stats.qu1.total_pop_amount != 0.0f)
        wk->stats.qu1.avg_pop_ns = 1000U * wk->stats.qu1.total_pop_us / wk->stats.qu1.total_pop_amount;

    if (wk->stats.qu2.total_push_amount != 0.0f)
        wk->stats.qu2.avg_push_ns = 1000U * wk->stats.qu2.total_push_us / wk->stats.qu2.total_push_amount;
    if (wk->stats.qu2.total_pop_amount != 0.0f)
        wk->stats.qu2.avg_pop_ns = 1000U * wk->stats.qu2.total_pop_us / wk->stats.qu2.total_pop_amount;

    wk->stats.qu2.curr_size = qu_get_size(&wk->qu2);

    return status;
}
