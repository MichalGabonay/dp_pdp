#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <time.h>
#include <vector>
#include <iostream>
#include <fstream>
#include <chrono>

#include "params.h"

bool CONFIG_DEBUG = false;
int CONFIG_PMUT = 60;
int CONFIG_MUTAGENES = 5;
int CONFIG_TOUR = 4;
int CONFIG_POPSIZE = 16;
int CONFIG_GENERATIONS = 100000;
std::string INPUT_FILE = "../dataset/lc101.txt";
std::chrono::_V2::system_clock::time_point time_beggining;
std::chrono::_V2::system_clock::time_point time_end;

// jednotky pro specifikaci pravdepodobnosti genetickych operatoru:
// 100 - procenta, 1000 - promile.
const UINT unit = 100;

// #ifdef DEBUG
// int generations = CONFIG_GENERATIONS; // 0 - pocet generaci neni omezen
// #else
// int generations = CONFIG_GENERATIONS; // po tomto poctu je GA zastaven
// #endif

std::vector<std::vector<int>> locations;
std::vector<int> map_locations;
std::vector<int> vehicles;
std::vector<double> matrix;
UINT NUMBER_OF_VEHICLES;
UINT CAPACITY_OF_VEHICLES;

int _popsize = CONFIG_POPSIZE;

// ------------------- implementace genetickeho algoritmu ----------------------
// *****************************************************************************

GA_chromosome best; // najlepsi dosud nalezene reseni
double best_ever;   // fitness dosud nejlepsiho jedince

int generation; // pocitadlo generaci
GA_chromosome *population;
GA_chromosome *next_population;
// pracovni populace - parny pocet jedincov
GA_chromosome *pool1;
GA_chromosome *pool2;

// a mutaci turnajem vybranych jedincu predchazejici generace.
void evolve()
{
  pool1 = (GA_chromosome *)calloc(_popsize, sizeof(GA_chromosome));
  pool2 = (GA_chromosome *)calloc(_popsize, sizeof(GA_chromosome));
  // inicializace promennych
  generation = 0;
  best.fitness = 0.0;
  best_ever = 0.0;
  GA_chromosome ind1_new, ind2_new;
  int _tour = (CONFIG_TOUR >= 2 ? CONFIG_TOUR : 2);
  UINT i1;

  // inicializace populace
  for (int i = 0; i < _popsize; i++)
  {
    initialize(&pool1[i]);
    pool1[i].evaluate = 1;
  }

  //------------------------------------------------------------------------------------------------------------------------
  // mutatorChangeRouteSchedule(&pool1[1]);
  // exit(0);
  //--------------------------------------------------------------------------------------------------------------------------

  // evolucni cyklus
  do
  {
    generation++;
    if (generation & 1)
    {
      population = pool1;
      next_population = pool2;
    }
    else
    {
      population = pool2;
      next_population = pool1;
    }
    // ohodnoceni populace
    for (int i = 0; i < _popsize; i++)
    {
      if (population[i].evaluate)
      {
        // gprint(&population[i]);
        population[i].fitness = fitness(&population[i]);
        if (population[i].fitness >= best.fitness)
          best = population[i];
        population[i].evaluate = 0;
      }
    }

    // elitizmus
    next_population[0] = best; // dosud nejlepsi nalezeny jedinec...
    GA_chromosome mutant = best;
    mutator(&mutant, unit);
    next_population[1] = mutant; // ...a mutant nejlepsiho
                                 // tvorba nove populace
    for (int i = 2; i < _popsize; i += 2)
    {
      GA_chromosome *ind1 = NULL, *ind2 = NULL;
      // turnajovy vyber jedincu
      for (int t = 0; t < _tour; t++)
      {
        i1 = urandom(0, _popsize - 1);
        if (ind1 == NULL)
          ind1 = &population[i1];
        else if (ind2 == NULL)
          ind2 = &population[i1];
        else if (population[i1].fitness > ind1->fitness)
          ind1 = &population[i1];
        else if (population[i1].fitness > ind2->fitness)
          ind2 = &population[i1];
      }
      ind1_new = *ind1;
      ind2_new = *ind2;
      // mutace
      if (mutator(&ind1_new, CONFIG_PMUT))
        ind1_new.evaluate = 1;
      if (mutator(&ind2_new, CONFIG_PMUT))
        ind2_new.evaluate = 1;
      // umisteni potomku do nove populace
      next_population[i] = ind1_new;
      next_population[i + 1] = ind2_new;
    }
  } while (!stop());
}

