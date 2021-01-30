#ifndef __WORKER_WRAPPER_H_
#define __WORKER_WRAPPER_H_

#include "core/worker.h"
#include "addr_list_wrapper.h"

struct worker_wrapper
{
    struct worker wk;
    struct addr_list_wrapper alw;
};

struct worker_wrapper wkw_create(queue_imp_t imp_qu1, queue_imp_t imp_qu2, uint32_t qu_capacity);
void wkw_destroy(struct worker_wrapper *wkw);

int wkw_model_run(struct worker_wrapper *wkw, uint32_t requests_amount, worker_interm_state_t *state);

#endif
