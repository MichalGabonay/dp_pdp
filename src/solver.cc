#include "solver.h"

#include <iostream>
#include <bits/stdc++.h>
#include <unistd.h>
#include <algorithm>

Solver::Solver(Task *i_task, Config *i_config)
{
  task = i_task;
  config = i_config;
}

bool Solver::Solve()
{
  if (config->CONFIG_EVOLUTION_TYPE == "ES")
  {
    pop = (GA_chromosome *)calloc(config->CONFIG_MI, sizeof(GA_chromosome));
    next_pop = (GA_chromosome *)calloc(config->CONFIG_MI, sizeof(GA_chromosome));
    offs = (GA_chromosome *)calloc(config->CONFIG_LAMBDA, sizeof(GA_chromosome));
    // int ofx[config->CONFIG_LAMBDA];
    
    generation = 0;
    best.fitness = 0.0;
    best_ever = 0.0;

    for (int i = 0; i < config->CONFIG_MI; i++)
    {
      initialize(&pop[i], this);
      pop[i].evaluate = 1;
    }

    do
    {
      generation++;
      std::map <float, int, std::greater<float>> sortMap;
      std::vector<int> ofx;

      if (config->CONFIG_ES_PLUS)
      {
        for (int i = 0; i < config->CONFIG_MI; i++) {
          sortMap[pop[i].fitness] = (i + 1) * (-1);
        }
      }
      
      // generovani LAMBDA potomku do offs
      for (int i = 0; i < config->CONFIG_LAMBDA; i++)
      {
        // selekce pro reprodukci nahodne
        offs[i] = pop[urandom(0, config->CONFIG_MI - 1)];
        mutator(&offs[i], config->unit, this, -1); // MUTAGENES postupne od 1 do LAMBDA:)
        offs[i].fitness = fitness(&offs[i], this->task);
        sortMap[offs[i].fitness] = i;
      }
      // serazeni indexu potomku podle fitness
      for (std::map<float, int>::iterator i = sortMap.begin(); i != sortMap.end(); i++)
      {
          ofx.push_back(i->second);
      }
      
      if (offs[ofx[0]].fitness >= best.fitness)
      {
        best = offs[ofx[0]];
      }
      // pop[0] = best; // elitismus
      for (int i = 0; i < config->CONFIG_MI; i++) {
        if (ofx[i] < 0)
        {
          next_pop[i] = pop[(ofx[i]+1)*(-1)];
        } else {
          next_pop[i] = offs[ofx[i]];
        }
      }
      pop = next_pop;

    } while (!stop(this->config, this));
  } else {
    // evolucni cyklus - GA ******************************************************************************************
    pool1 = (GA_chromosome *)calloc(config->CONFIG_POPSIZE, sizeof(GA_chromosome));
    pool2 = (GA_chromosome *)calloc(config->CONFIG_POPSIZE, sizeof(GA_chromosome));
    // inicializace promennych
    generation = 0;
    best.fitness = 0.0;
    best_ever = 0.0;
    GA_chromosome ind1_new, ind2_new;
    int _tour = (config->CONFIG_TOUR >= 2 ? config->CONFIG_TOUR : 2);
    UINT i1;

    // inicializace populace
    for (int i = 0; i < config->CONFIG_POPSIZE; i++)
    {
      initialize(&pool1[i], this);
      pool1[i].evaluate = 1;
    }
    
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
      for (int i = 0; i < config->CONFIG_POPSIZE; i++)
      {
        if (population[i].evaluate)
        {
          // gprint(&population[i]);
          population[i].fitness = fitness(&population[i], this->task);
          if (population[i].fitness >= best.fitness)
            best = population[i];
          population[i].evaluate = 0;
        }
      }

      // elitizmus
      next_population[0] = best; // dosud nejlepsi nalezeny jedinec...
      GA_chromosome mutant = best;
      mutator(&mutant, config->unit, this, -1);
      next_population[1] = mutant; // ...a mutant nejlepsiho
                                  // tvorba nove populace
      for (int i = 2; i < config->CONFIG_POPSIZE; i += 2)
      {
        GA_chromosome *ind1 = NULL, *ind2 = NULL;
        // turnajovy vyber jedincu
        for (int t = 0; t < _tour; t++)
        {
          i1 = urandom(0, config->CONFIG_POPSIZE - 1);
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
        if (mutator(&ind1_new, config->CONFIG_PMUT, this, -1))
          ind1_new.evaluate = 1;
        if (mutator(&ind2_new, config->CONFIG_PMUT, this, -1))
          ind2_new.evaluate = 1;
        // umisteni potomku do nove populace
        next_population[i] = ind1_new;
        next_population[i + 1] = ind2_new;
      }

    } while (!stop(this->config, this));
    // ******************************************************************************************
  }  


  return true;
}

