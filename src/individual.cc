#include "individual.h"

// random select route from individual
// sequentialy finding route with more then 1 customer
int Individual::selectRoute(int number_of_vehicles) {
  int vehicle = 0;
  UINT v_size = 0;
  vehicle = urandom(0, number_of_vehicles - 1);
  while (true)
  {
    v_size = this->routes[vehicle].route_length;
    if (v_size > 4)
      break;
    
    vehicle = (vehicle + 1) % number_of_vehicles;
  }
  return vehicle;
}

// weigted random select of the route from individual by ratio between number of customers in route and length of the route
int Individual::selectRouteByWeight() {
  std::vector<double> routeRatios;
  for (size_t i = 0; i < this->routes.size(); i++)
  {
    routeRatios.push_back(this->routes[i].duration/this->routes[i].route_length);
  }
  std::mt19937 gen(rand());
  std::discrete_distribution<> dist(routeRatios.begin(), routeRatios.end());
  return dist(gen);
}

// weigted random select of the route from individual by customer difference from centroid
int Individual::selectRouteByCentroid(UINT pickup, Task* task) {
  std::vector<double> routeRatios;
  for (size_t i = 0; i < this->routes.size(); i++)
  {
    double pickup_centroid_diff = centroidDiff(this->routes[i].centroid, task->coords[pickup]);
    double delivery_centroid_diff = centroidDiff(this->routes[i].centroid, task->coords[pickup+1]);
    double centroid_diff = pickup_centroid_diff + delivery_centroid_diff;
    routeRatios.push_back(1/centroid_diff);
  }
  std::mt19937 gen(rand());
  std::discrete_distribution<> dist(routeRatios.begin(), routeRatios.end());
  return dist(gen);
}

// try to move customer to better position in the route
void Individual::realocateCustomerInRoute(int vehicle, UINT pickup, Task *task) {
  UINT pickup_index = this->map_route_position[pickup];
  UINT delivery_index = this->map_route_position[pickup+1];

  deleteFromRoute(vehicle, delivery_index);
  deleteFromRoute(vehicle, pickup_index);
  recalculateRoute(vehicle, task);
  inserCustomerToRoute(vehicle, pickup, task);
}

// try to find and insert custumer to best position in route
void Individual::inserCustomerToRoute(int vehicle, UINT pickup, Task *task) {
  if (this->routes[vehicle].route_length <= 2)
  {
    insertToRoute(vehicle, 1, pickup, task->demands[pickup], task);
    insertToRoute(vehicle, 2, pickup+1, task->demands[pickup+1], task);
  } else {
    double best_insert = 999999999;
    std::pair<int, int> best_insert_index;
    double min_pickup, min_delivery, min_togather;
    int min_pickup_index, min_delivery_index, min_togather_index;

    bool reset_local_mins = true;
    for (size_t i = 1; i < this->routes[vehicle].route_length; i++)
    {
      double old_route = task->matrix[this->routes[vehicle].locations[i-1] * task->matrix_order + this->routes[vehicle].locations[i]];
      double new_pickup_in = task->matrix[this->routes[vehicle].locations[i-1] * task->matrix_order + pickup];
      double new_pickup_out = task->matrix[pickup * task->matrix_order + this->routes[vehicle].locations[i]];
      double new_delivery_in = task->matrix[this->routes[vehicle].locations[i-1] * task->matrix_order + pickup + 1];
      double new_delivery_out = task->matrix[(pickup + 1) * task->matrix_order + this->routes[vehicle].locations[i]];
      double pickup_change = new_pickup_in + new_pickup_out - old_route;
      double delivery_change = new_delivery_in + new_delivery_out - old_route;
      double togather_change = new_pickup_in + task->matrix[pickup * task->matrix_order + pickup + 1] + new_delivery_out - old_route;

      if (reset_local_mins)
      {
        reset_local_mins = false;
        min_pickup = pickup_change;
        min_delivery = delivery_change;
        min_togather = togather_change;
        min_pickup_index = i;
        min_delivery_index = i;
        min_togather_index = i;
      } else {
        if (pickup_change >= min_pickup || delivery_change >= min_delivery)
        {
          if (pickup_change < min_pickup)
          {
            min_pickup = pickup_change;
            min_pickup_index = i;
            min_delivery = delivery_change;
            min_delivery_index = i;
          } else if (delivery_change < min_delivery)
          {
            min_delivery = delivery_change;
            min_delivery_index = i;
          }
        }
        if (togather_change < min_togather)
        {
          min_togather = togather_change;
          min_togather_index = i;
        }
      }

      if (this->routes[vehicle].utilization[i] + task->demands[pickup] > task->capacity_of_vehicles || i == this->routes[vehicle].route_length - 1)
      {
        if (min_pickup == min_delivery || min_pickup + min_delivery > min_togather)
        {
          if (best_insert >= min_togather)
          {
            best_insert = min_togather;
            best_insert_index = std::make_pair(min_togather_index, min_togather_index);
          }
        } else {
          if (best_insert >= min_pickup_index + min_delivery_index)
          {
            best_insert = min_pickup + min_delivery;
            best_insert_index = std::make_pair(min_pickup_index, min_delivery_index);
          }
        }
        reset_local_mins = true;
      }
    }

    insertToRoute(vehicle, best_insert_index.first, pickup, task->demands[pickup], task);
    insertToRoute(vehicle, best_insert_index.second+1, pickup+1, task->demands[pickup+1], task);
  }
  
  recalculateRoute(vehicle, task);
}