// *****************************************************************************
// --------------- geneticke operatory a podpurne funkce pro GA ----------------

// generuje cele cislo v rozsahu low-high vcetne
UINT urandom(UINT low, UINT high)
{
  return rand() % (high - low + 1) + low;
}

// vypis chromozomu
void gprint(GA_chromosome *genome)
{
  UINT used_vehicles = 0;
  for (UINT i = 0; i < NUMBER_OF_VEHICLES; i++)
  {
    if (genome->routes[i].route_length > 2)
    {

      printf("Vehicle %d (%d): ", i, genome->routes[i].route_length);
      for (size_t j = 0; j < genome->routes[i].route_length; j++)
      {
        if (j != genome->routes[i].route_length - 1)
        {
          printf("%d[%d], ", genome->routes[i].locations[j], genome->routes[i].utilization[j]);
        }
        else
        {
          printf("%d[%d]", genome->routes[i].locations[j], genome->routes[i].utilization[j]);
        }
      }
      printf("\n");
      printf("cost: %d  --  duration: %d  --  distance: %d \n\n", genome->routes[i].cost, genome->routes[i].duration, genome->routes[i].distance);
      used_vehicles++;
    }
  }
  printf("TOTAL DISTANCE: %f, USED VEHICLES: %d\n", 1 / genome->fitness, used_vehicles);
}

// random initialization of population
void initialize(GA_chromosome *genome)
{
  for (size_t i = 0; i < NUMBER_OF_VEHICLES; i++)
  {
    Route route;
    route.cost = 0;
    route.distance = 0;
    route.duration = 0;
    route.route_length = 0;
    genome->routes.push_back(route);
    genome->routes[i].locations.push_back(0);
    genome->routes[i].utilization.push_back(0);
    genome->routes[i].route_length = 1;
  }
  genome->map_route_position.push_back(0);
  for (UINT i = 1; i < map_locations.size(); i += 2)
  {
    UINT vehicle = urandom(0, NUMBER_OF_VEHICLES - 1);
    UINT next = genome->routes[vehicle].route_length;

    genome->routes[vehicle].locations.push_back(i);
    genome->routes[vehicle].utilization.push_back(genome->routes[vehicle].utilization[next - 1] + locations[map_locations[i]][3]);
    genome->map_route_position.push_back(next);

    genome->routes[vehicle].locations.push_back(i + 1);
    genome->routes[vehicle].utilization.push_back(genome->routes[vehicle].utilization[next] + locations[map_locations[i + 1]][3]);
    genome->map_route_position.push_back(next + 1);

    genome->routes[vehicle].route_length += 2;
  }
  for (size_t i = 0; i < NUMBER_OF_VEHICLES; i++)
  {
    genome->routes[i].locations.push_back(0);
    genome->routes[i].utilization.push_back(0);
    genome->routes[i].route_length++;
  }
}

// mutace
BOOL mutator(GA_chromosome *genome, UINT _pmut)
{
  if (urandom(0, unit) <= _pmut) // mutace s pravdepodobnosti _pmut
  {
    for (int i = 0; i < CONFIG_MUTAGENES; i++)
    {
      switch (urandom(1, NUMBER_OF_MUTATORS))
      {
      case 1:
        mutatorMoveBetweenVehicles(genome);
        break;
      case 2:
        mutatorChangeRouteSchedule(genome);
        break;
      default:
        break;
      }
    }
    return 1; // probehla-li mutace, vratim true...
  }

  return 0; // ...jinak vracim false
}

