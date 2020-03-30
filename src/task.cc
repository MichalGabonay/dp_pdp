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

  UINT line_index = 0;
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
      for (UINT i = 0; i < tokens.size(); i++)
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

  for (UINT i = 0; i < line_index - 1; i++)
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

      // // Case if I decided to solve problem wit TW *******************
      // open_times.push_back(locations[i][4]);
      // open_times.push_back(locations[locations[i][8]][4]);
      // close_times.push_back(locations[i][5]);
      // close_times.push_back(locations[locations[i][8]][5]);
      // service_times.push_back(locations[i][6]);
      // service_times.push_back(locations[locations[i][8]][6]);
      // // **************************************************************
    }
  }

  matrix = calculateDistanceMatrix(locations_map, locations);
  matrix_order = sqrt(matrix.size());
  number_of_vehicles = vehicles[0];
  capacity_of_vehicles = vehicles[1];
  max_route_duration = MAX_ROUTE_DURATION;
  return true;
}

str2int_errno str2int(int *out, char *s, UINT base)
{
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

std::vector<double> calculateDistanceMatrix(std::vector<int> locations_map, std::vector<std::vector<int>> locations)
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