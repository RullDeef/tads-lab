#include <string.h>
#include "utils/colors.h"
#include "utils/str_parser.h"
#include "menu.h"
#include "core/worker.h"

static int parse_imp_char(const char *str, queue_imp_t *type)
{
    int status = strlen(str) == 1 ? EXIT_SUCCESS : EXIT_FAILURE;

    if (status == EXIT_SUCCESS)
    {
        switch (str[0])
        {
            case 'a': case 'A': case '1':
                *type = QUEUE_ARRAY;
                break;
            case 'l': case 'L': case '2':
                *type = QUEUE_LINKED_LIST;
                break;
            default:
                status = EXIT_FAILURE;
                break;
        }
    }

    return status;
}

static void print_queue_stats(queue_stats_t *stats)
{
    // printf("    Суммарное время работы очереди:    %7.2f е.в. (%7llu мкс)\n",
    //    stats->total_push_time + stats->total_pop_time, stats->total_push_us + stats->total_pop_us);
    printf("    Суммарное время прихода заявок:      %7.2f е.в. (%7llu мкс)\n",
        stats->total_push_time, stats->total_push_us);
    printf("    Суммарное время обработки заявок:    %7.2f е.в. (%7llu мкс)\n",
        stats->total_pop_time, stats->total_pop_us);
    printf("    Среднее время прихода заявки:        %7.2f е.в. (%7u нс )\n",
        stats->avg_push_time, stats->avg_push_ns);
    printf("    Среднее время обработки заявки:      %7.2f е.в. (%7u нс )\n",
        stats->avg_pop_time, stats->avg_pop_ns);
    printf("    Среднее время пребывания в очереди:  %7.2f е.в.\n", stats->avg_wait_time);
    printf("    Число заявок в очереди сейчас:       %7u\n", stats->curr_size);
    printf("    Максимальное число заявок в очереди: %7u\n", stats->max_size);

#if defined(FUNC_TEST) && defined(FUNC_TEST_FNAME)
    FILE *out_file = fopen(FUNC_TEST_FNAME, "at");

    fprintf(out_file, "%.2f %.2f %.2f %.2f %.2f %u %u\n",
            stats->total_push_time, stats->total_pop_time, stats->avg_push_time,
            stats->avg_pop_time, stats->avg_wait_time, stats->curr_size, stats->max_size);

    fclose(out_file);
#endif
}

static float _time_mid(time_interval_t interval)
{
    return 0.5f * (interval.begin + interval.end);
}

static void print_theor_stats(struct worker *wk, uint32_t requests_1)
{
    float time_in = _time_mid(wk->params.t_in1);
    float time_out = _time_mid(wk->params.t_out1);
    float full_time_in = (requests_1 - 1U) * time_in;
    float full_time_out = (requests_1 - 1U) * time_out;
    float full_time = time_in + (full_time_in > full_time_out ? full_time_in : full_time_out) + time_out;

    printf("\n  " CLR_BR_YELLOW_U "Теоритические рассчёты:" CLR_RESET "\n");
    printf("    Среднее полное время обработки заявок: %.2f е.в.\n", full_time);

#if defined(FUNC_TEST) && defined(FUNC_TEST_FNAME)
    FILE *out_file = fopen(FUNC_TEST_FNAME, "at");

    fprintf(out_file, "%.2f\n", full_time);

    fclose(out_file);
#endif
}

static void print_worker_stats(struct worker *wk)
{
    printf("\n  " CLR_BR_MAGENTA_U "Статистика первой очереди:" CLR_RESET "\n");
    print_queue_stats(&wk->stats.qu1);
    printf("\n  " CLR_BR_MAGENTA_U "Статистика второй очереди:" CLR_RESET "\n");
    print_queue_stats(&wk->stats.qu2);
    printf("\n  " CLR_BR_MAGENTA_U "Статистика обслуживающего аппарата:" CLR_RESET "\n");
    printf("    Полное время обработки заявок:       %.2f е.в.\n", wk->stats.time);
    printf("    Всего поступило заявок:              %u\n", wk->stats.request_index);
    printf("      Первого типа:                      %u\n", wk->stats.requests_1);
    printf("      Второго типа:                      %u\n", wk->stats.requests_2);
    printf("    Отклонено заявок из 2 очереди:       %u\n", wk->stats.request_dismissed);

#if defined(FUNC_TEST) && defined(FUNC_TEST_FNAME)
    FILE *out_file = fopen(FUNC_TEST_FNAME, "at");

    fprintf(out_file, "%.2f %u %u %u %u\n",
            wk->stats.time, wk->stats.request_index, wk->stats.requests_1,
            wk->stats.requests_2, wk->stats.request_dismissed);

    fclose(out_file);
#endif
}

int menu_run(cmdf_arglist *arglist)
{
    queue_imp_t qu1_imp;
    queue_imp_t qu2_imp;
    int32_t N = 1000, M = 100;

    if (!arglist || !(arglist->count >= 2 &&
        !parse_imp_char(arglist->args[0], &qu1_imp) &&
        !parse_imp_char(arglist->args[1], &qu2_imp) &&
        (
            arglist->count == 2 ||
            (arglist->count >= 3 && !parse_int32(arglist->args[2], &N) && 0 < N && N <= 1000 &&
                ((arglist->count == 3 && (M = N, true)) || (arglist->count == 4 && !parse_int32(arglist->args[3], &M) && 0 < M && M <= N))
            )
        )))
    {
        printf("Использование: " CLR_EMPH "run <очередь 1> <очередь 2> [N [M]]" CLR_RESET ", где\n");
        printf("    очередь 1 - реализация очереди высокого приоритета: "
            CLR_BR_GREEN_U "A" CLR_RESET " (массив) или " CLR_BR_GREEN_U "L" CLR_RESET " (список),\n");
        printf("    очередь 2 - реализация очереди низкого приоритета: "
            CLR_BR_GREEN_U "A" CLR_RESET " (массив) или " CLR_BR_GREEN_U "L" CLR_RESET " (список).\n");
        printf("    N - максимальное число обрабатываемых заявок первого типа в интервале [1; 1000]. 1000 по-умолчанию.\n");
        printf("    M - шаг для вывода промежуточной информации об ОА в интервале [1; N]. 100 по-умолчанию (или равно N, если N введено).\n");
    }
    else
    {
        uint32_t requests_count = (uint32_t)N;
        uint32_t requests_step = (uint32_t)M;
        uint32_t passed_requests = 0U;

        printf("Запуск моделирования для числа заявок первого типа: " CLR_GREEN "%u" CLR_RESET " и шага " CLR_GREEN "%u" CLR_RESET "\n", requests_count, requests_step);

        struct worker wk = wk_create(qu1_imp, qu2_imp, requests_count);
        wk.params = wk_params;

        while (passed_requests < requests_count)
        {
            int status = wk_model_run(&wk, requests_step);
            if (status != EXIT_SUCCESS)
            {
                printf("При моделировании возникли серьёзные неисправности!\n");
                break;
            }
            passed_requests += requests_step;

            print_worker_stats(&wk);
            print_theor_stats(&wk, passed_requests);

            printf(CLR_BLUE "  Нажмите Enter для продолжения..." CLR_RESET "\n");
            getchar();
        }

        wk_destroy(&wk);
    }

    return EXIT_SUCCESS;
}
