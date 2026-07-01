#ifndef RIDER_H
#define RIDER_H

struct Rider {
    int id;
    int currentNode;
    bool available;
    int busyUntil; // simulated time tick when this rider becomes free again
};

#endif