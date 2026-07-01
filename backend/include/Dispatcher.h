#ifndef DISPATCHER_H
#define DISPATCHER_H

#include "Graph.h"
#include "Rider.h"
#include "Order.h"
#include <vector>

class Dispatcher {
public:
    // Optimized: assigns the NEAREST available rider.
    // Returns total travel distance (pickup + delivery), or -1 if no rider available.
    static int assignOrder(
        Graph& graph,
        std::vector<Rider>& riders,
        const Order& order,
        int currentTime
    );

    static void releaseFinishedRiders(std::vector<Rider>& riders, int currentTime);

    // Naive baseline: assigns the FIRST available rider found,
    // ignoring distance entirely. Used only for benchmarking
    // against the optimized nearest-rider strategy.
    // Returns total travel distance (pickup + delivery), or -1 if no rider available.
    static int assignOrderNaive(
        Graph& graph,
        std::vector<Rider>& riders,
        const Order& order,
        int currentTime
    );
};

#endif