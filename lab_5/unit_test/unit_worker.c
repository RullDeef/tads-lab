#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "unit_worker.h"

#define MAX_CAPACITY 100U
#define EPS 1e-6
#define RUN_TEST(test_fun)              \
    if (test_fun())                     \
    {                                   \
        printf(#test_fun " failed.\n"); \
        status = EXIT_FAILURE;          \
    }                                   \
    else                                \
        printf(#test_fun " passed.\n")

static worker_stats_t test_params(uint32_t requests, float p1, float p2, float p3, float p4)
{
    struct worker wk = wk_create(QUEUE_ARRAY, QUEUE_ARRAY, MAX_CAPACITY);

    wk.params.t_in1.begin = p1;
    wk.params.t_in1.end = p1;

    wk.params.t_in2.begin = p2;
    wk.params.t_in2.end = p2;

    wk.params.t_out1.begin = p3;
    wk.params.t_out1.end = p3;

    wk.params.t_out2.begin = p4;
    wk.params.t_out2.end = p4;

    wk_model_run(&wk, requests);
    worker_stats_t result = wk.stats;
    wk_destroy(&wk);

    return result;
}

static int test_1_dismiss(void)
{
    worker_stats_t stats = test_params(1U, 3.0f, 2.0f, 2.0f, 1.0f);
    float expect = 5.0f;
    printf("expect %f, got %f\n", expect, stats.time);
    return fabsf(stats.time - expect) < EPS ? EXIT_SUCCESS : EXIT_FAILURE;
}

static int test_3I_1II_1dis(void)
{
    worker_stats_t stats = test_params(3U, 2.0f, 4.0f, 1.0f, 1.0f);
    float expect = 7.0f;
    printf("expect %f, got %f\n", expect, stats.time);
    return fabsf(stats.time - expect) < EPS ? EXIT_SUCCESS : EXIT_FAILURE;
}

int test_worker(void)
{
    int status = EXIT_SUCCESS;

    RUN_TEST(test_1_dismiss);
    RUN_TEST(test_3I_1II_1dis);

    return status;
}
