#include <string.h>
#include "utils/colors.h"
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
    printf("    Суммарное время работы очереди:    %7.2f е.в. (%7llu мкс)\n",
        stats->total_push_time + stats->total_pop_time, stats->total_push_us + stats->total_pop_us);
    printf("    Суммарное время добавления заявок: %7.2f е.в. (%7llu мкс)\n",
        stats->total_push_time, stats->total_push_us);
    printf("    Суммарное время удаления заявок:   %7.2f е.в. (%7llu мкс)\n",
        stats->total_pop_time, stats->total_pop_us);
    printf("    Среднее время добавления заявки:   %7.2f е.в. (%7u нс)\n",
        stats->avg_pop_time, stats->avg_push_ns);
    printf("    Среднее время удаления заявки:     %7.2f е.в. (%7u нс)\n",
        stats->avg_pop_time, stats->avg_pop_ns);
}

static void print_worker_stats(struct worker *wk)
{
    printf("\n  " CLR_BR_MAGENTA_U "Статистика первой очереди:" CLR_RESET "\n");
    print_queue_stats(&wk->stats.qu1);
    printf("\n  " CLR_BR_MAGENTA_U "Статистика второй очереди:" CLR_RESET "\n");
    print_queue_stats(&wk->stats.qu2);
    printf("\n  " CLR_BR_MAGENTA_U "Статистика обслуживающего аппарата:" CLR_RESET "\n");
    printf("    Полное время обработки заявок:     %.2f е.в.\n", wk->stats.time);
    printf("    Всего поступило заявок:            %u\n", wk->stats.request_index);
    printf("    Отклонено заявок из 2 очереди:     %u\n", wk->stats.request_dismissed);
}

int menu_run(cmdf_arglist *arglist)
{
    queue_imp_t qu1_imp;
    queue_imp_t qu2_imp;

    if (!arglist || arglist->count != 2 ||
        parse_imp_char(arglist->args[0], &qu1_imp) ||
        parse_imp_char(arglist->args[1], &qu2_imp))
    {
        printf("Использование: " CLR_EMPH "run [очередь 1] [очередь 2]" CLR_RESET ", где\n");
        printf("    очередь 1 - реализация очереди высокого приоритета: "
            CLR_BR_GREEN_U "A" CLR_RESET " (массив) или " CLR_BR_GREEN_U "L" CLR_RESET " (список),\n");
        printf("    очередь 2 - реализация очереди низкого приоритета: "
            CLR_BR_GREEN_U "A" CLR_RESET " (массив) или " CLR_BR_GREEN_U "L" CLR_RESET " (список).\n");
    }
    else
    {
        struct worker wk = wk_create(qu1_imp, qu2_imp, 1000U);
        wk.params = wk_params;

        int status = wk_model_run(&wk, 1000U);
        if (status != EXIT_SUCCESS)
            printf("При моделировании возникли серьёзные неисправности!\n");
        
        print_worker_stats(&wk);
        wk_destroy(&wk);
    }

    return EXIT_SUCCESS;
}
