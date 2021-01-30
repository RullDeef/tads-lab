#include <stdlib.h>
#include <stdbool.h>
#include "utils/logger.h"
#include "callocs.h"
#include "graph/graph.h"

#define MEM_POOL_SIZE 10240UL

void output_graph(road_graph_t graph)
{
    rg_output(graph, "graph.txt");
    system("dot -Tpng -o graph.png graph.txt");
}

int main(void)
{
    log_init("@stdout");
    callocs_init(MEM_POOL_SIZE);

    char file_name[64];
    printf("Введите имя файла с данными: ");
    scanf("%s", file_name);
    road_graph_t graph = rg_load(file_name);

    if (graph != NULL)
    {
        output_graph(graph);

        char center_name[128];
        int distance;
        printf("Введите название центрального города для расчёта и минимальное расстояние через пробел:\n");
        while (scanf("%s %d", center_name, &distance) != 2)
            printf("Неверный ввод. Повторите попытку.\n");

        rg_pick_farther(graph, center_name, distance);

        output_graph(graph);
        rg_destroy(&graph);
    }

    log_info("Пул памяти на момент окончания программы:");
    callocs_fprintf_heap_info(stdout);
    callocs_destroy();
    log_destroy();
    return 0;
}
