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

Solver::~Solver()
{
  if (config->CONFIG_EVOLUTION_TYPE == "ES")
  {
    free(pop);
    free(next_pop);
    free(offs);
  } else {
    free(pool1);
    free(pool2);
  }
}

bool Solver::Solve()
{
  if (config->CONFIG_EVOLUTION_TYPE == "ES")
  {
    pop = (Chromosome *)calloc(config->CONFIG_MI, sizeof(Chromosome));
    next_pop = (Chromosome *)calloc(config->CONFIG_MI, sizeof(Chromosome));
    offs = (Chromosome *)calloc(config->CONFIG_LAMBDA, sizeof(Chromosome));
    
    generation = 0;
    best.fitness = 0.0;
    best_ever = 0.0;

    for (int i = 0; i < config->CONFIG_MI; i++)
    {
      initialize(&pop[i]);
      pop[i].evaluate = 1;
    }

    // mutatorGuidedChange(&pop[0], this->task);

    // printRoute(pop[0].routes[0], 0);

    // for (size_t i = 0; i < 1000000; i++)
    // {
      // inserCustomerToRoute(&pop[0], 0, 9, this->task);
    // }
    // printRoute(pop[0].routes[0], 0);
    

    // std::map<int, int> m;
    // for(int n=0; n<10000; ++n) {
    //   ++m[selectCustomerByCost(&pop[0].routes[0], pop[0].map_route_position)];
    // }
    // for(auto p : m) {
    //     std::cout << p.first << " generated " << p.second << " times\n";
    //     // std::cout << p.first << " generated " << p.second << " times\n";
    // }
    // exit(1);

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
        offs[i] = pop[this->selectIndividByWeight()];
        // mutator(&offs[i], config->unit, this, -1); // MUTAGENES postupne od 1 do LAMBDA:)
        mutator(&offs[i], config->unit, i%config->CONFIG_MUTAGENES + 1); // MUTAGENES postupne od 1 do LAMBDA:)
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
      int start = 0;
      if (config->CONFIG_ES_ELITISM)
      {
        pop[0] = best; // elitismus
        start = 1;
      }
      if (config->CONFIG_ES_PLUS)
      {
        for (int i = start; i < config->CONFIG_MI; i++) {
          if (ofx[i] < 0)
          {
            next_pop[i] = pop[(ofx[i]+1)*(-1)];
          } else {
            next_pop[i] = offs[ofx[i]];
          }
        }
        pop = next_pop;
      } else {
        for (int i = start; i < config->CONFIG_MI; i++) {
          if (config->CONFIG_ES_ELITISM)
          {
            pop[i] = offs[ofx[i-1]];
          } else {
            pop[i] = offs[ofx[i]];
          }
        }
      }

    } while (!stop());
  } else {
    // evolucni cyklus - GA ******************************************************************************************
    pool1 = (Chromosome *)calloc(config->CONFIG_POPSIZE, sizeof(Chromosome));
    pool2 = (Chromosome *)calloc(config->CONFIG_POPSIZE, sizeof(Chromosome));
    // inicializace promennych
    generation = 0;
    best.fitness = 0.0;
    best_ever = 0.0;
    Chromosome ind1_new, ind2_new;
    int _tour = (config->CONFIG_TOUR >= 2 ? config->CONFIG_TOUR : 2);
    UINT i1;

    // inicializace populace
    for (int i = 0; i < config->CONFIG_POPSIZE; i++)
    {
      initialize(&pool1[i]);
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
      Chromosome mutant = best;
      mutator(&mutant, config->unit, -1);
      next_population[1] = mutant; // ...a mutant nejlepsiho
                                  // tvorba nove populace
      for (int i = 2; i < config->CONFIG_POPSIZE; i += 2)
      {
        Chromosome *ind1 = NULL, *ind2 = NULL;
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
        if (mutator(&ind1_new, config->CONFIG_PMUT, -1))
          ind1_new.evaluate = 1;
        if (mutator(&ind2_new, config->CONFIG_PMUT, -1))
          ind2_new.evaluate = 1;
        // umisteni potomku do nove populace
        next_population[i] = ind1_new;
        next_population[i + 1] = ind2_new;
      }

    } while (!stop());
    // ******************************************************************************************
  }  


  return true;
}

