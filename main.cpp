#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <time.h>
#include <vector>
#include <iostream>

#include "params.h"

// jednotky pro specifikaci pravdepodobnosti genetickych operatoru:
// 100 - procenta, 1000 - promile.
const UINT unit = 100;

#ifdef DEBUG
const UINT generations = 500000; // 0 - pocet generaci neni omezen
#else
const UINT generations = GENERATIONS; // po tomto poctu je GA zastaven
#endif

int locations[RSIZ][9];
std::vector<int> map_locations;
std::vector<int> vehicles;
UINT NUMBER_OF_VEHICLES;
UINT CAPACITY_OF_VEHICLES;

UINT _popsize = (POPSIZE & 1) ? POPSIZE + 1 : POPSIZE;

// ------------------- implementace genetickeho algoritmu ----------------------
// *****************************************************************************

GA_chromosome best; // najlepsi dosud nalezene reseni
double best_ever;   // fitness dosud nejlepsiho jedince

UINT generation; // pocitadlo generaci
GA_chromosome *population;
GA_chromosome *next_population;
// pracovni populace - parny pocet jedincov
GA_chromosome pool1[(POPSIZE & 1) ? POPSIZE + 1 : POPSIZE];
GA_chromosome pool2[(POPSIZE & 1) ? POPSIZE + 1 : POPSIZE];

