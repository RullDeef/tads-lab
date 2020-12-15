#ifndef __GRAPH_H_
#define __GRAPH_H_

typedef struct road_graph *road_graph_t;

road_graph_t rg_load(const char *filename);

void rg_clear_picked_edges(road_graph_t graph);
void rg_clear_node_distances(road_graph_t graph);

void rg_pick_farther(road_graph_t graph, const char *center, int distance);

void rg_output(road_graph_t graph, const char *filename);

void rg_destroy(road_graph_t *graph);

#endif // __GRAPH_H_
