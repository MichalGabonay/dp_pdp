#include "vrp_helpers.h"

int selectRoute(GA_chromosome *genome, int number_of_vehicles) {
  int vehicle = 0;
  UINT v_size = 0;
  vehicle = urandom(0, number_of_vehicles - 1);
  while (true)
  {
    v_size = genome->routes[vehicle].route_length;
    if (v_size > 4)
      break;
    
    vehicle = (vehicle + 1) % number_of_vehicles;
  }
  return vehicle;
}

int selectRouteByWeight(GA_chromosome *g) {
  std::vector<double> routeRatios;
  for (size_t i = 0; i < g->routes.size(); i++)
  {
    routeRatios.push_back(g->routes[i].duration/g->routes[i].route_length);
  }
  std::mt19937 gen(rand());
  std::discrete_distribution<> dist(routeRatios.begin(), routeRatios.end());
  return dist(gen);
}

int selectLocationByCost(Route *route) {
  std::vector<double> locationsRatios;
  for (size_t i = 0; i < route->route_length; i++)
  {
    if (i == 0 || i == route->route_length - 1)
    {
      locationsRatios.push_back(0);
    } else {
      locationsRatios.push_back(route->cost_in[i] + route->cost_out[i]);
    }
  }

  std::mt19937 gen(rand());
  std::discrete_distribution<> dist(locationsRatios.begin(), locationsRatios.end());
  return dist(gen);
}

int selectRandomCustomer(Route *route) {
  int index = urandom(1, route->route_length - 2);
  int location = route->locations[index];
  if (location % 2 == 0)
  {
    return location - 1;
  } else {
    return location;
  }
}

int selectCustomerByCost(Route *route, std::vector<UINT> map_route_position) {
  std::vector<double> locationsRatios;
  std::vector<double> map;
  for (size_t i = 0; i < route->route_length; i++)
  {
    if (i > 0 && i < route->route_length - 1 && route->locations[i] % 2 == 1)
    {
      UINT d_i = map_route_position[i+1];
      locationsRatios.push_back(route->cost_in[i] + route->cost_out[i] + route->cost_in[d_i] + route->cost_out[d_i]);
      map.push_back(route->locations[i]);
    }
  }

  std::mt19937 gen(rand());
  std::discrete_distribution<> dist(locationsRatios.begin(), locationsRatios.end());
  return map[dist(gen)];
}

void realocateCustomerInRoute(GA_chromosome *g, int vehicle, UINT pickup, Task *task) {
  UINT pickup_index = g->map_route_position[pickup];
  UINT delivery_index = g->map_route_position[pickup+1];

  deleteFromRoute(g, vehicle, delivery_index);
  deleteFromRoute(g, vehicle, pickup_index);
  recalculateRoute(g, vehicle, task);
  inserCustomerToRoute(g, vehicle, pickup, task);
}

void inserCustomerToRoute(GA_chromosome *g, int vehicle, UINT pickup, Task *task) {
  if (g->routes[vehicle].route_length <= 2)
  {
    insertToRoute(g, vehicle, 1, pickup, task->demands[pickup], task);
    insertToRoute(g, vehicle, 2, pickup+1, task->demands[pickup+1], task);
  } else {
    double best_insert = 999999999;
    std::pair<int, int> best_insert_index;
    double min_pickup, min_delivery, min_togather;
    int min_pickup_index, min_delivery_index, min_togather_index;

    bool reset_local_mins = true;
    for (size_t i = 1; i < g->routes[vehicle].route_length; i++)
    {
      double old_route = task->matrix[g->routes[vehicle].locations[i-1] * task->matrix_order + g->routes[vehicle].locations[i]];
      double new_pickup_in = task->matrix[g->routes[vehicle].locations[i-1] * task->matrix_order + pickup];
      double new_pickup_out = task->matrix[pickup * task->matrix_order + g->routes[vehicle].locations[i]];
      double new_delivery_in = task->matrix[g->routes[vehicle].locations[i-1] * task->matrix_order + pickup + 1];
      double new_delivery_out = task->matrix[(pickup + 1) * task->matrix_order + g->routes[vehicle].locations[i]];
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

      if (g->routes[vehicle].utilization[i] + task->demands[pickup] > task->capacity_of_vehicles || i == g->routes[vehicle].route_length - 1)
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

      // std::cout << old_route << " " << new_pickup_in << " " << new_pickup_out << " " << new_delivery_in << " " << new_delivery_out << " " << std::endl;
    }
    // std::cout << best_insert << " " << best_insert_index.first << " " << best_insert_index.second << std::endl;

    insertToRoute(g, vehicle, best_insert_index.first, pickup, task->demands[pickup], task);
    insertToRoute(g, vehicle, best_insert_index.second+1, pickup+1, task->demands[pickup+1], task);
  }
  
  recalculateRoute(g, vehicle, task);
}

