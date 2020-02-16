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
  if (index == g->routes[vehicle].route_length - 1)
  {
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
  }
  
  // std::cout << "som tu 2" << std::endl;
  swapArrayValues(&g->routes[vehicle].locations, index, index + 1);
  // std::cout << "som tu 3" << std::endl;
  g->routes[vehicle].utilization[index] = g->routes[vehicle].utilization[index - 1] + demands[value_of_switched];
  // std::cout << "som tu 4" << std::endl;
  g->map_route_position[value]++;
  // std::cout << "som tu 5" << std::endl;
  g->map_route_position[value_of_switched]--;
  // std::cout << "som tu 6" << std::endl;
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