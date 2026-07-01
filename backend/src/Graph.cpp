#include "Graph.h"
#include <queue>
#include <algorithm>

void Graph::addEdge(int u, int v, int weight) {
    // undirected: roads work both ways
    adjList[u].push_back({v, weight});
    adjList[v].push_back({u, weight});
}

std::pair<int, std::vector<int>> Graph::shortestPath(int start, int end) {
    std::unordered_map<int, int> dist;
    std::unordered_map<int, int> parent; // to reconstruct the path

    // min-heap: {distance, node} — smallest distance popped first
    std::priority_queue<
        std::pair<int, int>,
        std::vector<std::pair<int, int>>,
        std::greater<>
    > pq;

    dist[start] = 0;
    pq.push({0, start});

    while (!pq.empty()) {
        auto [d, node] = pq.top();
        pq.pop();

        if (d > dist[node]) continue; // stale entry, skip

        for (auto& [neighbor, weight] : adjList[node]) {
            int newDist = d + weight;
            if (!dist.count(neighbor) || newDist < dist[neighbor]) {
                dist[neighbor] = newDist;
                parent[neighbor] = node;
                pq.push({newDist, neighbor});
            }
        }
    }

    if (!dist.count(end)) {
        return {INT_MAX, {}}; // unreachable
    }

    // reconstruct path by walking parent[] backwards
    std::vector<int> path;
    int curr = end;
    while (curr != start) {
        path.push_back(curr);
        curr = parent[curr];
    }
    path.push_back(start);
    std::reverse(path.begin(), path.end());

    return {dist[end], path};
}