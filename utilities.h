#include "vector.h"

#define LSIZ 1024 
#define RSIZ 108 // Number of lines in input file FIXME: change to not be hardcoded
typedef unsigned int UINT;

void read_dataset(Vector *locations_map, Vector *vehicles, UINT locations[RSIZ][9]);
double calculate_distance_between_points(UINT x1, UINT y1, UINT x2, UINT y2);
void delete_from_array(UINT *array, UINT array_size, UINT index);
int find_value_in_array(UINT *array, UINT array_size, UINT value);
void swap_array_values(UINT *array, UINT position1, UINT position2);
void print_array(UINT *array, UINT array_size);