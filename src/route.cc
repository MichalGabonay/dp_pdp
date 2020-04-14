#include "route.h"

// weighted random select of location from route by inbound and outbound edges
int Route::selectLocationByCost() {
  std::vector<double> locationsRatios;
  for (size_t i = 0; i < this->route_length; i++)
  {
    if (i == 0 || i == this->route_length - 1)
    {
      locationsRatios.push_back(0);
    } else {
      locationsRatios.push_back(this->cost_in[i] + this->cost_out[i]);
    }
  }

  std::mt19937 gen(rand());
  std::discrete_distribution<> dist(locationsRatios.begin(), locationsRatios.end());
  return dist(gen);
}

int Route::selectRandomCustomer() {
  int index = urandom(1, this->route_length - 2);
  int location = this->locations[index];
  if (location % 2 == 0)
  {
    return location - 1;
  } else {
    return location;
  }
}

// weighted random select of customer from route by inbound and outbound edges of customer locations
int Route::selectCustomerByCost(std::vector<UINT> map_route_position) {
  std::vector<double> locationsRatios;
  std::vector<double> map;
  for (size_t i = 0; i < this->route_length; i++)
  {
    if (i > 0 && i < this->route_length - 1 && this->locations[i] % 2 == 1)
    {
      UINT d_i = map_route_position[this->locations[i]+1];
      locationsRatios.push_back(this->cost_in[i] + this->cost_out[i] + this->cost_in[d_i] + this->cost_out[d_i]);
      map.push_back(this->locations[i]);
    }
  }

  std::mt19937 gen(rand());
  std::discrete_distribution<> dist(locationsRatios.begin(), locationsRatios.end());
  return map[dist(gen)];
}

// weighted random select of customer from route by centroid distance
int Route::selectCustomerByCentroid(Task* task) {
  std::vector<double> locationsRatios;
  std::vector<double> map;
  for (size_t i = 0; i < this->route_length; i++)
  {
    if (i > 0 && i < this->route_length - 1 && this->locations[i] % 2 == 1)
    {
      locationsRatios.push_back(centroidDiff(this->centroid, task->coords[this->locations[i]]) + centroidDiff(this->centroid, task->coords[this->locations[i]+1]));
      map.push_back(this->locations[i]);
    }
  }

  std::mt19937 gen(rand());
  std::discrete_distribution<> dist(locationsRatios.begin(), locationsRatios.end());
  return map[dist(gen)];
}

void Route::printRoute(int vehicle_index) {
  printf("Vehicle %d (%d): ", vehicle_index, this->route_length);
  for (size_t j = 0; j < this->route_length; j++)
  {
    if (j != this->route_length - 1)
    {
      printf("%d[%d | %f], ", this->locations[j], this->utilization[j], this->cuml_duration[j]);
    }
    else
    {
      printf("%d[%d | %f]", this->locations[j], this->utilization[j], this->cuml_duration[j]);
    }
  }
  printf("\n");
  printf("cost: %d  --  duration: %f  --  distance: %d \n\n", this->cost, this->duration, this->distance);
}