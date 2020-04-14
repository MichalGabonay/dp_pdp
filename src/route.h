#ifndef ROUTE_H
#define ROUTE_H 1

#include <stdio.h>
#include <stdlib.h>
#include <vector>

#include "helpers.h"

typedef unsigned int UINT;
typedef int BOOL;

class Route
{
private:
  /* data */
public:
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

  void printRoute(int vehicle_index);
  int selectLocationByCost();
  int selectRandomCustomer();
  int selectCustomerByCost(std::vector<UINT> map_route_position);
  int selectCustomerByCentroid(Task* task);

};

#endif