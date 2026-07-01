#ifndef DISPATCHER_H
#define DISPATCHER_H

#include "Graph.h"
#include "Rider.h"
#include "Order.h"
#include <vector>

class Dispatcher {
public:
    // currentTime = simulated clock tick when this order comes in
    static void assignOrder(
        Graph& graph,
        std::vector<Rider>& riders,
        const Order& order,
        int currentTime
    );

    // Frees up any rider whose busyUntil has passed
    static void releaseFinishedRiders(std::vector<Rider>& riders, int currentTime);
};

#endif