// test na zastaveni evoluce
BOOL stop()
{
  if (best.fitness > best_ever)
  {
    best_ever = best.fitness;
    // #ifdef DEBUG
    if (CONFIG_DEBUG)
    {
      printf("Fitness = %f | Total distance = %.2f  in generation %d\n", best_ever, 1 / best_ever, generation);
    }
    // #endif
  }

  if (CONFIG_GENERATIONS > 0 && generation == CONFIG_GENERATIONS)
  {
    time_end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(time_end - time_beggining).count();
    duration = duration / 1000; // in miliseconds
    // #ifdef DEBUG
    if (CONFIG_DEBUG)
    {
      printf("END; generation=%d\n", generation);
      gprint(&best);
      std::cout << "PMUT:" << CONFIG_PMUT << "; ";
      std::cout << "MUTAGENES:" << CONFIG_MUTAGENES << "; ";
      std::cout << "TOUR:" << CONFIG_TOUR << "; ";
      std::cout << "POPSIZE:" << CONFIG_POPSIZE << "; ";
      std::cout << "GENERATIONS:" << CONFIG_GENERATIONS << "; ";
      std::cout << "DURATION:" << duration << "ms" << std::endl;
    }
    else
    {
      // #else
      printProgramReport(&best, duration);
      gprint(&best);
    }
    // #endif
    return 1;
  }

  return 0;
}

// evaluace fitness pro zadaneho jedince
double fitness(GA_chromosome *genome)
{
  // gprint(genome);
  double total_distance = 0;
  double total_route_distance;

  for (UINT i = 0; i < NUMBER_OF_VEHICLES; i++)
  {
    total_route_distance = 0;
    for (size_t j = 0; j < genome->routes[i].route_length - 1; j++)
    {
      UINT loc_o = genome->routes[i].locations[j];
      UINT loc_d = genome->routes[i].locations[j + 1];
      // printf("j=%d(%d): loc_o: %d, loc_d: %d, l: %d\n", (int)j, genome->routes[i].route_length, loc_o, loc_d, map_locations[loc_o]);

      int origin_x = locations[map_locations[loc_o]][1];
      int origin_y = locations[map_locations[loc_o]][2];
      int destination_x = locations[map_locations[loc_d]][1];
      int destination_y = locations[map_locations[loc_d]][2];
      printf("origin_x:%d, origin_y:%d, destination_x:%d, destination_y:%d\n", origin_x, origin_y, destination_x, destination_y);
      // total_route_distance += calculateDistanceBetweenPoints(
      //     origin_x, origin_y, destination_x, destination_y);

      // std::cout << map_locations.size() << std::endl;
      // std::cout << loc_o*map_locations.size() + loc_d << std::endl;
      // std::cout << loc_o << " " << loc_d << std::endl;
      // std::cout << matrix.size() << std::endl;
      // std::cout << matrix[loc_o*map_locations.size() + loc_d] << std::endl;

      std::cout << calculateDistanceBetweenPoints(origin_x, origin_y, destination_x, destination_y) << " == " << matrix[loc_o*map_locations.size() + loc_d] << std::endl;

      total_route_distance += matrix[loc_o*map_locations.size() + loc_d];
    }

    genome->routes[i].distance = total_route_distance;
    total_distance += total_route_distance;
  }

  return 1 / total_distance;
}

void mutatorMoveBetweenVehicles(GA_chromosome *genome)
{
  UINT pickup, delivery, index_2, vehicle_1;
  UINT v_1_size = 0;

  while (v_1_size <= 2)
  {
    vehicle_1 = urandom(0, NUMBER_OF_VEHICLES - 1);
    v_1_size = genome->routes[vehicle_1].route_length;
  }

  UINT vehicle_2 = urandom(0, NUMBER_OF_VEHICLES - 1);
  if (vehicle_1 == vehicle_2)
  {
    vehicle_2 = (vehicle_1 + 1) % NUMBER_OF_VEHICLES;
  }

  UINT v_2_size = genome->routes[vehicle_2].route_length;

  //------------------------------------------------------------------------------------------------------
  // printf("OLD:\n");
  // printArray(genome->routes[vehicle_1].locations, genome->routes[vehicle_1].route_length);
  // printArray(genome->routes[vehicle_2].locations, genome->routes[vehicle_2].route_length);
  //------------------------------------------------------------------------------------------------------

  UINT index_1 = urandom(1, v_1_size - 2);
  UINT value = genome->routes[vehicle_1].locations[index_1];

  if (value % 2 == 0)
  {
    pickup = value - 1;
    delivery = value;
    index_2 = genome->map_route_position[pickup];
    deleteFromRoute(genome, vehicle_1, index_1, locations[map_locations[delivery]]);
    deleteFromRoute(genome, vehicle_1, index_2, locations[map_locations[pickup]]);
  }
  else
  {
    pickup = value;
    delivery = value + 1;
    index_2 = genome->map_route_position[delivery];
    deleteFromRoute(genome, vehicle_1, index_2, locations[map_locations[delivery]]);
    deleteFromRoute(genome, vehicle_1, index_1, locations[map_locations[pickup]]);
  }
  UINT random_index = urandom(1, v_2_size - 1);
  UINT inserted_to_index = insertToRoute(genome, vehicle_2, random_index, pickup, locations[map_locations[pickup]], CAPACITY_OF_VEHICLES);
  insertToRoute(genome, vehicle_2, inserted_to_index + 1, delivery, locations[map_locations[delivery]], CAPACITY_OF_VEHICLES);

  //------------------------------------------------------------------------------------------------------
  // printf("NEW:\n");
  // printArray(genome->routes[vehicle_1].locations, genome->routes[vehicle_1].route_length);
  // printArray(genome->routes[vehicle_2].locations, genome->routes[vehicle_2].route_length);
  //------------------------------------------------------------------------------------------------------
}

