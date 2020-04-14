#ifndef INDIVIDUAL_H
#define INDIVIDUAL_H 1

#include <stdio.h>
#include <stdlib.h>
#include <vector>

#include "task.h"
#include "helpers.h"
#include "route.h"

typedef unsigned int UINT;
typedef int BOOL;

class Individual
{
public:
    double fitness; 
    double cost;
    bool evaluate;
    std::vector<Route> routes;
    std::vector<UINT> map_route_position;

    void swapLocations (UINT vehicle, UINT vehicle_capacity, std::vector<int> demands, Task* task);
    UINT insertToRoute (UINT vehicle, UINT index, UINT value, int demand, Task* task);
    void deleteFromRoute (UINT vehicle, UINT index);
    int selectRoute(int number_of_vehicles);
    void validateAndFixRoute(UINT vehicle, UINT vehicle_capacity, std::vector<int> demands, Task* task);
    void recalculateRoute (UINT vehicle, Task* task);
    int selectRouteByWeight();
    void inserCustomerToRoute(int vehicle, UINT pickup, Task *task);
    void realocateCustomerInRoute(int vehicle, UINT pickup, Task *task);
    int selectRouteByCentroid(UINT pickup, Task* task);
    void moveInsideRoute (UINT vehicle, UINT index_from, UINT index_to, UINT value);
};

#endif