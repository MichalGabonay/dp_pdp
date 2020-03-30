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
} GA_chromosome;

UINT urandom(UINT low, UINT high);
void swapArrayValues(std::vector<UINT> *locations, UINT position1, UINT position2);
void swapLocations (GA_chromosome *g, UINT vehicle, UINT vehicle_capacity, std::vector<int> demands, Task* task);
UINT insertToRoute (GA_chromosome *g, UINT vehicle, UINT index, UINT value, int demand, Task* task);
void deleteFromRoute (GA_chromosome *g, UINT vehicle, UINT index);
int selectRoute(GA_chromosome *genome, int number_of_vehicles);
void validateAndFixRoute(GA_chromosome *g, UINT vehicle, UINT vehicle_capacity, std::vector<int> demands, Task* task);
void printRoute(Route route, int vehicle_index);
void recalculateRoute (GA_chromosome *g, UINT vehicle, Task* task);
int selectRouteByWeight(GA_chromosome *g);
int selectLocationByCost(Route *route);
void inserCustomerToRoute(GA_chromosome *g, int vehicle, UINT pickup, Task *task);
int selectRandomCustomer(Route *route);
int selectCustomerByCost(Route *route, std::vector<UINT> map_route_position);
void realocateCustomerInRoute(GA_chromosome *g, int vehicle, UINT pickup, Task *task);
std::pair<int, int> calculateCentroid(std::set<std::pair<int, int>> setOfCords);
double centroidDiff(std::pair<int, int> centroid, std::pair<int, int> coord);
int selectRouteByCentroid(GA_chromosome *g, UINT pickup, Task* task);
int selectCustomerByCentroid(Route *route, Task* task);
#endif