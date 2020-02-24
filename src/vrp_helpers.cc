#include "vrp_helpers.h"

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
      printf("%d[%d], ", route.locations[j], route.utilization[j]);
    }
    else
    {
      printf("%d[%d]", route.locations[j], route.utilization[j]);
    }
  }
  printf("\n");
  printf("cost: %d  --  duration: %d  --  distance: %d \n\n", route.cost, route.duration, route.distance);
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
  // if (index == g->routes[vehicle].route_length - 1)
  // {
    // ****************
    // printf("Vehicle %d (%d): ", vehicle, g->routes[vehicle].route_length);
    // for (size_t j = 0; j < g->routes[vehicle].route_length; j++)
    // {
    //   if (j != g->routes[vehicle].route_length - 1)
    //   {
    //     printf("%d[%d], ", g->routes[vehicle].locations[j], g->routes[vehicle].utilization[j]);
    //   }
    //   else
    //   {
    //     printf("%d[%d]", g->routes[vehicle].locations[j], g->routes[vehicle].utilization[j]);
    //   }
    // }
    // printf("\n");
    // ****************

    // std::cout << index << "  " << g->routes[vehicle].route_length << "  " << std::endl;
    // std::cout << value << "  " << value_of_switched << "  " << std::endl;
  // }
  
  swapArrayValues(&g->routes[vehicle].locations, index, index + 1);
  g->routes[vehicle].utilization[index] = g->routes[vehicle].utilization[index - 1] + demands[value_of_switched];
  g->map_route_position[value]++;
  g->map_route_position[value_of_switched]--;
}

void swapLocations (GA_chromosome *g, UINT vehicle, UINT vehicle_capacity, std::vector<int> demands) {
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

  swapArrayValues(&g->routes[vehicle].locations, index_1, index_2);

  g->map_route_position[value] = index_2; 
  g->map_route_position[value_of_switched] = index_1;
  validateAndFixRoute(g, vehicle, vehicle_capacity, demands);
}

UINT insertToRoute (GA_chromosome *g, UINT vehicle, UINT index, UINT value, int demand, UINT vehicle_capacity) {
  UINT route_size = g->routes[vehicle].route_length;
  while (g->routes[vehicle].utilization[index - 1] + demand > (int) vehicle_capacity)
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
      g->routes[vehicle].locations.push_back(g->routes[vehicle].locations[i]);
      g->routes[vehicle].utilization.push_back(g->routes[vehicle].utilization[i] + demand);
    } else {
      g->routes[vehicle].locations[i + 1] = g->routes[vehicle].locations[i];
      g->routes[vehicle].utilization[i + 1] = g->routes[vehicle].utilization[i] + demand;
    }
  }
  g->map_route_position[value] = index;
  g->routes[vehicle].locations[index] = value;  
  g->routes[vehicle].utilization[index] = g->routes[vehicle].utilization[index - 1] + demand;  
  g->routes[vehicle].route_length++;

  return index;
}

void deleteFromRoute (GA_chromosome *g, UINT vehicle, UINT index, int demand) {
  UINT route_size = g->routes[vehicle].route_length;
  
  for (size_t i = index; i < route_size - 1; i++)
  {
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
  g->routes[vehicle].route_length--;
}

int selectRoute(GA_chromosome *genome, int number_of_vehicles) {
  int vehicle = 0;
  UINT v_size = 0;
  UINT number_of_tries = 0;
  while (v_size <= 4)
  {
    vehicle = urandom(0, number_of_vehicles - 1);
    v_size = genome->routes[vehicle].route_length;
    if (number_of_tries >= 9) { // 10 tries for founding route with length at least 4
      return -1;
    }
    number_of_tries++;
  }
  return vehicle;
}

void validateAndFixRoute(GA_chromosome *g, UINT vehicle, UINT vehicle_capacity, std::vector<int> demands) {
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
            deleteFromRoute(g, vehicle, i, demands[location]);
            insertToRoute(g, vehicle, new_index, location, demands[location], vehicle_capacity);
            break;
          } 
          case 2: { // move pickup exactly 1 place before delivery
            deleteFromRoute(g, vehicle, pickup_index, demands[location-1]);
            insertToRoute(g, vehicle, i, location-1, demands[location-1], vehicle_capacity);
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
          deleteFromRoute(g, vehicle, i, demands[location]);
          insertToRoute(g, vehicle, j, location, demands[location], vehicle_capacity);
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