// swap two location inside the route, after swap route has to be valid
void Individual::swapLocations (UINT vehicle, UINT vehicle_capacity, std::vector<int> demands, Task* task) {
  UINT value, value_of_switched, index_1, index_2;
  int number_of_tries = 0;
  bool found_valid = false;
  while (number_of_tries < 5 && found_valid == false) { // 5 tries to find valid, than it will take invalid which will be repaired
    number_of_tries++;
    index_1 = this->routes[vehicle].selectLocationByCost();
    value = this->routes[vehicle].locations[index_1];
    if (value % 2 == 0)
    {
      UINT index_1_pair = this->map_route_position[value - 1];
      index_2 = urandom(index_1_pair + 1, this->routes[vehicle].route_length - 2);
    } else {
      UINT index_1_pair = this->map_route_position[value + 1];
      index_2 = urandom(1, index_1_pair - 1);
    }
    if (index_1 == index_2) { // selected same index, dont count as a try
      number_of_tries--;
      continue;
    }
    
    value_of_switched = this->routes[vehicle].locations[index_2];
    if (value_of_switched % 2 == 0)
    {
      UINT index_2_pair = this->map_route_position[value_of_switched - 1];
      if (index_2_pair >= index_1) continue; // invalid pickup-delivery order of switched      
    } else {
      UINT index_2_pair = this->map_route_position[value_of_switched + 1];
      if (index_2_pair >= index_1) continue; // invalid pickup-delivery order of switched   
    }
    int load = 0;
    for (UINT i = 0; i < this->routes[vehicle].route_length; i++)
    {
      load += demands[this->routes[vehicle].locations[i]];
      if (load > (int) vehicle_capacity) continue; // capacity of vehicle exceeded           
    }
    found_valid = true;
  }
  
  std::swap(this->routes[vehicle].locations[index_1], this->routes[vehicle].locations[index_2]);

  this->map_route_position[value] = index_2; 
  this->map_route_position[value_of_switched] = index_1;
  recalculateRoute(vehicle, task);
  validateAndFixRoute(vehicle, vehicle_capacity, demands, task);
}

// recalculate all parameters and metadate of the route
void Individual::recalculateRoute (UINT vehicle, Task* task) {
  UINT route_size = this->routes[vehicle].route_length;
  std::set<std::pair<int, int>> setOfCords;
  for (size_t i = 0; i < route_size; i++)
  {
    setOfCords.insert(task->coords[this->routes[vehicle].locations[i]]);
    if (i == 0) 
    {
      this->routes[vehicle].cost_in[i] = 0;
      this->routes[vehicle].cost_out[i] = task->matrix[this->routes[vehicle].locations[i] * task->matrix_order + this->routes[vehicle].locations[i+1]];
      this->routes[vehicle].cuml_duration[i] = 0;
      this->routes[vehicle].utilization[i] = 0;
      this->routes[vehicle].duration = 0;

    } else if(i == route_size-1) 
    {
      this->routes[vehicle].cost_in[i] = this->routes[vehicle].cost_out[i - 1];
      this->routes[vehicle].cost_out[i] = 0;
      this->routes[vehicle].cuml_duration[i] = this->routes[vehicle].cuml_duration[i - 1] + this->routes[vehicle].cost_in[i];
      this->routes[vehicle].utilization[i] = 0;
      this->routes[vehicle].duration += this->routes[vehicle].cost_in[i];

    } else 
    {
      this->routes[vehicle].cost_in[i] = this->routes[vehicle].cost_out[i - 1];
      this->routes[vehicle].cost_out[i] = task->matrix[this->routes[vehicle].locations[i] * task->matrix_order + this->routes[vehicle].locations[i+1]];
      this->routes[vehicle].cuml_duration[i] = this->routes[vehicle].cuml_duration[i - 1] + this->routes[vehicle].cost_in[i];
      this->routes[vehicle].utilization[i] = this->routes[vehicle].utilization[i - 1] + task->demands[this->routes[vehicle].locations[i]];
      this->routes[vehicle].duration += this->routes[vehicle].cost_in[i];
    }  
  } 
  this->routes[vehicle].centroid = calculateCentroid(setOfCords);
}

