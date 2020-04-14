#ifndef HELPERS_H
#define HELPERS_H 1

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

UINT urandom(UINT low, UINT high);
std::pair<int, int> calculateCentroid(std::set<std::pair<int, int>> setOfCords);
double centroidDiff(std::pair<int, int> centroid, std::pair<int, int> coord);

#endif