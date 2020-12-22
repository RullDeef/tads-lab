#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "utils/logger.h"
#include "str_stack/str_stack.h"
#include "str_queue/str_queue.h"
#include "graph.h"

#define CALLOCS_REDEFINE
#include "callocs.h"

struct node;

struct edge
{
    struct edge *next;
    struct node *node_from;
    struct node *node_to;

    int weight;
    bool picked;
};

struct node
{
    struct node *next;
    struct edge *edges;

    const char *name;
    int distance;
    bool picked;
};

struct road_graph
{
    struct node *nodes;
    struct node *center;
};

static struct node *get_node(const road_graph_t graph, const char *name)
{
    for (struct node *node = graph->nodes; node != NULL; node = node->next)
        if (strcmp(name, node->name) == 0)
            return node;
    return NULL;
}

static struct node *insert_node(road_graph_t graph, const char *name)
{
    struct node *res = get_node(graph, name);

    if (res == NULL)
    {
        res = malloc(sizeof(struct node) + strlen(name) + 1UL);
        if (res == NULL)
            log_error("insert_node. res == null. abort(");
        res->name = (char *)(res + 1);
        strcpy((char *)res->name, name);
        res->edges = NULL;
        res->distance = 0;
        res->picked = false;
        res->next = graph->nodes;
        graph->nodes = res;
    }

    return res;
}

static void connect_nodes(struct node *node_from, struct node *node_to, int weight)
{
    struct edge *new_edge = malloc(sizeof(struct edge));
    if (new_edge == NULL)
        log_error("bad new edge! connect_nodes");

    new_edge->node_from = node_from;
    new_edge->node_to = node_to;
    new_edge->weight = weight;
    new_edge->next = node_from->edges;
    new_edge->picked = true;

    node_from->edges = new_edge;
}

static void pick_edge(struct node *node_from, struct node *node_to, bool pick)
{
    struct edge *edge = node_from->edges;
    while (edge->node_to != node_to)
    {
        edge = edge->next;
        if (edge == NULL)
            log_error("NULL edge as pick_edge");
    }

    edge->picked = pick;

    edge = node_to->edges;
    while (edge->node_to != node_from)
    {
        edge = edge->next;
        if (edge == NULL)
            log_error("NULL edge2 as pick_edge");
    }

    edge->picked = pick;
}

road_graph_t rg_load(const char *filename)
{
    FILE *file = fopen(filename, "rt");
    if (file == NULL)
    {
        log_error("не удалось открыть файл для чтения");
        return NULL;
    }

    road_graph_t graph = malloc(sizeof(struct road_graph));
    graph->nodes = NULL;
    graph->center = NULL;

    bool bad_file = false;
    char buffer[128];
    int line_number = 0;
    while (fgets(buffer, 128, file) != NULL && !feof(file))
    {
        line_number++;

        char city_from[64];
        char city_to[64];
        int weight;
        if (sscanf(buffer, "%s %s %d", city_from, city_to, &weight) != 3)
        {
            log_error("Некорректный файл! строка %d.", line_number);
            bad_file = true;
        }
        else if (weight <= 0)
        {
            log_error("вес %d недопустим (строка %d). Используйте положительные числа.", weight, line_number);
            bad_file = true;
        }
        else
        {
            struct node *node_from = insert_node(graph, city_from);
            struct node *node_to = insert_node(graph, city_to);
            connect_nodes(node_from, node_to, weight);
            connect_nodes(node_to, node_from, weight);
        }
    }

    fclose(file);

    if (bad_file)
        rg_destroy(&graph);
    return graph;
}

void rg_clear_picked_edges(road_graph_t graph)
{
    for (struct node *node = graph->nodes; node != NULL; node = node->next)
        for (struct edge *edge = node->edges; edge != NULL; edge = edge->next)
            edge->picked = false;
}

void rg_clear_node_distances(road_graph_t graph)
{
    for (struct node *node = graph->nodes; node != NULL; node = node->next)
        node->distance = -1;
}

