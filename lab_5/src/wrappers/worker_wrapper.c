#include <stdlib.h>
#include <math.h>
#include "utils/timing.h"
#include "worker_wrapper.h"

static float get_random_time(time_interval_t tv)
{
    float t = (rand() % 1001) / 1000.0f;
    return tv.begin + (tv.end - tv.begin) * t;
}

struct worker_wrapper wkw_create(queue_imp_t imp_qu1, queue_imp_t imp_qu2, uint32_t qu_capacity)
{
    struct worker_wrapper wkw;
    wkw.wk = wk_create(imp_qu1, imp_qu2, qu_capacity);
    wkw.alw = alw_create();
    return wkw;
}

void wkw_destroy(struct worker_wrapper *wkw)
{
    wk_destroy(&wkw->wk);
    alw_destroy(&wkw->alw);
}

static int model_req1_arrive(struct addr_list_wrapper *alw, struct worker *wk)
{
    int status;

    BEGIN_TIMER;
    status = qu_push_back(&wk->qu1, wk->stats.request_index);
    END_TIMER;

    alw_handle_push(alw, &wk->qu1);

    wk->stats.qu1.total_push_amount++;
    wk->stats.qu1.total_push_us += TIMER_MICROSECONDS;
    wk->stats.qu1.curr_size = qu_get_size(&wk->qu1);
    if (wk->stats.qu1.max_size < wk->stats.qu1.curr_size)
        wk->stats.qu1.max_size = wk->stats.qu1.curr_size;
    wk->stats.request_index++;
    wk->stats.requests_1++;

    return status;
}

static int model_req2_arrive(struct addr_list_wrapper *alw, struct worker *wk)
{
    int status;

    BEGIN_TIMER;
    status = qu_push_back(&wk->qu2, wk->stats.request_index);
    END_TIMER;

    alw_handle_push(alw, &wk->qu2);

    wk->stats.qu2.total_push_amount++;
    wk->stats.qu2.total_push_us += TIMER_MICROSECONDS;
    wk->stats.qu2.curr_size = qu_get_size(&wk->qu2);
    if (wk->stats.qu2.max_size < wk->stats.qu2.curr_size)
        wk->stats.qu2.max_size = wk->stats.qu2.curr_size;
    wk->stats.request_index++;
    wk->stats.requests_2++;

    return status;
}

static int model_proc_next_req(struct addr_list_wrapper *alw, struct worker *wk, qdata_t *value)
{
    int status = EXIT_SUCCESS;

    if (qu_get_size(&wk->qu1) > 0U)
    {
        alw_handle_pop(alw, &wk->qu1);
        BEGIN_TIMER;
        status = qu_pop_front(&wk->qu1, value);
        END_TIMER;
        wk->stats.qu1.total_pop_amount++;
        wk->stats.qu1.total_pop_us += TIMER_MICROSECONDS;
        *value = 1U;
    }
    else if (qu_get_size(&wk->qu2) > 0U)
    {
        alw_handle_pop(alw, &wk->qu2);
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

static queue_stats_t update_qu_stats(struct queue *qu, queue_stats_t stats)
{
    if (stats.total_push_amount != 0U)
        stats.avg_push_time = stats.total_push_time / stats.total_push_amount;
    if (stats.total_pop_amount != 0U)
        stats.avg_pop_time = stats.total_pop_time / stats.total_pop_amount;

    if (stats.total_push_amount != 0U)
        stats.avg_push_ns = 1000U * stats.total_push_us / stats.total_push_amount;
    if (stats.total_pop_amount != 0U)
        stats.avg_pop_ns = 1000U * stats.total_pop_us / stats.total_pop_amount;

    if (stats.total_push_amount != 0U)
        stats.avg_wait_time = stats.total_wait_time / stats.total_push_amount;

    stats.curr_size = qu_get_size(qu);
    return stats;
}

int wkw_model_run(struct worker_wrapper *wkw, uint32_t requests_amount, worker_interm_state_t *state)
{
    struct worker *wk = &wkw->wk;

    int status = EXIT_SUCCESS;

    if (!state->initialized)
    {
        state->next_req1_time = get_random_time(wk->params.t_in1);
        state->next_req2_time = get_random_time(wk->params.t_in2);
        // время на обработку текущего элемента из непустой очереди
        // элемент в данный момент уже находится в ОА, поэтому удалять
        // из очереди ничего не нужно.
        state->curr_request = 0U; // 1 - из первой очереди, 2 - из второй
        state->curr_proc_time = 0.0f;
    }

    while (status == EXIT_SUCCESS && requests_amount > 0U)
    {
        // определиться с текущим событиями и обработать их
        if (state->next_req1_time < 1.0e-3)
        {
            status = model_req1_arrive(&wkw->alw, wk);
            state->next_req1_time = get_random_time(wk->params.t_in1);
            wk->stats.qu1.total_push_time += state->next_req1_time;

            if (state->curr_request == 2U)
            {
                state->curr_proc_time = 0U;
                wk->stats.request_dismissed++;
            }
        }
        if (state->next_req2_time < 1.0e-3)
        {
            status = model_req2_arrive(&wkw->alw, wk);
            state->next_req2_time = get_random_time(wk->params.t_in2);
            wk->stats.qu2.total_push_time += state->next_req2_time;
        }
        if (state->curr_proc_time < 1.0e-3)
        {
            status = model_proc_next_req(&wkw->alw, wk, &state->curr_request);
            if (state->curr_request == 1U)
            {
                state->curr_proc_time = get_random_time(wk->params.t_out1);
                wk->stats.qu1.total_pop_time += state->curr_proc_time;
                requests_amount--;
            }
            else if (state->curr_request == 2U)
            {
                state->curr_proc_time = get_random_time(wk->params.t_out2);
                wk->stats.qu2.total_pop_time += state->curr_proc_time;
            }
            wk->stats.work_time += state->curr_proc_time;
        }

        // рассчитать смещение во времени до следующего ближайшего события
        float delta = fmin(state->next_req1_time, state->next_req2_time);
        if (state->curr_proc_time > 1.0e-3)
            delta = fmin(delta, state->curr_proc_time);

        state->next_req1_time -= delta;
        state->next_req2_time -= delta;
        state->curr_proc_time -= delta;

        wk->stats.qu1.total_wait_time += wk->stats.qu1.curr_size * delta;
        wk->stats.qu2.total_wait_time += wk->stats.qu2.curr_size * delta;

        wk->stats.time += delta;
    }

    if (!state->initialized)
    {
        wk->stats.work_time -= state->curr_proc_time;
        if (state->curr_request == 1U)
            wk->stats.time += state->curr_proc_time;
        state->initialized = true;
    }

    // обновить средние статистические показатели
    wk->stats.qu1 = update_qu_stats(&wk->qu1, wk->stats.qu1);
    wk->stats.qu2 = update_qu_stats(&wk->qu2, wk->stats.qu2);

    return status;
}