// generuje cele cislo v rozsahu low-high vcetne
UINT urandom(UINT low, UINT high)
{
  return rand() % (high - low + 1) + low;
}

void swapArrayValues(std::vector<UINT> *locations, UINT position1, UINT position2) {
    UINT value1 = locations->at(position1);
    locations->at(position1) = locations->at(position2);
    locations->at(position2) = value1;
}

void printRoute(Route route, int vehicle_index) {
  printf("Vehicle %d (%d): ", vehicle_index, route.route_length);
  for (size_t j = 0; j < route.route_length; j++)
  {
    if (j != route.route_length - 1)
    {
      printf("%d[%d | %f], ", route.locations[j], route.utilization[j], route.cuml_duration[j]);
    }
    else
    {
      printf("%d[%d | %f]", route.locations[j], route.utilization[j], route.cuml_duration[j]);
    }
  }
  printf("\n");
  printf("cost: %d  --  duration: %f  --  distance: %d \n\n", route.cost, route.duration, route.distance);
}

void swapLocations (GA_chromosome *g, UINT vehicle, UINT vehicle_capacity, std::vector<int> demands, Task* task) {
  UINT value, value_of_switched, index_1, index_2;
  int number_of_tries = 0;
  bool found_valid = false;
  while (number_of_tries < 5 && found_valid == false) { // 5 tries to find valid, than it will take invalid which will be repaired
    number_of_tries++;
    // index_1 = urandom(1, g->routes[vehicle].route_length - 2);
    index_1 = selectLocationByCost(&g->routes[vehicle]);
    value = g->routes[vehicle].locations[index_1];
    if (value % 2 == 0)
    {
      UINT index_1_pair = g->map_route_position[value - 1];
      // std::cout << index_1 << "(" << value << ") : " << index_1_pair + 1 << " - " << g->routes[vehicle].route_length - 3 << std::endl;
      index_2 = urandom(index_1_pair + 1, g->routes[vehicle].route_length - 2);
    } else {
      UINT index_1_pair = g->map_route_position[value + 1];
      index_2 = urandom(1, index_1_pair - 1);
    }
    if (index_1 == index_2) { // selected same index, dont count as a try
      number_of_tries--;
      continue;
    }
    
    value_of_switched = g->routes[vehicle].locations[index_2];
    if (value_of_switched % 2 == 0)
    {
      UINT index_2_pair = g->map_route_position[value_of_switched - 1];
      if (index_2_pair >= index_1) continue; // invalid pickup-delivery order of switched      
    } else {
      UINT index_2_pair = g->map_route_position[value_of_switched + 1];
      if (index_2_pair >= index_1) continue; // invalid pickup-delivery order of switched   
    }
    int load = 0;
    for (UINT i = 0; i < g->routes[vehicle].route_length; i++)
    {
      load += demands[g->routes[vehicle].locations[i]];
      if (load > (int) vehicle_capacity) continue; // capacity of vehicle exceeded           
    }
    found_valid = true;
  }
  
  swapArrayValues(&g->routes[vehicle].locations, index_1, index_2);

  g->map_route_position[value] = index_2; 
  g->map_route_position[value_of_switched] = index_1;
  recalculateRoute(g, vehicle, task);
  validateAndFixRoute(g, vehicle, vehicle_capacity, demands, task);
}