void rg_calc_distances(road_graph_t graph)
{
    rg_clear_picked_edges(graph);
    rg_clear_node_distances(graph);

    graph->center->distance = 0;

    str_queue_t visited = squ_create();

    struct node *curr_node = graph->center;
    while (curr_node != NULL)
    {
        for (struct edge *edge = curr_node->edges; edge != NULL; edge = edge->next)
        {
            if (edge->node_to->distance == -1 || edge->node_to->distance > curr_node->distance + edge->weight)
            {
                edge->node_to->distance = curr_node->distance + edge->weight;
                squ_insert(&visited, edge->node_to->name);
            }
        }

        char next_node_name[128];
        if (squ_pop(&visited, next_node_name, 128) != NULL)
            curr_node = get_node(graph, next_node_name);
        else
            curr_node = NULL;
    }

    squ_destroy(&visited);
    visited = squ_create();

    curr_node = graph->center;
    while (curr_node != NULL)
    {
        for (struct edge *edge = curr_node->edges; edge != NULL; edge = edge->next)
        {
            if (edge->node_to->distance == curr_node->distance + edge->weight)
            {
                pick_edge(curr_node, edge->node_to, true);
                squ_insert(&visited, edge->node_to->name);
            }
        }

        char next_node_name[128];
        if (squ_pop(&visited, next_node_name, 128) != NULL)
            curr_node = get_node(graph, next_node_name);
        else
            curr_node = NULL;
    }

    squ_destroy(&visited);
}

void rg_pick_farther(road_graph_t graph, const char *center, int distance)
{
    struct node *center_node = get_node(graph, center);

    if (center_node == NULL)
    {
        log_error("неверное название целевого города.");
        return;
    }

    graph->center = center_node;
    rg_calc_distances(graph);

    for (struct node *node = graph->nodes; node != NULL; node = node->next)
        node->picked = node->distance > distance || node->distance == -1;
}

void rg_output(road_graph_t graph, const char *filename)
{
    FILE *file = fopen(filename, "wt");
    if (file == NULL)
    {
        log_error("bad output file: \"%s\"", filename);
        return;
    }

    fprintf(file, "strict graph {\n");
    fprintf(file, "    overlap = false\n");

    for (struct node *node = graph->nodes; node != NULL; node = node->next)
    {
        if (node == graph->center)
            fprintf(file, "    \"%s\"[color=green, label=<%s<BR/><FONT POINT-SIZE=\"10\">%d</FONT>>]\n",
                node->name, node->name, node->distance);
        else if (node->picked)
            fprintf(file, "    \"%s\"[color=red, label=<%s<BR/><FONT POINT-SIZE=\"10\">%d</FONT>>]\n",
                node->name, node->name, node->distance);
        else
            fprintf(file, "    \"%s\"[label=<%s<BR/><FONT POINT-SIZE=\"10\">%d</FONT>>]\n",
                node->name, node->name, node->distance);

        for (struct edge *edge = node->edges; edge != NULL; edge = edge->next)
        if (edge->node_from < edge->node_to)
        {
            fprintf(file, "    \"%s\" -- \"%s\" ", edge->node_from->name, edge->node_to->name);
            fprintf(file, "[label=\"%d\", style=\"%s\", weight=%d]",
                edge->weight, (edge->picked ? "solid" : "dotted"), edge->weight);
            fprintf(file, "\n");
        }
    }

    fprintf(file, "}\n");
    fclose(file);
}

void rg_destroy(road_graph_t *graph)
{
    struct node *node = (*graph)->nodes;

    while (node != NULL)
    {
        while (node->edges != NULL)
        {
            struct edge *next_edge = node->edges->next;
            free(node->edges);
            node->edges = next_edge;
        }

        struct node *next_node = node->next;
        free(node);
        node = next_node;
    }

    free(*graph);
    *graph = NULL;
}
