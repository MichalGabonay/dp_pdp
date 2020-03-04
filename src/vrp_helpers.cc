#include "vrp_helpers.h"

// generuje cele cislo v rozsahu low-high vcetne
UINT urandom(UINT low, UINT high)
{
  return rand() % (high - low + 1) + low;
}


double travelCost(std::vector<double> matrix, int matrix_order, UINT from, UINT to) {
  return matrix[from * matrix_order + to];
}

void swapArrayValues(std::vector<UINT> *locations, UINT position1, UINT position2) {
    UINT value1 = locations->at(position1);
    locations->at(position1) = locations->at(position2);
    locations->at(position2) = value1;
}

void printArray(UINT *array, UINT array_size) {
  for (size_t j = 0; j < array_size; j++)
    {
        if (j != array_size - 1)
        {
            printf("%d, ", array[j]);
        }
        else
        {
            printf("%d", array[j]);
        }
    }
    printf("\n");
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

void swapNeighborsInRoute (GA_chromosome *g, UINT vehicle, UINT vehicle_capacity, std::vector<int> demands) {
  bool is_possible_swap = false;
  UINT value, value_of_switched;

  UINT index = urandom(1, g->routes[vehicle].route_length - 3);
  while (!is_possible_swap) {
    value = g->routes[vehicle].locations[index];
    value_of_switched = g->routes[vehicle].locations[index + 1];

    if (value == value_of_switched - 1)
    {
      index++;
      if (index > g->routes[vehicle].route_length - 3)
      {
        return;
      }
      
      continue;
    }
    if (g->routes[vehicle].utilization[index - 1] + demands[value_of_switched] > (int) vehicle_capacity)
    {
      index++;
      continue;
    }
    is_possible_swap = true;
  }
  
  swapArrayValues(&g->routes[vehicle].locations, index, index + 1);
  g->routes[vehicle].utilization[index] = g->routes[vehicle].utilization[index - 1] + demands[value_of_switched];
  g->map_route_position[value]++;
  g->map_route_position[value_of_switched]--;
}

void swapLocations (GA_chromosome *g, UINT vehicle, UINT vehicle_capacity, std::vector<int> demands, Task* task) {
  UINT value, value_of_switched, index_1, index_2;
  int number_of_tries = 0;
  bool found_valid = false;
  while (number_of_tries < 5 && found_valid == false) { // 5 tries to find valid, than it will take invalid which will be repaired
    number_of_tries++;
    index_1 = urandom(1, g->routes[vehicle].route_length - 3);
    value = g->routes[vehicle].locations[index_1];
    if (value % 2 == 0)
    {
      UINT index_1_pair = g->map_route_position[value - 1];
      // std::cout << index_1 << "(" << value << ") : " << index_1_pair + 1 << " - " << g->routes[vehicle].route_length - 3 << std::endl;
      index_2 = urandom(index_1_pair + 1, g->routes[vehicle].route_length - 3);
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

  if (index_1 < index_2)
  {
    /* code */
  } else {

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
      g->routes[vehicle].cuml_duration[i] += g->routes[vehicle].cost_in[i];
      g->routes[vehicle].utilization[i] = 0;
      g->routes[vehicle].duration += g->routes[vehicle].cost_in[i];

    } else 
    {
      g->routes[vehicle].cost_in[i] = g->routes[vehicle].cost_out[i - 1];
      g->routes[vehicle].cost_out[i] = task->matrix[g->routes[vehicle].locations[i] * task->matrix_order + g->routes[vehicle].locations[i+1]];
      g->routes[vehicle].cuml_duration[i] += g->routes[vehicle].cost_in[i];
      g->routes[vehicle].utilization[i] += task->demands[g->routes[vehicle].locations[i]];
      g->routes[vehicle].duration += g->routes[vehicle].cost_in[i];
    }  
  } 
}

void moveInsideRoute (GA_chromosome *g, UINT vehicle, UINT index_from, UINT index_to, UINT value, int demand, Task* task) {
  UINT route_size = g->routes[vehicle].route_length;
  if (index_from < index_to) {
    double without_from = travelCost(task->matrix, task->matrix_order, g->routes[vehicle].locations[index_from - 1], g->routes[vehicle].locations[index_from + 1]);
    double change1 = without_from - g->routes[vehicle].cost_in[index_from] - g->routes[vehicle].cost_out[index_from];
    double new_in = travelCost(task->matrix, task->matrix_order, g->routes[vehicle].locations[index_to], value);
    double new_out = travelCost(task->matrix, value, task->matrix_order, g->routes[vehicle].locations[index_to + 1]);
    double change2 = new_in + new_out - g->routes[vehicle].cost_in[index_to];
    double change = change1 + change2;

    g->routes[vehicle].cost_out[index_from - 1] = without_from;
    if (index_from == index_to + 1)
    {
      g->routes[vehicle].cost_in[index_from] = without_from;
      g->routes[vehicle].cost_out[index_from] = new_in;
      g->routes[vehicle].cuml_duration[index_from] = g->routes[vehicle].cuml_duration[index_from - 1] + without_from;

      g->routes[vehicle].cost_in[index_to] = new_in;
      g->routes[vehicle].cost_out[index_to] = new_out;
      g->routes[vehicle].cuml_duration[index_to] = g->routes[vehicle].cuml_duration[index_from] + new_in;
      g->routes[vehicle].cost_in[index_to+1] = new_out;

      g->map_route_position[g->routes[vehicle].locations[index_to]]--;
      g->routes[vehicle].locations[index_from] = g->routes[vehicle].locations[index_to];
      g->routes[vehicle].utilization[index_from] = g->routes[vehicle].utilization[index_to] - demand;
    } else {
      for (size_t i = index_from + 1; i <= index_to; i++)
      {
        if (i == index_from + 1) {
          g->routes[vehicle].cost_in[i-1] = without_from;
          g->routes[vehicle].cost_out[i-1] = g->routes[vehicle].cost_out[i];
          g->routes[vehicle].cuml_duration[i - 1] = g->routes[vehicle].cuml_duration[i] + change1;
        } else if (i == index_to) {
          g->routes[vehicle].cost_in[i-1] = g->routes[vehicle].cost_in[i];
          g->routes[vehicle].cost_out[i-1] = new_in;
          g->routes[vehicle].cuml_duration[i-1] = g->routes[vehicle].cuml_duration[i] + change1;
          g->routes[vehicle].cost_in[i] = new_in;
          g->routes[vehicle].cost_out[i] = new_out;
          g->routes[vehicle].cuml_duration[i] = g->routes[vehicle].cuml_duration[i-1] + new_in;
          g->routes[vehicle].cost_in[i+1] = new_out;
        } else {
          g->routes[vehicle].cost_in[i-1] = g->routes[vehicle].cost_in[i];
          g->routes[vehicle].cost_out[i-1] = g->routes[vehicle].cost_out[i];
          g->routes[vehicle].cuml_duration[i-1] = g->routes[vehicle].cuml_duration[i] + change1;
        }
        
        if (g->map_route_position[g->routes[vehicle].locations[i]] != 0)
        {
          g->map_route_position[g->routes[vehicle].locations[i]]--;
        }
        g->routes[vehicle].locations[i-1] = g->routes[vehicle].locations[i];
        g->routes[vehicle].utilization[i-1] = g->routes[vehicle].utilization[i] - demand;
      }
    }
    for (size_t i = index_to; i < route_size; i++)
    {
      g->routes[vehicle].cuml_duration[i] = g->routes[vehicle].cuml_duration[i] + change;
    }  
    g->routes[vehicle].duration += change;
  } else 
  {
    for (size_t i = index_from - 1; i >= index_to; i--)
    {
      if (g->map_route_position[g->routes[vehicle].locations[i]] != 0)
      {
        g->map_route_position[g->routes[vehicle].locations[i]]++; 
      }
      g->routes[vehicle].locations[i+1] = g->routes[vehicle].locations[i];
      g->routes[vehicle].utilization[i+1] = g->routes[vehicle].utilization[i] + demand;
    }
    g->routes[vehicle].utilization[index_to] = g->routes[vehicle].utilization[index_to-1] + demand;
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

  double arc_in = travelCost(task->matrix, task->matrix_order, g->routes[vehicle].locations[index - 1], value);
  double arc_out = travelCost(task->matrix, task->matrix_order, value, g->routes[vehicle].locations[index]);
  double old_arc = travelCost(task->matrix, task->matrix_order, g->routes[vehicle].locations[index - 1], g->routes[vehicle].locations[index]);
  double arc_change = arc_in + arc_out - old_arc;
  
  for (size_t i = route_size - 1; i >= index; i--)
  {
    if (g->map_route_position[g->routes[vehicle].locations[i]] != 0)
    {
      g->map_route_position[g->routes[vehicle].locations[i]]++; 
    }
    if (i == route_size - 1)
    {
      if (i == index + 1)
      {
        g->routes[vehicle].cost_in.push_back(arc_out);
      } else {
        g->routes[vehicle].cost_in.push_back(g->routes[vehicle].cost_in[i]);
      }
      g->routes[vehicle].cuml_duration.push_back(g->routes[vehicle].cuml_duration[i] + arc_change);
      g->routes[vehicle].cost_out.push_back(0);
      g->routes[vehicle].locations.push_back(g->routes[vehicle].locations[i]);
      g->routes[vehicle].utilization.push_back(g->routes[vehicle].utilization[i] + demand);
    } else {
      if (i == index + 1)
      {
        g->routes[vehicle].cost_in[i + 1] = arc_out;
      } else {
        g->routes[vehicle].cost_in[i + 1] = g->routes[vehicle].cost_in[i];
      }
      g->routes[vehicle].cost_out[i + 1] = g->routes[vehicle].cost_out[i];
      g->routes[vehicle].cuml_duration[i + 1] = g->routes[vehicle].cuml_duration[i] + arc_change;
      g->routes[vehicle].locations[i + 1] = g->routes[vehicle].locations[i];
      g->routes[vehicle].utilization[i + 1] = g->routes[vehicle].utilization[i] + demand;
    }
  }
  g->routes[vehicle].cost_out[index - 1] = arc_in;
  g->routes[vehicle].cost_in[index] = arc_in;
  g->routes[vehicle].cost_out[index] = arc_out;
  g->routes[vehicle].cuml_duration[index] = g->routes[vehicle].cuml_duration[index - 1] + arc_in;
  g->routes[vehicle].duration += arc_change;

  g->map_route_position[value] = index;
  g->routes[vehicle].locations[index] = value;  
  g->routes[vehicle].utilization[index] = g->routes[vehicle].utilization[index - 1] + demand;  
  g->routes[vehicle].route_length++;

  return index;
}

void deleteFromRoute (GA_chromosome *g, UINT vehicle, UINT index, int demand, Task* task) {
  UINT route_size = g->routes[vehicle].route_length;
  
  double arc_in = travelCost(task->matrix, task->matrix_order, g->routes[vehicle].locations[index - 1], g->routes[vehicle].locations[index]);
  double arc_out = travelCost(task->matrix, task->matrix_order, g->routes[vehicle].locations[index], g->routes[vehicle].locations[index + 1]);
  double new_arc = travelCost(task->matrix, task->matrix_order, g->routes[vehicle].locations[index - 1], g->routes[vehicle].locations[index + 1]);
  double arc_change = new_arc - (arc_in + arc_out);

  for (size_t i = index; i < route_size - 1; i++)
  {
    if  (i ==  index) {
      g->routes[vehicle].cost_out[i-1] = new_arc;
      g->routes[vehicle].cost_in[i] = new_arc;
    } else {
      g->routes[vehicle].cost_out[i-1] = g->routes[vehicle].cost_out[i];
      g->routes[vehicle].cost_in[i] = g->routes[vehicle].cost_in[i + 1];
    }
    g->routes[vehicle].cuml_duration[i] = g->routes[vehicle].cuml_duration[i] + arc_change;

    if (g->map_route_position[g->routes[vehicle].locations[i]] != 0 && g->map_route_position[g->routes[vehicle].locations[i]] != index)
    {
      g->map_route_position[g->routes[vehicle].locations[i]]--; 
    }
    g->routes[vehicle].locations[i] = g->routes[vehicle].locations[i + 1];
    g->routes[vehicle].utilization[i] = g->routes[vehicle].utilization[i + 1] - demand;
  }
  g->routes[vehicle].locations[route_size - 1] = 0;
  g->routes[vehicle].locations.pop_back();
  g->routes[vehicle].utilization[route_size - 1] = 0;
  g->routes[vehicle].utilization.pop_back();

  double arc_to_depot = travelCost(task->matrix, task->matrix_order, g->routes[vehicle].locations[route_size - 2], g->routes[vehicle].locations[route_size - 1]);
  g->routes[vehicle].cost_out[route_size - 2] = arc_to_depot;
  g->routes[vehicle].cost_in[route_size - 1] = arc_to_depot;
  g->routes[vehicle].cost_in.pop_back();
  g->routes[vehicle].cost_out[route_size - 1] = 0;
  g->routes[vehicle].cost_out.pop_back();
  g->routes[vehicle].cuml_duration[route_size - 1] = g->routes[vehicle].cuml_duration[route_size - 2] + arc_to_depot;
  g->routes[vehicle].cuml_duration.pop_back();
  g->routes[vehicle].duration += arc_change;

  g->routes[vehicle].route_length--;
}

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

void validateAndFixRoute(GA_chromosome *g, UINT vehicle, UINT vehicle_capacity, std::vector<int> demands, Task* task) {
  int load = 0;
  std::map<int, bool> visited;
  UINT i = 0;
  while (i < g->routes[vehicle].route_length)
  {
    // printRoute(g->routes[vehicle], vehicle);
    int location = g->routes[vehicle].locations[i];
    if (location != 0)
    {
      if (location % 2 == 0 && visited.find(location - 1) == visited.end()) // if pickup is in route behind delivery
      {
        UINT pickup_index = g->map_route_position[location - 1];
        switch (urandom(1, 2))
        {
          case 1: { // move delivery somewhere behind pickup
            int new_index = urandom(pickup_index + 1, g->routes[vehicle].route_length - 2);
            // moveInsideRoute(g, vehicle, i, new_index, location, demands[location], task);
            deleteFromRoute(g, vehicle, i, demands[location], task);
            insertToRoute(g, vehicle, new_index, location, demands[location], task);
            recalculateRoute(g, vehicle, task);
            break;
          } 
          case 2: { // move pickup exactly 1 place before delivery
            // moveInsideRoute(g, vehicle, pickup_index, i, location-1, demands[location-1], task);
            deleteFromRoute(g, vehicle, pickup_index, demands[location-1], task);
            insertToRoute(g, vehicle, i, location-1, demands[location-1], task);
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
          // moveInsideRoute(g, vehicle, i, j, location, demands[location], task);
          deleteFromRoute(g, vehicle, i, demands[location], task);
          insertToRoute(g, vehicle, j-1, location, demands[location], task);
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