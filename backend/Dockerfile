FROM gcc:13

WORKDIR /app
COPY . .

RUN g++ -std=c++17 -Wall -Iinclude src/server.cpp src/Graph.cpp src/Dispatcher.cpp -o swiftroute_server

EXPOSE 8080
CMD ["./swiftroute_server"]