void recalculateRoute (GA_chromosome *g, UINT vehicle, Task* task) {
  UINT route_size = g->routes[vehicle].route_length;
  for (size_t i = 0; i < route_size; i++)
  {
    if (i == 0) 
    {
      g->routes[vehicle].cost_in[i] = 0;
      g->routes[vehicle].cost_out[i] = task->matrix[g->routes[vehicle].locations[i] * task->matrix_order + g->routes[vehicle].locations[i+1]];
      g->routes[vehicle].cuml_duration[i] = 0;
      g->routes[vehicle].utilization[i] = 0;
      g->routes[vehicle].duration = 0;

    } else if(i == route_size-1) 
    {
      g->routes[vehicle].cost_in[i] = g->routes[vehicle].cost_out[i - 1];
      g->routes[vehicle].cost_out[i] = 0;
      g->routes[vehicle].cuml_duration[i] = g->routes[vehicle].cuml_duration[i - 1] + g->routes[vehicle].cost_in[i];
      g->routes[vehicle].utilization[i] = 0;
      g->routes[vehicle].duration += g->routes[vehicle].cost_in[i];

    } else 
    {
      g->routes[vehicle].cost_in[i] = g->routes[vehicle].cost_out[i - 1];
      g->routes[vehicle].cost_out[i] = task->matrix[g->routes[vehicle].locations[i] * task->matrix_order + g->routes[vehicle].locations[i+1]];
      g->routes[vehicle].cuml_duration[i] = g->routes[vehicle].cuml_duration[i - 1] + g->routes[vehicle].cost_in[i];
      g->routes[vehicle].utilization[i] = g->routes[vehicle].utilization[i - 1] + task->demands[g->routes[vehicle].locations[i]];
      g->routes[vehicle].duration += g->routes[vehicle].cost_in[i];
    }  
  } 
}

void moveInsideRoute (GA_chromosome *g, UINT vehicle, UINT index_from, UINT index_to, UINT value) {
  if (index_from < index_to) {
    for (size_t i = index_from + 1; i <= index_to; i++)
    {
      if (g->map_route_position[g->routes[vehicle].locations[i]] != 0)
      {
        g->map_route_position[g->routes[vehicle].locations[i]]--;
      }
      g->routes[vehicle].locations[i-1] = g->routes[vehicle].locations[i];
    }
  } else {
    for (size_t i = index_from - 1; i >= index_to; i--)
    {
      if (g->map_route_position[g->routes[vehicle].locations[i]] != 0)
      {
        g->map_route_position[g->routes[vehicle].locations[i]]++; 
      }
      g->routes[vehicle].locations[i+1] = g->routes[vehicle].locations[i];
    }
  }

  g->routes[vehicle].locations[index_to] = value;
  g->map_route_position[value] = index_to;
}

UINT insertToRoute (GA_chromosome *g, UINT vehicle, UINT index, UINT value, int demand, Task* task) {
  UINT route_size = g->routes[vehicle].route_length;
  while (g->routes[vehicle].utilization[index - 1] + demand > (int) task->capacity_of_vehicles)
  {
    index++;
  }
  for (size_t i = route_size - 1; i >= index; i--)
  {
    if (g->map_route_position[g->routes[vehicle].locations[i]] != 0)
    {
      g->map_route_position[g->routes[vehicle].locations[i]]++; 
    }
    if (i == route_size - 1)
    {
      g->routes[vehicle].cost_in.push_back(0);
      g->routes[vehicle].cuml_duration.push_back(0);
      g->routes[vehicle].cost_out.push_back(0);
      g->routes[vehicle].utilization.push_back(0);
      g->routes[vehicle].locations.push_back(g->routes[vehicle].locations[i]);
    } else {
      g->routes[vehicle].locations[i + 1] = g->routes[vehicle].locations[i];
    }
  }
  g->map_route_position[value] = index;
  g->routes[vehicle].locations[index] = value;  
  g->routes[vehicle].route_length++;

  return index;
}

void deleteFromRoute (GA_chromosome *g, UINT vehicle, UINT index) {
  UINT route_size = g->routes[vehicle].route_length;
  
  for (size_t i = index; i < route_size - 1; i++)
  {
    if (g->map_route_position[g->routes[vehicle].locations[i]] != 0 && g->map_route_position[g->routes[vehicle].locations[i]] != index)
    {
      g->map_route_position[g->routes[vehicle].locations[i]]--; 
    }
    g->routes[vehicle].locations[i] = g->routes[vehicle].locations[i + 1];
  }
  g->routes[vehicle].locations[route_size - 1] = 0;

  g->routes[vehicle].locations.pop_back();
  g->routes[vehicle].utilization.pop_back();
  g->routes[vehicle].cost_in.pop_back();
  g->routes[vehicle].cost_out.pop_back();
  g->routes[vehicle].cuml_duration.pop_back();

  g->routes[vehicle].route_length--;
}

