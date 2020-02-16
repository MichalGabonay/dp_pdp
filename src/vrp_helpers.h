#include <stdio.h>
#include <stdlib.h>
#include <vector>

typedef unsigned int UINT;
typedef int BOOL;

typedef struct {
    std::vector<UINT> locations;
    std::vector<int> utilization;
    UINT route_length;
    UINT cost;
    UINT duration;
    UINT distance;
} Route;

// definice typu chromozomu GA
typedef struct {
    double fitness;   // fitness daneho jedince
    double cost;    // fitness daneho jedince
    BOOL evaluate;  // ci je treba znovu vyhodnotit fitness
    std::vector<Route> routes;
    std::vector<UINT> map_route_position;
} GA_chromosome;

UINT urandom(UINT low, UINT high);
void swapArrayValues(std::vector<UINT> *locations, UINT position1, UINT position2);
void swapNeighborsInRoute (GA_chromosome *g, UINT vehicle, UINT vehicle_capacity, std::vector<int> demands);
UINT insertToRoute (GA_chromosome *g, UINT vehicle, UINT index, UINT value, int demand, UINT vehicle_capacity);
void deleteFromRoute (GA_chromosome *g, UINT vehicle, UINT index, int demand);