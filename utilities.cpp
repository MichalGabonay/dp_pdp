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

void readDataset(std::vector<int> *locations_map, std::vector<int> *vehicles, std::vector<std::vector<int>> *locations){

  std::ifstream file("lc101.txt");
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

void insertToRoute (GA_chromosome *g, UINT vehicle, UINT index, UINT value, std::vector<int> location) {
  UINT route_size = g->routes[vehicle].route_length;
  
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
