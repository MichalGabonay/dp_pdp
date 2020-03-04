#ifndef VRP_HELPERS_H
#define VRP_HELPERS_H 1

#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <map>
#include <iostream>
#include "task.h"

typedef unsigned int UINT;
typedef int BOOL;

typedef struct {
    std::vector<UINT> locations;
    std::vector<int> utilization;
    std::vector<double> cuml_duration;
    std::vector<double> cost_in;
    std::vector<double> cost_out;
    UINT route_length;
    UINT cost;
    double duration;
    UINT distance;
} Route;

// definice typu chromozomu GA
typedef struct {
    double fitness;   // fitness daneho jedince
    double cost;    // fitness daneho jedince
    BOOL evaluate;  // ci je treba znovu vyhodnotit fitness
    std::vector<Route> routes;
    std::vector<UINT> map_route_position;
    std::vector<UINT> map_customer_in_vehicle;
} GA_chromosome;

UINT urandom(UINT low, UINT high);
void swapArrayValues(std::vector<UINT> *locations, UINT position1, UINT position2);
void swapNeighborsInRoute (GA_chromosome *g, UINT vehicle, UINT vehicle_capacity, std::vector<int> demands);
void swapLocations (GA_chromosome *g, UINT vehicle, UINT vehicle_capacity, std::vector<int> demands, Task* task);
UINT insertToRoute (GA_chromosome *g, UINT vehicle, UINT index, UINT value, int demand, Task* task);
void deleteFromRoute (GA_chromosome *g, UINT vehicle, UINT index, int demand, Task* task);
int selectRoute(GA_chromosome *genome, int number_of_vehicles);
void validateAndFixRoute(GA_chromosome *g, UINT vehicle, UINT vehicle_capacity, std::vector<int> demands, Task* task);
void printRoute(Route route, int vehicle_index);
void recalculateRoute (GA_chromosome *g, UINT vehicle, Task* task);
#endif