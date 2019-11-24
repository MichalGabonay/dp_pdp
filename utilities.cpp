#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <ctype.h>
#include <errno.h>
#include <limits.h>
#include <math.h>
#include <vector>


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

void readDataset(std::vector<int> *locations_map, std::vector<int> *vehicles, int locations[RSIZ][9]){
  char line[RSIZ][LSIZ];

  FILE *fptr = NULL; 
  UINT line_index = 0;
  char const *fname = "lc101.txt";

  UINT h = 0;
  fptr = fopen(fname, "r");
  while(fgets(line[line_index], LSIZ, fptr)) 
	{
    // printf("line_index: %d\n", line_index);
    // if (line_index > 0)
    // {
      char* p = strtok(line[line_index], " \t\n\r");
      int int_value;
      h = 0;
      // init(&locations[line_index - 1]);
      while(p)
      {
        int_value = 0;
        str2int(&int_value, p, 10);
        // if (line_index == 4)
        // {
          // printf(" %d: %s - %d\n", h, p, int_value);
        // }
        
        if (line_index == 0)
        {
          vehicles->push_back(int_value);
        } else {
          // printf("h: %d  --  int_value: %d\n", h, int_value);
          // append(&locations[line_index - 1], int_value);
          locations[line_index - 1][h] = int_value;
          // print_vector(&locations[line_index - 1]);
        }
        
        p = strtok(NULL, " \t\n\r");
        h++;
      }

      // for (UINT i = 0; i < h; i++)
      // {
      //   printf("%d", locations[line_index - 1][i]);
      //   if (i != h - 1)
      //   {
      //     printf(", ");
      //   }
      // }
      // printf("\n");

      
    // }
      // if (line_index == 2)
      // {
      //   break;
      // }
    
    line_index++;
  }
  fclose(fptr);

  for (UINT i = 0; i < line_index - 1; i++)
  {
    // for (size_t j = 0; j < 9; j++)
    // {
    //   printf("%d, ", locations[i][j]);
    // }
    // printf("\n");
    if (i == 0)
    {
      locations_map->push_back(0);
    }
    // printf("%d - ", locations[i][9]);
    if (locations[i][8] != 0)
    {
      // print_vector(&locations[i]);
      locations_map->push_back(i);
      locations_map->push_back(locations[i][8]);
    }
  }
}

double calculateDistanceBetweenPoints(int x1, int y1, int x2, int y2) {
  return( hypot(x1 - x2, y1 - y2) );
}

void deleteFromArray(UINT *array, UINT array_size, UINT index, int value_of_index) {
  for(UINT i = index; i < array_size - 1; i++){
    if (value_of_index == 0)
    {
      array[i] = array[i + 1];
    } else {
      array[i] = array[i + 1] - value_of_index;
    }
  }
}

int findValueInArray(UINT *array, UINT array_size, UINT value){
    for(UINT i = 0; i < array_size; i++){
        if(array[i] == value){
            return i;
        }
    }

    return -1;
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

void insertToRoute (GA_chromosome *g, UINT vehicle, UINT index, UINT value, int *location) {
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

void deleteFromRoute (GA_chromosome *g, UINT vehicle, UINT index, int *location) {
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
