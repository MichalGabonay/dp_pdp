#ifndef VRP_HELPERS_H
#define VRP_HELPERS_H 1

#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <map>
#include <iostream>
#include "task.h"
#include <random>
#include<set>
#include <math.h>

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
    std::pair<int, int> centroid;
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
} Chromosome;

UINT urandom(UINT low, UINT high);
void swapArrayValues(std::vector<UINT> *locations, UINT position1, UINT position2);
std::pair<int, int> calculateCentroid(std::set<std::pair<int, int>> setOfCords);
double centroidDiff(std::pair<int, int> centroid, std::pair<int, int> coord);

void swapLocations (Chromosome *g, UINT vehicle, UINT vehicle_capacity, std::vector<int> demands, Task* task);
UINT insertToRoute (Chromosome *g, UINT vehicle, UINT index, UINT value, int demand, Task* task);
void deleteFromRoute (Chromosome *g, UINT vehicle, UINT index);
int selectRoute(Chromosome *genome, int number_of_vehicles);
void validateAndFixRoute(Chromosome *g, UINT vehicle, UINT vehicle_capacity, std::vector<int> demands, Task* task);
void recalculateRoute (Chromosome *g, UINT vehicle, Task* task);
int selectRouteByWeight(Chromosome *g);
void inserCustomerToRoute(Chromosome *g, int vehicle, UINT pickup, Task *task);
void realocateCustomerInRoute(Chromosome *g, int vehicle, UINT pickup, Task *task);
int selectRouteByCentroid(Chromosome *g, UINT pickup, Task* task);

void printRoute(Route route, int vehicle_index);
int selectLocationByCost(Route *route);
int selectRandomCustomer(Route *route);
int selectCustomerByCost(Route *route, std::vector<UINT> map_route_position);
int selectCustomerByCentroid(Route *route, Task* task);

#endif