#ifndef TASK_H
#define TASK_H 1

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <limits.h>
#include <vector>
#include <fstream>
#include <sstream>
#include <math.h>
#include <iostream>

typedef unsigned int UINT;

typedef enum {
    STR2INT_SUCCESS,
    STR2INT_OVERFLOW,
    STR2INT_UNDERFLOW,
    STR2INT_INCONVERTIBLE
} str2int_errno;

std::vector<double> calculateDistanceMatrix(std::vector<int> locations_map, std::vector<std::vector<int>> locations);
str2int_errno str2int(int *out, char *s, UINT base);

#define MAX_ROUTE_DURATION 200;

class Task
{
public:
  // input data
  std::vector<double> matrix;
  int matrix_order;
  int number_of_vehicles;
  int capacity_of_vehicles;
  std::vector<int> demands;
  std::vector<int> locations_map;
  std::vector<std::vector<int>> locations;
  std::vector<int> open_times;
  std::vector<int> close_times;
  std::vector<int> service_times;
  double max_route_duration;

  Task();
  bool FetchTask(std::string input_file);
};

#endif