void mutatorChangeRouteSchedule(GA_chromosome *genome)
{

  UINT vehicle;
  UINT v_size = 0;
  while (v_size <= 4)
  {
    vehicle = urandom(0, NUMBER_OF_VEHICLES - 1);
    v_size = genome->routes[vehicle].route_length;
  }

  // ****************
  // printf("Vehicle %d (%d): ", vehicle, genome->routes[vehicle].route_length);
  // for (size_t j = 0; j < genome->routes[vehicle].route_length; j++)
  // {
  //   if (j != genome->routes[vehicle].route_length - 1)
  //   {
  //     printf("%d[%d], ", genome->routes[vehicle].locations[j], genome->routes[vehicle].utilization[j]);
  //   }
  //   else
  //   {
  //     printf("%d[%d]", genome->routes[vehicle].locations[j], genome->routes[vehicle].utilization[j]);
  //   }
  // }
  // printf("\n");
  // ****************

  swapNeighborsInRoute(genome, vehicle, CAPACITY_OF_VEHICLES, locations, map_locations);

  // ****************
  // printf("Vehicle %d (%d): ", vehicle, genome->routes[vehicle].route_length);
  // for (size_t j = 0; j < genome->routes[vehicle].route_length; j++)
  // {
  //   if (j != genome->routes[vehicle].route_length - 1)
  //   {
  //     printf("%d[%d], ", genome->routes[vehicle].locations[j], genome->routes[vehicle].utilization[j]);
  //   }
  //   else
  //   {
  //     printf("%d[%d]", genome->routes[vehicle].locations[j], genome->routes[vehicle].utilization[j]);
  //   }
  // }
  // printf("\n");
  // ****************

  // UINT index = urandom(2, v_size - 3);
  // UINT value = genome->routes[vehicle].locations[index];

  // if (value % 2 == 0)
  // {
  //   swapArrayValues(&genome->routes[vehicle].locations, index, index + 1);
  //   UINT value_of_switched = genome->routes[vehicle].locations[index];
  //   genome->routes[vehicle].utilization[index] = genome->routes[vehicle].utilization[index - 1] + locations[map_locations[value_of_switched]][3];
  //   genome->map_route_position[value]++;
  //   genome->map_route_position[value_of_switched]--;
  // }
  // else
  // {
  //   swapArrayValues(&genome->routes[vehicle].locations, index, index - 1);
  //   UINT value_of_switched = genome->routes[vehicle].locations[index];
  //   genome->routes[vehicle].utilization[index - 1] = genome->routes[vehicle].utilization[index - 2] + locations[map_locations[value]][3];
  //   genome->map_route_position[value]--;
  //   genome->map_route_position[value_of_switched]++;
  // }
}

