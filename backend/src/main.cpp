#include "Graph.h"
#include "Rider.h"
#include "Order.h"
#include "Dispatcher.h"
#include <iostream>
#include <vector>

int main() {
    Graph cityMap;
    cityMap.addEdge(0, 1, 4);
    cityMap.addEdge(0, 2, 2);
    cityMap.addEdge(1, 3, 5);
    cityMap.addEdge(2, 3, 8);
    cityMap.addEdge(2, 4, 10);
    cityMap.addEdge(3, 5, 6);
    cityMap.addEdge(4, 5, 2);
    cityMap.addEdge(5, 6, 3);

    // busyUntil = 0 means "free from the start"
    std::vector<Rider> riders = {
        {1, 0, true, 0},
        {2, 4, true, 0},
        {3, 6, true, 0},
        {4, 1, true, 0},
        {5, 3, true, 0},
        {6, 2, true, 0}
    };

    // Each order now has an arrival time, so riders can free up between orders
    struct TimedOrder {
        Order order;
        int arrivalTime;
    };

    std::vector<TimedOrder> incomingOrders = {
        {{101, 2, 5}, 0},
        {{102, 3, 6}, 5},
        {{103, 0, 4}, 20} // arrives later, gives earlier riders time to free up
    };

    std::cout << "--- SwiftRoute Dispatch Simulation ---\n";
    for (const auto& timedOrder : incomingOrders) {
        Dispatcher::assignOrder(cityMap, riders, timedOrder.order, timedOrder.arrivalTime);
    }

    return 0;
}