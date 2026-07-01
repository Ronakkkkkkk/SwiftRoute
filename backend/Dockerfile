FROM gcc:13 AS build

WORKDIR /app
COPY . .

RUN g++ -std=c++17 -Wall -Iinclude src/server.cpp src/Graph.cpp src/Dispatcher.cpp -o swiftroute_server

FROM debian:bookworm-slim

WORKDIR /app
COPY --from=build /app/swiftroute_server .

EXPOSE 8080
CMD ["./swiftroute_server"]