cxxopts::ParseResult
parse(int argc, char *argv[])
{
  try
  {
    cxxopts::Options options(argv[0], " - example command line options");
    options
        .positional_help("[optional args]")
        .show_positional_help();

    options
        .allow_unrecognised_options()
        .add_options()("h,help", "Print help")("d,debug", "Debugging")("p,pmut", "Probability of mutation", cxxopts::value<int>(), "N")("m,mutagenes", "Number of mutated genes in the chromosome", cxxopts::value<int>(), "N")("t,tour", "Number of individuals in tournament selection", cxxopts::value<int>(), "N")("s,popsize", "Population size", cxxopts::value<int>(), "N")("g,generations", "Maximum number of generations", cxxopts::value<int>(), "N")("i,input_file", "Input file path", cxxopts::value<std::string>());

    auto result = options.parse(argc, argv);

    if (result.count("help"))
    {
      std::cout << options.help({"", "Group"}) << std::endl;
      exit(0);
    }
    if (result.count("debug"))
    {
      CONFIG_DEBUG = true;
    }
    if (result.count("input_file"))
    {
      INPUT_FILE = result["input_file"].as<std::string>();
    }

    if (result.count("pmut"))
    {
      CONFIG_PMUT = result["pmut"].as<int>();
    }
    else
    {
      CONFIG_PMUT = 60;
    }
    if (result.count("mutagenes"))
    {
      CONFIG_MUTAGENES = result["mutagenes"].as<int>();
    }
    else
    {
      CONFIG_MUTAGENES = 5;
    }
    if (result.count("tour"))
    {
      CONFIG_TOUR = result["tour"].as<int>();
    }
    else
    {
      CONFIG_TOUR = 4;
    }
    if (result.count("popsize"))
    {
      CONFIG_POPSIZE = result["popsize"].as<int>();
      _popsize = (CONFIG_POPSIZE & 1) ? CONFIG_POPSIZE + 1 : CONFIG_POPSIZE;
    }
    // else
    // {
    //   CONFIG_POPSIZE = 15;
    // }
    if (result.count("generations"))
    {
      CONFIG_GENERATIONS = result["generations"].as<int>();
    }
    // else
    // {
    //   CONFIG_GENERATIONS = 2000000;
    // }

    return result;
  }
  catch (const cxxopts::OptionException &e)
  {
    std::cout << "error parsing options: " << e.what() << std::endl;
    exit(1);
  }
}

void printProgramReport(GA_chromosome *best, float computed_time)
{
  std::ofstream out;
  out.open("report.csv", std::ios::app);

  UINT used_vehicles = 0;
  for (UINT i = 0; i < NUMBER_OF_VEHICLES; i++)
  {
    if (best->routes[i].route_length > 2)
    {
      used_vehicles++;
    }
  }
  float TOTAL_DISTANCE = (int)((1 / best->fitness) * 100 + .5);
  std::stringstream output_ss;

  output_ss << CONFIG_PMUT << ';';
  output_ss << CONFIG_MUTAGENES << ';';
  output_ss << CONFIG_TOUR << ';';
  output_ss << CONFIG_POPSIZE << ';';
  output_ss << CONFIG_GENERATIONS << ';';
  output_ss << (float)TOTAL_DISTANCE / 100 << ';';
  output_ss << used_vehicles << ';';
  output_ss << computed_time << ';';
  output_ss << std::endl;

  std::string output_string = output_ss.str();
  out << output_string;
  std::cout << output_string;
}

double Travel(UINT from, UINT to, int matrix_order, std::vector<double> matrix){
  return matrix[from*matrix_order + to];
}

// *****************************************************************************
// ------------------------------ main program -------------------------------
int main(int argc, char *argv[])
{
  auto result = parse(argc, argv);
  auto arguments = result.arguments();

  readDataset(&map_locations, &vehicles, &locations, INPUT_FILE, &matrix);
  NUMBER_OF_VEHICLES = vehicles[0];
  CAPACITY_OF_VEHICLES = vehicles[1];

  // std::cout << NUMBER_OF_VEHICLES << std::endl;
  // std::cout << CAPACITY_OF_VEHICLES << std::endl;

  // for (size_t i = 0; i < locations.size(); i++)
  // {
  //   for (size_t j = 0; j < locations[i].size(); j++)
  //   {
  //     if (j != locations[i].size() - 1)
  //     {
  //         printf("%d, ", locations[i][j]);
  //     }
  //     else
  //     {
  //         printf("%d", locations[i][j]);
  //     }
  //   }
  //   std::cout << std::endl; 
  // }
  // std::cout << std::endl;

  srand(955031225); // random seed
  time_beggining = std::chrono::high_resolution_clock::now();
  evolve();

  return 0;
}
