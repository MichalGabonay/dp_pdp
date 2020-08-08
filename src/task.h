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

// Class represent structure of input task for solving
class Task
{
public:
  std::vector<double> matrix;
  int matrix_order;
  int number_of_vehicles;
  int capacity_of_vehicles;
  std::vector<int> demands;
  std::vector<std::pair<int, int>> coords;
  std::vector<int> locations_map;
  std::vector<std::vector<int>> locations;

  Task();
  bool FetchTask(std::string input_file);
  std::vector<double> calculateDistanceMatrix(std::vector<int> locations_map, std::vector<std::vector<int>> locations);
};

#endif