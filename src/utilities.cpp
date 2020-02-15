#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <ctype.h>
#include <errno.h>
#include <limits.h>
#include <math.h>
#include <vector>
#include <sstream>
#include <fstream>
#include <string>

typedef enum {
    STR2INT_SUCCESS,
    STR2INT_OVERFLOW,
    STR2INT_UNDERFLOW,
    STR2INT_INCONVERTIBLE
} str2int_errno;

#include "params.h"

str2int_errno str2int(int *out, char *s, UINT base) {
    char *end;
    if (s[0] == '\0' || isspace(s[0]))
        return STR2INT_INCONVERTIBLE;
    errno = 0;
    long l = strtol(s, &end, base);
    /* Both checks are needed because INT_MAX == LONG_MAX is possible. */
    if (l > INT_MAX || (errno == ERANGE && l == LONG_MAX))
        return STR2INT_OVERFLOW;
    if (l < INT_MIN || (errno == ERANGE && l == LONG_MIN))
        return STR2INT_UNDERFLOW;
    if (*end != '\0')
        return STR2INT_INCONVERTIBLE;
    *out = l;
    return STR2INT_SUCCESS;
}

void readDataset(std::vector<int> *locations_map, std::vector<int> *vehicles, std::vector<std::vector<int>> *locations, std::string input_file_name){

  // std::string input_file_name = "lc101.txt";
  std::ifstream file(input_file_name);
  UINT line_index = 0;
  std::string line;
  std::string partial;

  std::vector<int> tokens;

  while(std::getline(file, line)) {

    std::istringstream iss(line);
    std::string token;
    while(std::getline(iss, token, '\t')) {
      tokens.push_back(std::stoi( token ));
    }
    if (line_index == 0)
    {
      for (UINT i=0; i<tokens.size(); i++) 
        vehicles->push_back(tokens[i]); 
    } else {
      locations->push_back(tokens);
    }
    tokens.clear();
    line_index++;
  }

  file.close();

  for (UINT i = 0; i < line_index - 1; i++)
  {
    if (i == 0)
    {
      locations_map->push_back(0);
    }
    if (locations->at(i)[8] != 0)
    {
      locations_map->push_back(i);
      locations_map->push_back(locations->at(i)[8]);
    }
  }
}

double calculateDistanceBetweenPoints(int x1, int y1, int x2, int y2) {
  return( hypot(x1 - x2, y1 - y2) );
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

void swapNeighborsInRoute (GA_chromosome *g, UINT vehicle, UINT vehicle_capacity, std::vector<std::vector<int>> &locations, std::vector<int> &map_locations) {
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
    if (g->routes[vehicle].utilization[index - 1] + locations[map_locations[value_of_switched]][3] > (int) vehicle_capacity)
    {
      index++;
      continue;
    }
    is_possible_swap = true;
  }
  if (index == g->routes[vehicle].route_length - 1)
  {
    // ****************
    printf("Vehicle %d (%d): ", vehicle, g->routes[vehicle].route_length);
    for (size_t j = 0; j < g->routes[vehicle].route_length; j++)
    {
      if (j != g->routes[vehicle].route_length - 1)
      {
        printf("%d[%d], ", g->routes[vehicle].locations[j], g->routes[vehicle].utilization[j]);
      }
      else
      {
        printf("%d[%d]", g->routes[vehicle].locations[j], g->routes[vehicle].utilization[j]);
      }
    }
    printf("\n");
    // ****************

    std::cout << index << "  " << g->routes[vehicle].route_length << "  " << std::endl;
    std::cout << value << "  " << value_of_switched << "  " << std::endl;
  }
  
  // std::cout << "som tu 2" << std::endl;
  swapArrayValues(&g->routes[vehicle].locations, index, index + 1);
  // std::cout << "som tu 3" << std::endl;
  g->routes[vehicle].utilization[index] = g->routes[vehicle].utilization[index - 1] + locations[map_locations[value_of_switched]][3];
  // std::cout << "som tu 4" << std::endl;
  g->map_route_position[value]++;
  // std::cout << "som tu 5" << std::endl;
  g->map_route_position[value_of_switched]--;
  // std::cout << "som tu 6" << std::endl;
}

UINT insertToRoute (GA_chromosome *g, UINT vehicle, UINT index, UINT value, std::vector<int> location, UINT vehicle_capacity) {
  UINT route_size = g->routes[vehicle].route_length;
  while (g->routes[vehicle].utilization[index - 1] + location[3] > (int) vehicle_capacity)
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
      g->routes[vehicle].utilization.push_back(g->routes[vehicle].utilization[i] + location[3]);
    } else {
      g->routes[vehicle].locations[i + 1] = g->routes[vehicle].locations[i];
      g->routes[vehicle].utilization[i + 1] = g->routes[vehicle].utilization[i] + location[3];
    }
  }
  g->map_route_position[value] = index;
  g->routes[vehicle].locations[index] = value;  
  g->routes[vehicle].utilization[index] = g->routes[vehicle].utilization[index - 1] + location[3];  
  g->routes[vehicle].route_length++;

  return index;
}

void deleteFromRoute (GA_chromosome *g, UINT vehicle, UINT index, std::vector<int> location) {
  UINT route_size = g->routes[vehicle].route_length;
  
  for (size_t i = index; i < route_size - 1; i++)
  {
    if (g->map_route_position[g->routes[vehicle].locations[i]] != 0 && g->map_route_position[g->routes[vehicle].locations[i]] != index)
    {
      g->map_route_position[g->routes[vehicle].locations[i]]--; 
    }
    g->routes[vehicle].locations[i] = g->routes[vehicle].locations[i + 1];
    g->routes[vehicle].utilization[i] = g->routes[vehicle].utilization[i + 1] - location[3];
  }
  g->routes[vehicle].locations[route_size - 1] = 0;
  g->routes[vehicle].locations.pop_back();
  g->routes[vehicle].utilization[route_size - 1] = 0;
  g->routes[vehicle].utilization.pop_back();
  g->routes[vehicle].route_length--;
}
