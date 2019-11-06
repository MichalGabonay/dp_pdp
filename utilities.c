#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <ctype.h>
#include <errno.h>
#include <limits.h>
#include <math.h>

typedef enum {
    STR2INT_SUCCESS,
    STR2INT_OVERFLOW,
    STR2INT_UNDERFLOW,
    STR2INT_INCONVERTIBLE
} str2int_errno;

#include "params.h"

str2int_errno str2int(UINT *out, char *s, UINT base) {
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

void read_dataset(Vector *locations_map, Vector *vehicles, UINT locations[RSIZ][9]){
  char line[RSIZ][LSIZ];

  FILE *fptr = NULL; 
  UINT line_index = 0;
  char *fname = "lc101.txt";

  UINT h = 0;
  fptr = fopen(fname, "r");
  while(fgets(line[line_index], LSIZ, fptr)) 
	{
    // printf("line_index: %d\n", line_index);
    if (line_index > 0)
    {
      char* p = strtok(line[line_index], " \t\n\r");
      UINT int_value;
      h = 0;
      // init(&locations[line_index - 1]);
      while(p)
      {
        int_value = 0;
        str2int(&int_value, p, 10);
        // if (line_index == 4)
        // {
        //   printf(" %d: %s - %d\n", h, p, int_value);
        // }
        
        if (line_index == 0)
        {
          append(vehicles, int_value);
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

      
    }
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
      append(locations_map, 0);
    }
    // printf("%d - ", locations[i][9]);
    if (locations[i][8] != 0)
    {
      // print_vector(&locations[i]);
      append(locations_map, i);
      append(locations_map, locations[i][8]);
    }
  }
  // print_vector(locations_map);
}

double calculate_distance_between_points(UINT x1, UINT y1, UINT x2, UINT y2) {
  return( hypot((int)x1 - (int)x2, (int)y1 - (int)y2) );
}

void delete_from_array(UINT *array, UINT array_size, UINT index) {
  for(UINT i = index; i < array_size - 1; i++){
    array[i] = array[i + 1];
  }
}

int find_value_in_array(UINT *array, UINT array_size, UINT value){
    for(int i = 0; i < array_size; i++){
        if(array[i] == value){
            return i;
        }
    }

    return -1;
}

void insert_to_array_on_position(UINT *array, UINT array_size, UINT position1) {

}

void swap_array_values(UINT *array, UINT position1, UINT position2) {
    UINT value1 = array[position1];
    array[position1] = array[position2];
    array[position2] = value1;
}

void print_array(UINT *array, UINT array_size) {
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

void generate_report() {

}
