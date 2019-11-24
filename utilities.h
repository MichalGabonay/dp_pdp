#define LSIZ 1024 
#define RSIZ 108 // Number of lines in input file FIXME: change to not be hardcoded
typedef unsigned int UINT;

void readDataset(std::vector<int> *locations_map, std::vector<int> *vehicles, int locations[RSIZ][9]);
double calculateDistanceBetweenPoints(int x1, int y1, int x2, int y2);
void deleteFromArray(UINT *array, UINT array_size, UINT index, int value_of_index);
int findValueInArray(UINT *array, UINT array_size, UINT value);
void swapArrayValues(std::vector<UINT> *array, UINT position1, UINT position2);
void printArray(UINT *array, UINT array_size);