void validateAndFixRoute(GA_chromosome *g, UINT vehicle, UINT vehicle_capacity, std::vector<int> demands, Task* task) {
  int load = 0;
  std::map<int, bool> visited;
  UINT i = 1;
  while (i < g->routes[vehicle].route_length-1)
  {
    // printRoute(g->routes[vehicle], vehicle);
    int location = g->routes[vehicle].locations[i];
    if (location != 0)
    {
      if (location % 2 == 0 && visited.find(location - 1) == visited.end()) // if pickup is in route behind delivery
      {
        UINT pickup_index = g->map_route_position[location - 1];
        int variant;
        if (pickup_index == g->routes[vehicle].route_length - 2)
        {
          variant = 2;
        } else {
          variant = urandom(1, 2);
        }        
        switch (variant)
        {
          case 1: { // move delivery somewhere behind pickup
            int new_index = urandom(pickup_index + 1, g->routes[vehicle].route_length - 2);
            moveInsideRoute(g, vehicle, i, new_index, location);
            recalculateRoute(g, vehicle, task);
            break;
          } 
          case 2: { // move pickup exactly 1 place before delivery
            moveInsideRoute(g, vehicle, pickup_index, i, location-1);
            recalculateRoute(g, vehicle, task);
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
        for (UINT j = i + 1; j < g->routes[vehicle].route_length - 1; j++)
        {
          if ((int) g->routes[vehicle].locations[j] != location + 1) {
            load_over += demands[g->routes[vehicle].locations[j]];
          }
          if (load_over > (int) vehicle_capacity)
          {
            continue;
          }
          moveInsideRoute(g, vehicle, i, j, location);
          recalculateRoute(g, vehicle, task);
          break;
        }
        continue;        
      }
      visited[location] = true;
      g->routes[vehicle].utilization[i] = load;   
    }
    i++;   
  }
}

// void insertCustomerToRoute(GA_chromosome *g,
//                            UINT pickup, 
//                            UINT delivery,
//                            int vehicle,
//                            UINT vehicle_capacity, 
//                            std::vector<int> demands,
//                            std::vector<int> open_times,
//                            std::vector<int> close_times,
//                            std::vector<int> service_times,
//                            std::vector<double> matrix,
//                            int matrix_order
//                           ){
//   if (vehicle > -1)
//   {
//     bool found_place_to_insert = false;
//     std::vector<std::pair<int, int>> valid_pickups;
//     std::vector<std::pair<int, int>> valid_inserts;
//     for (size_t i = 1; i < g->routes[vehicle].route_length - 2; i++) // excluded depot
//     {
//       double arc_cost = travelCost(matrix, matrix_order, g->routes[vehicle].locations[i], pickup);
//       if (g->routes[vehicle].cuml_duration[i] + arc_cost >=  open_times[pickup] &&
//           g->routes[vehicle].cuml_duration[i] + arc_cost <=  close_times[pickup])
//       {
//         double replaced_arc = travelCost(matrix, matrix_order, g->routes[vehicle].locations[i], g->routes[vehicle].locations[i+1]);
//         double arc_change = replaced_arc - service_times[g->routes[vehicle].locations[i+1]] + arc_cost + service_times[pickup];
//         bool is_valid = false;
//         int deliver_at_least = 0;
//         for (size_t j = i + 1; j < g->routes[vehicle].route_length - 2; j++)
//         {
//           if (!(g->routes[vehicle].cuml_duration[j] + arc_change >= open_times[g->routes[vehicle].locations[j]] &&
//               g->routes[vehicle].cuml_duration[j] + arc_change <= close_times[g->routes[vehicle].locations[j]]))
//           {
//             break;
//           }
//           if (g->routes[vehicle].utilization[j] + demands[pickup] > vehicle_capacity && deliver_at_least == 0)
//           {           
//             deliver_at_least = j+1;
//           }  
//           if (j == g->routes[vehicle].route_length - 2)
//           {
//             is_valid = true;
//           }                           
//         }
//         if (is_valid)
//         {
//           valid_pickups.push_back(std::make_pair(i+1, deliver_at_least));
//         }
//       }      
//     }
//     for(auto vp : valid_pickups)
//     {

//       for (size_t i = vp.second; i < g->routes[vehicle].route_length - 2; i++)
//       {
//         /* code */
//       }
//     }    
//   }
//   for (int i = 0; i < g->routes.size(); i++)
//   {
//     if (i == vehicle)
//     {
//       continue;
//     }
    
//   }
// }