// vypis chromozomu
void gprint(GA_chromosome *genome, Solver *solver)
{
  UINT used_vehicles = 0;
  for (int i = 0; i < solver->task->number_of_vehicles; i++)
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
  printf("TOTAL DISTANCE: %f, USED VEHICLES: %d\n", 1000 / genome->fitness, used_vehicles);
}

// random initialization of population
void initialize(GA_chromosome *genome, Solver *solver)
{
  for (int i = 0; i < solver->task->number_of_vehicles; i++)
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
  for (UINT i = 1; i < solver->task->locations_map.size(); i += 2)
  {
    UINT vehicle = urandom(0, solver->task->number_of_vehicles - 1);
    UINT next = genome->routes[vehicle].route_length;

    genome->routes[vehicle].locations.push_back(i);
    genome->routes[vehicle].utilization.push_back(genome->routes[vehicle].utilization[next - 1] + solver->task->demands[i]);
    genome->map_route_position.push_back(next);

    genome->routes[vehicle].locations.push_back(i + 1);
    genome->routes[vehicle].utilization.push_back(genome->routes[vehicle].utilization[next] + solver->task->demands[i + 1]);
    genome->map_route_position.push_back(next + 1);

    genome->routes[vehicle].route_length += 2;
  }
  for (int i = 0; i < solver->task->number_of_vehicles; i++)
  {
    genome->routes[i].locations.push_back(0);
    genome->routes[i].utilization.push_back(0);
    genome->routes[i].route_length++;
  }
}

