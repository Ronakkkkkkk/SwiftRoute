# SwiftRoute — Real-Time Delivery Routing & Dispatch Engine

A simulation of a food-delivery dispatch system (Zomato/Swiggy-style), built to explore how core data structures and algorithms solve real routing and assignment problems — not just as a CRUD clone, but as a small systems project focused on the actual hard part: **matching riders to orders efficiently as a city-scale graph problem.**

---

## The Problem

Given a city road network, a fleet of riders scattered across it, and a live stream of incoming orders (each with a restaurant pickup point and a customer drop-off point), assign each order to the best available rider and compute the fastest route — in real time, without recomputing everything from scratch on every request.

## Architecture

```
┌─────────────────────┐        HTTP / JSON        ┌──────────────────────┐
│   Frontend (JS)     │  ─────────────────────►   │   Backend (C++)      │
│   Leaflet.js map    │  ◄─────────────────────   │   cpp-httplib serve  │
│   Polls /riders     │                           │   Graph + Dispatcher │
│   Posts to /orde    │                           │   Dijkstra + Heaps   │
└─────────────────────┘                           └──────────────────────┘
```

- **Backend**: C++17, exposes a small REST API (`cpp-httplib`) around a graph-based routing/dispatch engine.
- **Frontend**: Plain HTML/CSS/JS + Leaflet.js, renders the city graph on a real map and polls the backend for live rider state.

## Data Structures & Why Each One Earns Its Place

| Component | Data Structure / Algorithm | Why not brute force |
|---|---|---|
| City road network | Weighted graph (adjacency list, `unordered_map<int, vector<pair<int,int>>>`) | Real road networks aren't fully connected — an adjacency list models this accurately and stays memory-efficient as the map grows |
| Shortest path | Dijkstra's algorithm with a binary min-heap (`priority_queue`) | O((V+E) log V) instead of O(V²) — matters once the graph scales past a handful of nodes |
| Rider selection | Linear scan + min comparison (heap-ready for scale) | Picks the nearest *available* rider per order; structured so it can be swapped for a per-zone min-heap without touching the rest of the system |
| Rider availability | Time-based state (`busyUntil` tick) | Riders become available again after their simulated delivery completes, instead of being permanently consumed by one order |
| API responses | Hand-rolled minimal JSON | No external JSON library needed for two integer fields — kept dependencies minimal on purpose |

## Tech Stack

- **C++17** — core engine (Graph, Dijkstra, Dispatcher)
- **cpp-httplib** — single-header HTTP server, no heavyweight framework needed
- **Leaflet.js + OpenStreetMap** — map rendering, no API key required
- **Vanilla JS** — no build tooling, deliberately kept simple

## Project Structure

```
swiftroute/
├── backend/
│   ├── include/
│   │   ├── Graph.h
│   │   ├── Rider.h
│   │   ├── Order.h
│   │   ├── Dispatcher.h
│   │   └── httplib.h
│   ├── src/
│   │   ├── Graph.cpp
│   │   ├── Dispatcher.cpp
│   │   ├── main.cpp        # CLI test harness for the algorithm in isolation
│   │   └── server.cpp       # HTTP server exposing the engine as a REST API
│   └── Makefile
├── frontend/
│   ├── index.html
│   ├── style.css
│   └── script.js
└── README.md
```

## Running It Locally

**Backend:**
```bash
cd backend
mingw32-make run-server
# Server runs on http://localhost:8080
```

**Frontend:**
Open `frontend/index.html` directly, or serve it with VS Code's Live Server extension.
> The frontend expects the backend at `http://localhost:8080` — update `API_BASE` in `script.js` if you deploy the backend elsewhere.

**CLI test harness (optional, for testing the algorithm without the server):**
```bash
cd backend
mingw32-make run-cli
```

## API

| Method | Endpoint | Description |
|---|---|---|
| GET | `/riders` | Returns current position & availability of all riders |
| POST | `/order` | `{"restaurantNode": int, "customerNode": int}` → assigns the nearest available rider |

## Design Decisions & Tradeoffs

- **Synthetic graph instead of real street data**: the graph nodes are mapped to real coordinates for visualization, but edges are straight lines, not street-snapped routes. This was a deliberate scope decision — the point of the project is the DSA/routing logic, not integrating a full routing engine like OSRM.
- **No persistent order queue**: if no rider is available, the order is currently rejected rather than queued. This avoids unbounded backlog growth in the simulation, but a production system would need a proper queue with retry/backoff.
- **Simulated clock increments per request** rather than wall-clock time, to keep the demo deterministic and easy to reason about.

## Future Improvements

- Event-driven simulation using a global min-heap ordered by event timestamp (order arrival, delivery completion) instead of a simple counter
- LRU cache (hashmap + doubly linked list) for frequently requested restaurant→zone paths
- Per-zone rider indexing (hashmap of zone → min-heap of riders) to avoid scanning the full rider list per order
- Real street-routing via OSRM/GraphHopper integration
- Concurrency handling for simultaneous order submissions (mutex/lock-free structures around shared rider state)
- Benchmark suite comparing this approach against a naive nearest-rider linear scan at scale

## What This Project Demonstrates

- Graph modeling and Dijkstra's algorithm applied to a real-world-shaped problem, not a leetcode-style toy input
- Priority queues / heaps used because of genuine time-complexity constraints, not for their own sake
- A minimal, dependency-light REST API in C++ 
- Clear separation between algorithm logic (`Graph`, `Dispatcher`) and delivery mechanism (`server.cpp`, `main.cpp`) — the core engine doesn't know or care whether it's being called from a CLI or an HTTP request
