#include "cxxopts.hpp"

typedef unsigned int UINT;

void readDataset(std::vector<int> *locations_map, std::vector<int> *vehicles, std::vector<std::vector<int>> *locations, std::string input_file_name);
double calculateDistanceBetweenPoints(int x1, int y1, int x2, int y2);
void swapArrayValues(std::vector<UINT> *array, UINT position1, UINT position2);
void printArray(UINT *array, UINT array_size);