// vypis chromozomu
void Solver::gprint(Chromosome *genome)
{
  UINT used_vehicles = 0;
  for (int i = 0; i < this->task->number_of_vehicles; i++)
  {
    if (genome->routes[i].route_length > 2)
    {
      printf("Vehicle %d (%d): ", i, genome->routes[i].route_length);
      for (size_t j = 0; j < genome->routes[i].route_length; j++)
      {
        if (j != genome->routes[i].route_length - 1)
        {
          printf("%d[%d][%f], ", genome->routes[i].locations[j], genome->routes[i].utilization[j], genome->routes[i].cuml_duration[j]);
        }
        else
        {
          printf("%d[%d][%f]", genome->routes[i].locations[j], genome->routes[i].utilization[j], genome->routes[i].cuml_duration[j]);
        }
      }
      printf("\n");
      printf("cost: %d  --  duration: %f  --  distance: %d \n\n", genome->routes[i].cost, genome->routes[i].duration, genome->routes[i].distance);
      used_vehicles++;
    }
  }
  printf("TOTAL DISTANCE: %f, USED VEHICLES: %d\n", 1000 / genome->fitness, used_vehicles);
}

// random initialization of population
void Solver::initialize(Chromosome *genome)
{
  for (int i = 0; i < this->task->number_of_vehicles; i++)
  {
    Route route;
    route.cost = 0;
    route.distance = 0;
    route.duration = 0;
    route.route_length = 0;
    
    genome->routes.push_back(route);
    genome->routes[i].locations.push_back(0);
    genome->routes[i].utilization.push_back(0);
    genome->routes[i].cuml_duration.push_back(0);
    genome->routes[i].route_length = 1;
    genome->routes[i].cost_in.push_back(0);
  }
  genome->map_route_position.push_back(0);
  for (UINT i = 1; i < this->task->locations_map.size(); i += 2)
  {
    bool succes_insert = false;
    // UINT vehicle = urandom(0, this->task->number_of_vehicles - 1);
    UINT vehicle = 0;
    int tries = 0;
    while (!succes_insert && tries < this->task->number_of_vehicles)
    {
      tries++;
      UINT prev = genome->routes[vehicle].route_length - 1;
      UINT next = genome->routes[vehicle].route_length;

      double route_to_pickup = this->task->matrix[genome->routes[vehicle].locations[prev] * this->task->matrix_order + i];
      double route_to_delivery = this->task->matrix[i * this->task->matrix_order + i + 1];
      double route_to_depot = this->task->matrix[(i + 1) * this->task->matrix_order];
      if (genome->routes[vehicle].duration + route_to_pickup + route_to_delivery + route_to_depot > this->task->max_route_duration)
      {
        vehicle = (vehicle + 1) % this->task->number_of_vehicles;
        continue;
      } else {
        succes_insert = true;
      }
      genome->routes[vehicle].cost_out.push_back(route_to_pickup); // cost out from previous location
      genome->routes[vehicle].cost_in.push_back(route_to_pickup);
    
      genome->routes[vehicle].locations.push_back(i);
      genome->routes[vehicle].duration += route_to_pickup;
      genome->routes[vehicle].utilization.push_back(genome->routes[vehicle].utilization[next - 1] + this->task->demands[i]);
      genome->routes[vehicle].cuml_duration.push_back(genome->routes[vehicle].duration);
      genome->map_route_position.push_back(next);

      genome->routes[vehicle].cost_out.push_back(route_to_delivery); // cost out from previous location
      genome->routes[vehicle].cost_in.push_back(route_to_delivery);

      genome->routes[vehicle].locations.push_back(i + 1);
      genome->routes[vehicle].duration += route_to_delivery;
      genome->routes[vehicle].utilization.push_back(genome->routes[vehicle].utilization[next] + this->task->demands[i + 1]);
      genome->routes[vehicle].cuml_duration.push_back(genome->routes[vehicle].duration);
      genome->map_route_position.push_back(next + 1);

      genome->routes[vehicle].route_length += 2;
    }    
    if (succes_insert == false)
    {
      std::cout << "Unsuccesful inicialization, try bigger max route duration." << std::endl;
      exit(1);
    }
  }
  for (int i = 0; i < this->task->number_of_vehicles; i++)
  {
    int last_customer = genome->routes[i].route_length - 1;
    double route_to_depot = this->task->matrix[genome->routes[i].locations[last_customer] * this->task->matrix_order];
    genome->routes[i].cost_out.push_back(route_to_depot); // cost out from previous location
    genome->routes[i].cost_in.push_back(route_to_depot);
    genome->routes[i].cost_out.push_back(0);
    genome->routes[i].duration += route_to_depot;
    genome->routes[i].cuml_duration.push_back(genome->routes[i].duration);
    genome->routes[i].locations.push_back(0);
    genome->routes[i].utilization.push_back(0);
    genome->routes[i].route_length++;
  }
  // for (UINT i = 1; i < this->task->locations_map.size(); i += 2) {
  //   UINT vehicle = urandom(0, this->task->number_of_vehicles - 1);
  //   printRoute(genome->routes[vehicle], vehicle);
  //   inserCustomerToRoute(genome, vehicle, i, this->task);
  // }
}

