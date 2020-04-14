#include "task.h"

Task::Task() {}

bool Task::FetchTask(std::string input_file)
{
  std::ifstream file(input_file);
  if (!file.is_open())
  {
    std::cout << "Unable to open file" << std::endl; 
    return false;
  } 
 
  int line_index = 0;
  std::string line;
  std::string partial;
  std::vector<int> vehicles;
  std::vector<int> tokens;

  while (std::getline(file, line))
  {
    std::istringstream iss(line);
    std::string token;
    while (std::getline(iss, token, '\t'))
    {
      tokens.push_back(std::stoi(token));
    }
    if (line_index == 0)
    {
      for (size_t i = 0; i < tokens.size(); i++)
        vehicles.push_back(tokens[i]);
    }
    else
    {
      locations.push_back(tokens);
    }
    tokens.clear();
    line_index++;
  }

  file.close();

  for (int i = 0; i < line_index - 1; i++)
  {
    if (i == 0)
    {
      locations_map.push_back(0);
      demands.push_back(0);
      coords.push_back(std::make_pair(locations[0][1], locations[0][2]));
    }
    if (locations[i][8] != 0)
    {
      locations_map.push_back(i);
      locations_map.push_back(locations[i][8]);
      demands.push_back(locations[i][3]);
      demands.push_back(locations[locations[i][8]][3]);
      coords.push_back(std::make_pair(locations[i][1], locations[i][2]));
      coords.push_back(std::make_pair(locations[locations[i][8]][1], locations[locations[i][8]][2]));
    }
  }

  matrix = calculateDistanceMatrix(locations_map, locations);
  matrix_order = sqrt(matrix.size());
  number_of_vehicles = vehicles[0];
  capacity_of_vehicles = vehicles[1];
  return true;
}

std::vector<double> Task::calculateDistanceMatrix(std::vector<int> locations_map, std::vector<std::vector<int>> locations)
{
  std::vector<double> matrix;

  for (size_t i = 0; i < locations.size(); i++)
  {
    for (size_t j = 0; j < locations.size(); j++)
    {
      if (i == j)
      {
        matrix.push_back(0);
      }
      else
      {
        std::vector<int> from = locations[locations_map[i]];
        std::vector<int> to = locations[locations_map[j]];
        matrix.push_back(hypot(from[1] - to[1], from[2] - to[2]));
      }
    }
  }
  return matrix;
}