// a mutaci turnajem vybranych jedincu predchazejici generace.
void evolve()
{
  // inicializace promennych
  generation = 0;
  best.fitness = 0.0;
  best_ever = 0.0;
  GA_chromosome ind1_new, ind2_new;
  UINT _tour = (TOUR >= 2 ? TOUR : 2);
  UINT i1;

  // inicializace populace
  for (UINT i = 0; i < _popsize; i++)
  {
    initialize(&pool1[i]);
    pool1[i].evaluate = 1;
  }
  // gprint(&pool1[1]);
  // for (int i = 0; i < RSIZ; i++)
  // {
  //   printf("%d: %d\n", i, pool1[1].map_route_position[i]);
  // }
  // return;
  //------------------------------------------------------------------------------------------------------------------------
  // gprint(&pool1[1]);
  // printArray(pool1[1].routes[1].locations, pool1[1].routes[1].route_length);
  // swapArrayValues(pool1[1].routes[1].locations, 2, 3);
  // printArray(pool1[1].routes[1].locations, pool1[1].routes[1].route_length);
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
    for (UINT i = 0; i < _popsize; i++)
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
    for (UINT i = 2; i < _popsize; i += 2)
    {
      GA_chromosome *ind1 = NULL, *ind2 = NULL;
      // turnajovy vyber jedincu
      for (UINT t = 0; t < _tour; t++)
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
      if (mutator(&ind1_new, PMUT))
        ind1_new.evaluate = 1;
      if (mutator(&ind2_new, PMUT))
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
  // genome = malloc( sizeof(*genome) + sizeof(Route [NUMBER_OF_VEHICLES]));
  for (size_t i = 0; i < NUMBER_OF_VEHICLES; i++)
  {
    Route route;
    route.cost = 0;
    route.distance = 0;
    route.duration = 0;
    route.route_length = 0;
    genome->routes.push_back(route);
    // genome->routes[i].locations[0] = 0;
    genome->routes[i].locations.push_back(0);
    // genome->routes[i].utilization[0] = 0;
    genome->routes[i].utilization.push_back(0);
    genome->routes[i].route_length = 1;
  }
  for (UINT i = 1; i < map_locations.size(); i += 2)
  {
    UINT vehicle = urandom(0, NUMBER_OF_VEHICLES - 1);
    UINT next = genome->routes[vehicle].route_length;

    // genome->routes[vehicle].locations[next] = i;
    genome->routes[vehicle].locations.push_back(i);
    // genome->routes[vehicle].utilization[next] = genome->routes[vehicle].utilization[next - 1] + locations[map_locations[i]][3];
    genome->routes[vehicle].utilization.push_back(genome->routes[vehicle].utilization[next - 1] + locations[map_locations[i]][3]);
    genome->map_route_position[i] = next;

    // genome->routes[vehicle].locations[next + 1] = i + 1;
    genome->routes[vehicle].locations.push_back(i + 1);
    // genome->routes[vehicle].utilization[next + 1] = genome->routes[vehicle].utilization[next] + locations[map_locations[i + 1]][3];
    genome->routes[vehicle].utilization.push_back(genome->routes[vehicle].utilization[next] + locations[map_locations[i + 1]][3]);
    genome->map_route_position[i + 1] = next + 1;

    genome->routes[vehicle].route_length += 2;
  }
  for (size_t i = 0; i < NUMBER_OF_VEHICLES; i++)
  {
    // genome->routes[i].locations[genome->routes[i].route_length] = 0;
    genome->routes[i].locations.push_back(0);
    // genome->routes[i].utilization[genome->routes[i].route_length] = 0;
    genome->routes[i].utilization.push_back(0);
    genome->routes[i].route_length++;
  }
}

// mutace
BOOL mutator(GA_chromosome *genome, UINT _pmut)
{
  if (urandom(0, unit) <= _pmut) // mutace s pravdepodobnosti _pmut
  {
    for (UINT i = 0; i < MUTAGENES; i++)
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
#ifdef DEBUG
    printf("Fitness = %f | Total distance = %.2f  in generation %d\n", best_ever, 1 / best_ever, generation);
//        gprint(&best);
#endif
  }

  if (generations > 0 && generation == generations)
  {
    printf("END; generation=%d\n", generation);
    gprint(&best);
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
      // printf("origin_x:%d, origin_y:%d, destination_x:%d, destination_y:%d\n", origin_x, origin_y, destination_x, destination_y);
      total_route_distance += calculateDistanceBetweenPoints(
          origin_x, origin_y, destination_x, destination_y);
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
    // index_2 = findValueInArray(genome->routes[vehicle_1].locations, genome->routes[vehicle_1].route_length, pickup);
    // deleteFromArray(genome->routes[vehicle_1].locations, genome->routes[vehicle_1].route_length, index_1, 0);
    // deleteFromArray(genome->routes[vehicle_1].utilization, genome->routes[vehicle_1].route_length, index_1, locations[map_locations[delivery]][3]);
    // genome->routes[vehicle_1].route_length--;
    // deleteFromArray(genome->routes[vehicle_1].locations, genome->routes[vehicle_1].route_length, index_2, 0);
    // deleteFromArray(genome->routes[vehicle_1].utilization, genome->routes[vehicle_1].route_length, index_2, locations[map_locations[pickup]][3]);
    // genome->routes[vehicle_1].route_length--;
  }
  else
  {
    pickup = value;
    delivery = value + 1;
    index_2 = genome->map_route_position[delivery];
    deleteFromRoute(genome, vehicle_1, index_2, locations[map_locations[delivery]]);
    deleteFromRoute(genome, vehicle_1, index_1, locations[map_locations[pickup]]);
    // index_2 = findValueInArray(genome->routes[vehicle_1].locations, genome->routes[vehicle_1].route_length, value + 1);
    // deleteFromArray(genome->routes[vehicle_1].locations, genome->routes[vehicle_1].route_length, index_2, 0);
    // deleteFromArray(genome->routes[vehicle_1].utilization, genome->routes[vehicle_1].route_length, index_2, locations[map_locations[pickup]][3]);
    // genome->routes[vehicle_1].route_length--;
    // deleteFromArray(genome->routes[vehicle_1].locations, genome->routes[vehicle_1].route_length, index_1, 0);
    // deleteFromArray(genome->routes[vehicle_1].utilization, genome->routes[vehicle_1].route_length, index_1, locations[map_locations[delivery]][3]);
    // genome->routes[vehicle_1].route_length--;
  }
  UINT random_index = urandom(1, v_2_size - 1);
  insertToRoute(genome, vehicle_2, random_index, pickup, locations[map_locations[pickup]]);
  insertToRoute(genome, vehicle_2, random_index + 1, delivery, locations[map_locations[delivery]]);

  // genome->routes[vehicle_2].locations[v_2_size - 1] = pickup;
  // genome->routes[vehicle_2].locations[v_2_size] = delivery;
  // genome->routes[vehicle_2].locations[v_2_size + 1] = 0;

  // genome->routes[vehicle_2].utilization[v_2_size - 1] = locations[map_locations[pickup]][3];
  // genome->routes[vehicle_2].utilization[v_2_size] = 0;
  // genome->routes[vehicle_2].utilization[v_2_size + 1] = 0;
  
  // genome->routes[vehicle_2].route_length += 2;

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

  UINT index = urandom(2, v_size - 3);
  UINT value = genome->routes[vehicle].locations[index];

  if (value % 2 == 0)
  {
    swapArrayValues(&genome->routes[vehicle].locations, index, index + 1);
    UINT value_of_switched = genome->routes[vehicle].locations[index];
    genome->routes[vehicle].utilization[index] = genome->routes[vehicle].utilization[index - 1] + locations[map_locations[value_of_switched]][3];
    genome->map_route_position[value]++;
    genome->map_route_position[value_of_switched]--;
  }
  else
  {
    swapArrayValues(&genome->routes[vehicle].locations, index, index - 1);
    UINT value_of_switched = genome->routes[vehicle].locations[index];
    genome->routes[vehicle].utilization[index - 1] = genome->routes[vehicle].utilization[index - 2] + locations[map_locations[value]][3];
    genome->map_route_position[value]--;
    genome->map_route_position[value_of_switched]++;
  }
}

// *****************************************************************************
// ------------------------------ main program -------------------------------
int main()
{

  readDataset(&map_locations, &vehicles, locations);
  NUMBER_OF_VEHICLES = vehicles[0];
  CAPACITY_OF_VEHICLES = vehicles[1];

  srand(time(0)); // random seed
  evolve();

  return 0;
}
