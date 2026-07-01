#include "Graph.h"
#include "Rider.h"
#include "Order.h"
#include "Dispatcher.h"
#include <iostream>
#include <vector>
#include <random>
#include <numeric>

// Builds the same sample city graph used elsewhere in the project.
Graph buildCityGraph() {
    Graph cityMap;
    cityMap.addEdge(0, 1, 4);
    cityMap.addEdge(0, 2, 2);
    cityMap.addEdge(1, 3, 5);
    cityMap.addEdge(2, 3, 8);
    cityMap.addEdge(2, 4, 10);
    cityMap.addEdge(3, 5, 6);
    cityMap.addEdge(4, 5, 2);
    cityMap.addEdge(5, 6, 3);
    return cityMap;
}

std::vector<Rider> buildRiders() {
    // Larger fleet so both strategies can serve nearly all orders,
    // keeping the comparison apples-to-apples (avoids failure-rate bias).
    std::vector<Rider> riders;
    std::vector<int> startNodes = {0, 1, 2, 3, 4, 5, 6};
    int id = 1;
    for (int r = 0; r < 20; ++r) {
        riders.push_back({id++, startNodes[r % startNodes.size()], true, 0});
    }
    return riders;
}

int main() {
    const int NUM_ORDERS = 300;
    const std::vector<int> restaurantNodes = {2, 3, 4};
    const std::vector<int> customerNodes = {5, 6, 1};

    // Fixed seed so both strategies see the EXACT same sequence of orders —
    // this makes the comparison fair (same demand pattern, different dispatch logic).
    std::mt19937 rng(42);
    std::uniform_int_distribution<int> restaurantDist(0, restaurantNodes.size() - 1);
    std::uniform_int_distribution<int> customerDist(0, customerNodes.size() - 1);

    std::vector<Order> orders;
    for (int i = 0; i < NUM_ORDERS; ++i) {
        orders.push_back({
            100 + i,
            restaurantNodes[restaurantDist(rng)],
            customerNodes[customerDist(rng)]
        });
    }

    // --- Run naive strategy ---
    Graph graph1 = buildCityGraph();
    std::vector<Rider> riders1 = buildRiders();
    std::vector<int> naiveDistances;
    int naiveFailures = 0;

    for (int t = 0; t < NUM_ORDERS; ++t) {
        int result = Dispatcher::assignOrderNaive(graph1, riders1, orders[t], t);
        if (result == -1) naiveFailures++;
        else naiveDistances.push_back(result);
    }

    // --- Run optimized (nearest-rider) strategy ---
    // Suppress the per-order console spam from assignOrder for this run.
    Graph graph2 = buildCityGraph();
    std::vector<Rider> riders2 = buildRiders();
    std::vector<int> optimizedDistances;
    int optimizedFailures = 0;

    std::streambuf* oldCout = std::cout.rdbuf(nullptr); // silence assignOrder's internal prints
    for (int t = 0; t < NUM_ORDERS; ++t) {
        int result = Dispatcher::assignOrder(graph2, riders2, orders[t], t);
        if (result == -1) optimizedFailures++;
        else optimizedDistances.push_back(result);
    }
    std::cout.rdbuf(oldCout); // restore output

    // --- Compute stats ---
    auto average = [](const std::vector<int>& v) -> double {
        if (v.empty()) return 0.0;
        return static_cast<double>(std::accumulate(v.begin(), v.end(), 0)) / v.size();
    };

    double naiveAvg = average(naiveDistances);
    double optimizedAvg = average(optimizedDistances);
    double improvementPct = ((naiveAvg - optimizedAvg) / naiveAvg) * 100.0;

    std::cout << "\n===== SwiftRoute Dispatch Benchmark =====\n";
    std::cout << "Orders simulated: " << NUM_ORDERS << "\n\n";

    std::cout << "-- Naive (first available rider) --\n";
    std::cout << "  Successful assignments: " << naiveDistances.size() << "\n";
    std::cout << "  Failed (no rider available): " << naiveFailures << "\n";
    std::cout << "  Avg total travel distance: " << naiveAvg << "\n\n";

    std::cout << "-- Optimized (nearest available rider, Dijkstra) --\n";
    std::cout << "  Successful assignments: " << optimizedDistances.size() << "\n";
    std::cout << "  Failed (no rider available): " << optimizedFailures << "\n";
    std::cout << "  Avg total travel distance: " << optimizedAvg << "\n\n";

    std::cout << "Improvement: " << improvementPct << "% reduction in avg travel distance\n";

    return 0;
}