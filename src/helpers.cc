#include "helpers.h"

// generate random unsigned integer in range low-high included
UINT urandom(UINT low, UINT high)
{
  return rand() % (high - low + 1) + low;
}

// centroid is arithmetic mean position of all the points in route
std::pair<int, int> calculateCentroid(std::set<std::pair<int, int>> setOfCords) 
{
  int set_size = setOfCords.size();
  int x_sum = 0;
  int y_sum = 0;
  for (std::set<std::pair<int, int>>::iterator it=setOfCords.begin(); it!=setOfCords.end(); ++it) {
    x_sum += it->first;
    y_sum += it->second;
  }

  return std::make_pair(round(x_sum/set_size), round(y_sum/set_size));
}

// calculate squared difference of point and centroid of the route, using q
double centroidDiff(std::pair<int, int> centroid, std::pair<int, int> coord) 
{
  return pow(centroid.first - coord.first, 2) + pow(centroid.second - coord.second, 2);
}