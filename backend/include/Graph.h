#ifndef GRAPH_H
#define GRAPH_H

#include <vector>
#include <unordered_map>
#include <climits>

class Graph {
public:
    // adjacency list: node -> list of (neighbor, weight)
    std::unordered_map<int, std::vector<std::pair<int, int>>> adjList;

    void addEdge(int u, int v, int weight);

    // Returns {distance, path} from start to end using Dijkstra
    std::pair<int, std::vector<int>> shortestPath(int start, int end);
};

#endif