void Individual::moveInsideRoute (UINT vehicle, UINT index_from, UINT index_to, UINT value) {
  if (index_from < index_to) {
    for (size_t i = index_from + 1; i <= index_to; i++)
    {
      if (this->map_route_position[this->routes[vehicle].locations[i]] != 0)
      {
        this->map_route_position[this->routes[vehicle].locations[i]]--;
      }
      this->routes[vehicle].locations[i-1] = this->routes[vehicle].locations[i];
    }
  } else {
    for (size_t i = index_from - 1; i >= index_to; i--)
    {
      if (this->map_route_position[this->routes[vehicle].locations[i]] != 0)
      {
        this->map_route_position[this->routes[vehicle].locations[i]]++; 
      }
      this->routes[vehicle].locations[i+1] = this->routes[vehicle].locations[i];
    }
  }

  this->routes[vehicle].locations[index_to] = value;
  this->map_route_position[value] = index_to;
}

// insert location into route to specific index
UINT Individual::insertToRoute (UINT vehicle, UINT index, UINT value, int demand, Task* task) {
  UINT route_size = this->routes[vehicle].route_length;
  while (this->routes[vehicle].utilization[index - 1] + demand > (int) task->capacity_of_vehicles)
  {
    index++;
  }
  for (size_t i = route_size - 1; i >= index; i--)
  {
    if (this->map_route_position[this->routes[vehicle].locations[i]] != 0)
    {
      this->map_route_position[this->routes[vehicle].locations[i]]++; 
    }
    if (i == route_size - 1)
    {
      this->routes[vehicle].cost_in.push_back(0);
      this->routes[vehicle].cuml_duration.push_back(0);
      this->routes[vehicle].cost_out.push_back(0);
      this->routes[vehicle].utilization.push_back(0);
      this->routes[vehicle].locations.push_back(this->routes[vehicle].locations[i]);
    } else {
      this->routes[vehicle].locations[i + 1] = this->routes[vehicle].locations[i];
    }
  }
  this->map_route_position[value] = index;
  this->routes[vehicle].locations[index] = value;  
  this->routes[vehicle].route_length++;

  return index;
}

// delete location at specific index from route
void Individual::deleteFromRoute (UINT vehicle, UINT index) {
  UINT route_size = this->routes[vehicle].route_length;
  
  for (size_t i = index; i < route_size - 1; i++)
  {
    if (this->map_route_position[this->routes[vehicle].locations[i]] != 0 && this->map_route_position[this->routes[vehicle].locations[i]] != index)
    {
      this->map_route_position[this->routes[vehicle].locations[i]]--; 
    }
    this->routes[vehicle].locations[i] = this->routes[vehicle].locations[i + 1];
  }
  this->routes[vehicle].locations[route_size - 1] = 0;

  this->routes[vehicle].locations.pop_back();
  this->routes[vehicle].utilization.pop_back();
  this->routes[vehicle].cost_in.pop_back();
  this->routes[vehicle].cost_out.pop_back();
  this->routes[vehicle].cuml_duration.pop_back();

  this->routes[vehicle].route_length--;
}

// checks if the route meets all the conditions, if not fix it easist (not optimal) way possible
void Individual::validateAndFixRoute(UINT vehicle, UINT vehicle_capacity, std::vector<int> demands, Task* task) {
  int load = 0;
  std::map<int, bool> visited;
  UINT i = 1;
  while (i < this->routes[vehicle].route_length-1)
  {
    // printRoute(this->routes[vehicle], vehicle);
    int location = this->routes[vehicle].locations[i];
    if (location != 0)
    {
      if (location % 2 == 0 && visited.find(location - 1) == visited.end()) // if pickup is in route behind delivery
      {
        UINT pickup_index = this->map_route_position[location - 1];
        int variant;
        if (pickup_index == this->routes[vehicle].route_length - 2)
        {
          variant = 2;
        } else {
          variant = urandom(1, 2);
        }        
        switch (variant)
        {
          case 1: { // move delivery somewhere behind pickup
            int new_index = urandom(pickup_index + 1, this->routes[vehicle].route_length - 2);
            moveInsideRoute(vehicle, i, new_index, location);
            recalculateRoute(vehicle, task);
            break;
          } 
          case 2: { // move pickup exactly 1 place before delivery
            moveInsideRoute(vehicle, pickup_index, i, location-1);
            recalculateRoute(vehicle, task);
            break;
          }
        }
        continue;
      }
      load += demands[location];
      if (load > (int) vehicle_capacity)
      {
        int load_over = load;
        load -= demands[location];
        for (UINT j = i + 1; j < this->routes[vehicle].route_length - 1; j++)
        {
          if ((int) this->routes[vehicle].locations[j] != location + 1) {
            load_over += demands[this->routes[vehicle].locations[j]];
          }
          if (load_over > (int) vehicle_capacity)
          {
            continue;
          }
          moveInsideRoute(vehicle, i, j, location);
          recalculateRoute(vehicle, task);
          break;
        }
        continue;        
      }
      visited[location] = true;
      this->routes[vehicle].utilization[i] = load;   
    }
    i++;   
  }
}