// mutace
BOOL mutator(GA_chromosome *genome, UINT _pmut, Solver *solver, int mutagens)
{
  if (mutagens == -1)
  {
    mutagens = solver->config->CONFIG_MUTAGENES;
  }
  
  if (urandom(0, solver->config->unit) <= _pmut) // mutace s pravdepodobnosti _pmut
  {
    for (int i = 0; i < mutagens; i++)
    {
      switch (urandom(1, 2))
      {
      case 1:
        mutatorMoveBetweenVehicles(genome, solver);
        break;
      case 2:
        mutatorChangeRouteSchedule(genome, solver);
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
BOOL stop(Config *config, Solver *solver)
{
  if (config->CONFIG_GENERATIONS_PRINT)
  {
    if (solver->generation % 1000 == 0 || solver->generation == 1)
    {
      std::ostringstream os;
      os << getpid() << ";";
      int popsize;
      if (solver->config->CONFIG_EVOLUTION_TYPE == "ES")
      {
        popsize = solver->config->CONFIG_MI;
      } else {
        popsize = solver->config->CONFIG_POPSIZE;
      }
      
      for (int i = 0; i < popsize; i++)
      {
        if (solver->config->CONFIG_EVOLUTION_TYPE == "ES")
        {
          os << solver->pop[i].fitness;
        } else {
          os << solver->population[i].fitness;
        }
        if (i != popsize - 1)
        {
          os << ";";
        }
      }

      std::string gen_summary = os.str();
      printf("%s\n", gen_summary.c_str());
    }
  }

  if (solver->best.fitness > solver->best_ever)
  {
    solver->best_ever = solver->best.fitness;
    if (config->CONFIG_DEBUG)
    {
      printf("Fitness = %f | Total distance = %.2f  in generation %d\n", solver->best_ever, 1000 / solver->best_ever, solver->generation);
    }
  }

  if (config->CONFIG_GENERATIONS > 0 && solver->generation == config->CONFIG_GENERATIONS)
  {
    if (config->CONFIG_DEBUG)
    {
      // printf("END; generation=%d\n", solver->generation);
      // gprint(&solver->best, solver);
      std::cout << "PMUT:" << config->CONFIG_PMUT << "; ";
      std::cout << "MUTAGENES:" << config->CONFIG_MUTAGENES << "; ";
      std::cout << "TOUR:" << config->CONFIG_TOUR << "; ";
      std::cout << "POPSIZE:" << config->CONFIG_POPSIZE << "; ";
      std::cout << "GENERATIONS:" << config->CONFIG_GENERATIONS << "; ";
      // std::cout << "DURATION:" << duration << "ms" << std::endl;
    }
    return 1;
  }

  return 0;
}

// evaluace fitness pro zadaneho jedince
double fitness(GA_chromosome *genome, Task *task)
{
  double total_distance = 0;
  double total_route_distance;

  for (int i = 0; i < task->number_of_vehicles; i++)
  {
    total_route_distance = 0;
    for (size_t j = 0; j < genome->routes[i].route_length - 1; j++)
    {
      UINT loc_o = genome->routes[i].locations[j];
      UINT loc_d = genome->routes[i].locations[j + 1];
      total_route_distance += task->matrix[loc_o * task->matrix_order + loc_d];
    }

    genome->routes[i].distance = total_route_distance;

    total_distance += total_route_distance;
  }

  genome->cost = total_distance;

  return 1000 / total_distance;
}

void mutatorMoveBetweenVehicles(GA_chromosome *genome, Solver *solver)
{
  UINT pickup, delivery, index_2, vehicle_1;
  UINT v_1_size = 0;

  while (v_1_size <= 2)
  {
    vehicle_1 = urandom(0, solver->task->number_of_vehicles - 1);
    v_1_size = genome->routes[vehicle_1].route_length;
  }

  UINT vehicle_2 = urandom(0, solver->task->number_of_vehicles - 1);
  if (vehicle_1 == vehicle_2)
  {
    vehicle_2 = (vehicle_1 + 1) % solver->task->number_of_vehicles;
  }

  UINT v_2_size = genome->routes[vehicle_2].route_length;

  UINT index_1 = urandom(1, v_1_size - 2);
  UINT value = genome->routes[vehicle_1].locations[index_1];

  if (value % 2 == 0)
  {
    pickup = value - 1;
    delivery = value;
    index_2 = genome->map_route_position[pickup];
    deleteFromRoute(genome, vehicle_1, index_1, solver->task->demands[delivery]);
    deleteFromRoute(genome, vehicle_1, index_2, solver->task->demands[pickup]);
  }
  else
  {
    pickup = value;
    delivery = value + 1;
    index_2 = genome->map_route_position[delivery];
    deleteFromRoute(genome, vehicle_1, index_2, solver->task->demands[delivery]);
    deleteFromRoute(genome, vehicle_1, index_1, solver->task->demands[pickup]);
  }
  UINT random_index = urandom(1, v_2_size - 1);
  insertToRoute(genome, vehicle_2, random_index, pickup, solver->task->demands[pickup], solver->task->capacity_of_vehicles);
  insertToRoute(genome, vehicle_2, random_index + 1, delivery, solver->task->demands[delivery], solver->task->capacity_of_vehicles);
}

void mutatorChangeRouteSchedule(GA_chromosome *genome, Solver *solver)
{
  int route1 = selectRoute(genome, solver->task->number_of_vehicles);
  if (route1 == -1)
    return; // finding of route was not successful

  for (int i = 0; i < solver->config->CONFIG_MUTAGENE_PER_ROUTE; i++)
  {
  // swapNeighborsInRoute(genome, route1, solver->task->capacity_of_vehicles, solver->task->demands);
    swapLocations(genome, route1, solver->task->capacity_of_vehicles, solver->task->demands);
  }
}

void test(GA_chromosome *genome, Solver *solver)
{
  // std::cout << solver->task->capacity_of_vehicles << std::endl;
  mutatorChangeRouteSchedule(genome, solver);
  // exit(0);
}