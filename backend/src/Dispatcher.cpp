#include "Dispatcher.h"
#include <iostream>
#include <climits>

void Dispatcher::releaseFinishedRiders(std::vector<Rider>& riders, int currentTime) {
    for (auto& rider : riders) {
        if (!rider.available && rider.busyUntil <= currentTime) {
            rider.available = true;
        }
    }
}

void Dispatcher::assignOrder(
    Graph& graph,
    std::vector<Rider>& riders,
    const Order& order,
    int currentTime
) {
    // Before assigning, check if anyone finished their previous delivery
    releaseFinishedRiders(riders, currentTime);

    int bestRiderIndex = -1;
    int bestDist = INT_MAX;

    for (size_t i = 0; i < riders.size(); ++i) {
        if (!riders[i].available) continue;

        auto [dist, path] = graph.shortestPath(riders[i].currentNode, order.restaurantNode);
        if (dist < bestDist) {
            bestDist = dist;
            bestRiderIndex = static_cast<int>(i);
        }
    }

    if (bestRiderIndex == -1) {
        std::cout << "[t=" << currentTime << "] No available riders for order " << order.id << "\n";
        return;
    }

    Rider& rider = riders[bestRiderIndex];
    auto [distToCustomer, pathToCustomer] = graph.shortestPath(order.restaurantNode, order.customerNode);

    int totalTravelTime = bestDist + distToCustomer; // treating distance as time units for simplicity

    rider.available = false;
    rider.currentNode = order.customerNode;
    rider.busyUntil = currentTime + totalTravelTime; // rider is busy until delivery completes

    std::cout << "[t=" << currentTime << "] Order " << order.id << " assigned to Rider " << rider.id
              << " | pickup: " << bestDist
              << ", delivery: " << distToCustomer
              << ", total: " << totalTravelTime
              << ", free again at t=" << rider.busyUntil << "\n";
}