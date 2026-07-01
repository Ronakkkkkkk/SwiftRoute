#include "httplib.h"
#include "Graph.h"
#include "Rider.h"
#include "Order.h"
#include "Dispatcher.h"
#include <iostream>
#include <vector>
#include <sstream>

// Very small hand-rolled JSON builder — no external JSON lib needed
// for a project this size. (For anything bigger, you'd reach for
// nlohmann/json, but that's overkill here.)

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

    std::vector<Rider> riders = {
        {1, 0, true, 0},
        {2, 4, true, 0},
        {3, 6, true, 0},
        {4, 1, true, 0},
        {5, 3, true, 0},
        {6, 2, true, 0}
    };

    int simulatedTime = 0;
    int nextOrderId = 101;

    httplib::Server svr;

    // Allow the React dev server (different port) to call this API
    svr.set_default_headers({
        {"Access-Control-Allow-Origin", "*"},
        {"Access-Control-Allow-Methods", "GET, POST, OPTIONS"},
        {"Access-Control-Allow-Headers", "Content-Type"}
    });

    // GET /riders — see current rider positions/status
    svr.Get("/riders", [&](const httplib::Request&, httplib::Response& res) {
        std::ostringstream json;
        json << "[";
        for (size_t i = 0; i < riders.size(); ++i) {
            json << "{\"id\":" << riders[i].id
                 << ",\"currentNode\":" << riders[i].currentNode
                 << ",\"available\":" << (riders[i].available ? "true" : "false")
                 << "}";
            if (i + 1 < riders.size()) json << ",";
        }
        json << "]";
        res.set_content(json.str(), "application/json");
    });

    // POST /order  body: {"restaurantNode": 2, "customerNode": 5}
    svr.Post("/order", [&](const httplib::Request& req, httplib::Response& res) {
        // Minimal manual parsing — fine for two known integer fields.
        int restaurantNode = -1, customerNode = -1;
        std::string body = req.body;

        auto extractInt = [&](const std::string& key) -> int {
            auto pos = body.find("\"" + key + "\"");
            if (pos == std::string::npos) return -1;
            pos = body.find(":", pos);
            return std::stoi(body.substr(pos + 1));
        };

        restaurantNode = extractInt("restaurantNode");
        customerNode = extractInt("customerNode");

        if (restaurantNode == -1 || customerNode == -1) {
            res.status = 400;
            res.set_content("{\"error\":\"missing restaurantNode or customerNode\"}", "application/json");
            return;
        }

        Order order{nextOrderId++, restaurantNode, customerNode};
        Dispatcher::releaseFinishedRiders(riders, simulatedTime);
        Dispatcher::assignOrder(cityMap, riders, order, simulatedTime);
        simulatedTime += 1; // advance clock a bit per order for demo purposes

        std::ostringstream json;
        json << "{\"orderId\":" << order.id
             << ",\"status\":\"assigned\","
             << "\"time\":" << simulatedTime << "}";
        res.set_content(json.str(), "application/json");
    });

    std::cout << "SwiftRoute server running on http://localhost:8080\n";
    // Handle CORS preflight requests
    svr.Options(R"(/.*)", [](const httplib::Request&, httplib::Response& res) {
        res.status = 200;
    });
    svr.listen("0.0.0.0", 8080);

    return 0;
}