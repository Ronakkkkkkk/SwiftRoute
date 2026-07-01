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

int Dispatcher::assignOrder(
    Graph& graph,
    std::vector<Rider>& riders,
    const Order& order,
    int currentTime
) {
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
        return -1;
    }

    Rider& rider = riders[bestRiderIndex];
    auto [distToCustomer, pathToCustomer] = graph.shortestPath(order.restaurantNode, order.customerNode);

    int totalTravelTime = bestDist + distToCustomer;

    rider.available = false;
    rider.currentNode = order.customerNode;
    rider.busyUntil = currentTime + totalTravelTime;

    std::cout << "[t=" << currentTime << "] Order " << order.id << " assigned to Rider " << rider.id
              << " | pickup: " << bestDist
              << ", delivery: " << distToCustomer
              << ", total: " << totalTravelTime
              << ", free again at t=" << rider.busyUntil << "\n";

    return totalTravelTime;
}

int Dispatcher::assignOrderNaive(
    Graph& graph,
    std::vector<Rider>& riders,
    const Order& order,
    int currentTime
) {
    releaseFinishedRiders(riders, currentTime);

    // Just take the FIRST available rider, no distance comparison at all.
    int chosenIndex = -1;
    for (size_t i = 0; i < riders.size(); ++i) {
        if (riders[i].available) {
            chosenIndex = static_cast<int>(i);
            break;
        }
    }

    if (chosenIndex == -1) {
        return -1;
    }

    Rider& rider = riders[chosenIndex];
    auto [distToRestaurant, path1] = graph.shortestPath(rider.currentNode, order.restaurantNode);
    auto [distToCustomer, path2] = graph.shortestPath(order.restaurantNode, order.customerNode);

    int totalTravelTime = distToRestaurant + distToCustomer;

    rider.available = false;
    rider.currentNode = order.customerNode;
    rider.busyUntil = currentTime + totalTravelTime;

    return totalTravelTime;
}