CXX = g++
CXXFLAGS = -std=c++17 -Wall -Iinclude

CORE_SRC = src/Graph.cpp src/Dispatcher.cpp

# CLI version — good for quickly testing the algorithm
cli: src/main.cpp $(CORE_SRC)
	$(CXX) $(CXXFLAGS) src/main.cpp $(CORE_SRC) -o swiftroute_cli

run-cli: cli
	./swiftroute_cli

# Server version — what the frontend talks to
server: src/server.cpp $(CORE_SRC)
	$(CXX) $(CXXFLAGS) src/server.cpp $(CORE_SRC) -o swiftroute_server -lws2_32

run-server: server
	./swiftroute_server

# Benchmark — compares naive vs optimized dispatch strategies
benchmark: src/benchmark.cpp $(CORE_SRC)
	$(CXX) $(CXXFLAGS) src/benchmark.cpp $(CORE_SRC) -o swiftroute_benchmark

run-benchmark: benchmark
	./swiftroute_benchmark

clean:
	rm -f swiftroute_cli swiftroute_server swiftroute_benchmark