// test na zastaveni evoluce
BOOL Solver::stop()
{
  if (config->CONFIG_GENERATIONS_PRINT)
  {
    if (this->generation % 1000 == 0 || this->generation == 1)
    {
      std::ostringstream os;
      os << getpid() << ";";
      int popsize;
      if (this->config->CONFIG_EVOLUTION_TYPE == "ES")
      {
        popsize = this->config->CONFIG_MI;
      } else {
        popsize = this->config->CONFIG_POPSIZE;
      }
      
      for (int i = 0; i < popsize; i++)
      {
        if (this->config->CONFIG_EVOLUTION_TYPE == "ES")
        {
          os << this->pop[i].fitness;
        } else {
          os << this->population[i].fitness;
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

  if (this->best.fitness > this->best_ever)
  {
    this->best_ever = this->best.fitness;
    if (config->CONFIG_DEBUG)
    {
      printf("Fitness = %f | Total distance = %.2f  in generation %d\n", this->best_ever, 1000 / this->best_ever, this->generation);
    }
  }
  // if (config->CONFIG_DEBUG && this->generation % 1000 == 0)
  // {
  //   printf("Fitness = %f | Total distance = %.2f  in generation %d\n", this->best_ever, 1000 / this->best_ever, this->generation);
  // }

  if (config->CONFIG_GENERATIONS > 0 && this->generation == config->CONFIG_GENERATIONS)
  {
    if (config->CONFIG_DEBUG)
    {
      // printf("END; generation=%d\n", this->generation);
      gprint(&this->best);
      std::cout << "ELITISM:" << config->CONFIG_ES_ELITISM << "; ";
      std::cout << "PLUS:" << config->CONFIG_ES_PLUS << "; ";
      std::cout << "MUTAGENES:" << config->CONFIG_MUTAGENES << "; ";
      std::cout << "MUTAGENE_PER_ROUTE:" << config->CONFIG_MUTAGENE_PER_ROUTE << "; ";
      std::cout << "LAMBDA:" << config->CONFIG_LAMBDA << "; ";
      std::cout << "MI:" << config->CONFIG_MI << "; ";
      std::cout << "GENERATIONS:" << config->CONFIG_GENERATIONS << "; " << std::endl;
      // std::cout << "DURATION:" << duration << "ms" << std::endl;
    }
    return 1;
  }

  return 0;
}

// evaluace fitness pro zadaneho jedince
double Solver::fitness(Chromosome *genome, Task *task)
{
  double cost = 0;
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

    if (total_route_distance > task->max_route_duration )
    {
      cost += total_route_distance * 5;
    } else {
      cost += total_route_distance;
    }
    
  }

  genome->cost = cost;

  return 1000 / cost;
}

// mutace
BOOL Solver::mutator(Chromosome *genome, UINT _pmut, int mutagens)
{
  if (mutagens == -1)
  {
    mutagens = urandom(0, this->config->CONFIG_MUTAGENES);
  }
  
  if (urandom(0, this->config->unit) <= _pmut) // mutace s pravdepodobnosti _pmut
  {
    for (int i = 0; i < mutagens; i++)
    {
      switch (urandom(1, 3))
      {
      case 1:
        Solver::mutatorMoveBetweenVehicles(genome);
        break;
      case 2:
        Solver::mutatorChangeRouteSchedule(genome);
        break;
      case 3:
        Solver::mutatorGuidedChange(genome);
        break;
      // case 4:
      //   mutatorRandomRealocate(genome);
      //   break;
      default:
        break;
      }
    }
    return 1; // probehla-li mutace, vratim true...
  }
  return 0; // ...jinak vracim false
}

void Solver::mutatorMoveBetweenVehicles(Chromosome *genome)
{
  UINT pickup, delivery, index_2, vehicle_1, vehicle_2, v_2_size;
  UINT v_1_size = 0;

  while (v_1_size <= 2)
  {
    vehicle_1 = urandom(0, this->task->number_of_vehicles - 1);
    // vehicle_1 = selectRouteByWeight(genome);

    v_1_size = genome->routes[vehicle_1].route_length;
  }


  UINT mutagens_per_route = urandom(1, this->config->CONFIG_MUTAGENE_PER_ROUTE/2);
  for (UINT i = 0; i < mutagens_per_route; i++)
  {
    if (genome->routes[vehicle_1].route_length <= 2)
    {
      break;
    }

    // UINT index_1 = urandom(1, v_1_size - 2);
    // UINT index_1 = selectLocationByCost(&genome->routes[vehicle_1]);
    // UINT value = genome->routes[vehicle_1].locations[index_1];

    UINT value = selectCustomerByCentroid(&genome->routes[vehicle_1], this->task);
    UINT index_1 = genome->map_route_position[value];

    if (value % 2 == 0)
    {
      pickup = value - 1;
      delivery = value;
      index_2 = genome->map_route_position[pickup];
      deleteFromRoute(genome, vehicle_1, index_1);
      deleteFromRoute(genome, vehicle_1, index_2);
    }
    else
    {
      pickup = value;
      delivery = value + 1;
      index_2 = genome->map_route_position[delivery];
      deleteFromRoute(genome, vehicle_1, index_2);
      deleteFromRoute(genome, vehicle_1, index_1);
    }

    vehicle_2 = selectRouteByCentroid(genome, pickup, this->task);

    v_2_size = genome->routes[vehicle_2].route_length;

    inserCustomerToRoute(genome, vehicle_2, pickup, this->task);
    // UINT random_index = urandom(1, v_2_size - 1);
    // UINT new_index = insertToRoute(genome, vehicle_2, random_index, pickup, this->task->demands[pickup], this->task);
    // insertToRoute(genome, vehicle_2, new_index + 1, delivery, this->task->demands[delivery], this->task);
    // recalculateRoute(genome, vehicle_2, this->task);

    recalculateRoute(genome, vehicle_1, this->task);
  }


  if (urandom(0, 100) <= 50 && v_2_size > 4) // mutace s pravdepodobnosti _pmut
  {
    // swapLocations(genome, vehicle_2, this->task->capacity_of_vehicles, this->task->demands, this->task);
    int pickup = selectCustomerByCost(&genome->routes[vehicle_2], genome->map_route_position);
    // int pickup = selectCustomerByCentroid(&genome->routes[vehicle_2], this->task);
    realocateCustomerInRoute(genome, vehicle_2, pickup, this->task);
  }
}

void Solver::mutatorChangeRouteSchedule(Chromosome *genome)
{
  int route1 = selectRoute(genome, this->task->number_of_vehicles);
  // int route1 = selectRouteByWeight(genome);
  if (route1 == -1 || genome->routes[route1].route_length < 6)
    return; // finding of route was not successful

  UINT mutagens_per_route = urandom(1, this->config->CONFIG_MUTAGENE_PER_ROUTE);
  for (UINT i = 0; i < mutagens_per_route; i++)
  {
    // printRoute(genome->routes[route1], route1);
    // std::cout << "som tu 10" << std::endl;
    swapLocations(genome, route1, this->task->capacity_of_vehicles, this->task->demands, this->task);
    // printRoute(genome->routes[route1], route1);
  }
}

void Solver::mutatorGuidedChange(Chromosome *genome)
{
  int vehicle = selectRouteByWeight(genome);
  UINT mutagens_per_route = urandom(1, this->config->CONFIG_MUTAGENE_PER_ROUTE);
  for (UINT i = 0; i < mutagens_per_route; i++)
  {
    int pickup = selectCustomerByCost(&genome->routes[vehicle], genome->map_route_position);
    realocateCustomerInRoute(genome, vehicle, pickup, this->task);
  }
}

void Solver::mutatorRandomRealocate(Chromosome *genome)
{
  int vehicle = selectRoute(genome, this->task->number_of_vehicles);
  UINT mutagens_per_route = urandom(1, this->config->CONFIG_MUTAGENE_PER_ROUTE);
  for (UINT i = 0; i < mutagens_per_route; i++)
  {
    int pickup = selectRandomCustomer(&genome->routes[vehicle]);
    realocateCustomerInRoute(genome, vehicle, pickup, this->task);
  }
}

int Solver::selectIndividByWeight() {
  std::vector<double> finess;
  for (int i = 0; i < this->config->CONFIG_MI; i++)
  {
    finess.push_back(this->offs[i].fitness * 10);
  }
  std::mt19937 gen(rand());
  std::discrete_distribution<> dist(finess.begin(), finess.end());
  return dist(gen);
}