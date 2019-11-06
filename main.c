//////////////////////////////////////////////////////////////
// VZOROVA IMPLEMENTACE JEDNODUCHEHO GENETICKEHO ALGORITMU  //
// JAZYK C, PREKLAD S OHLEDEM NA NORMU C99                  //
//                                                          //
// (c) MICHAL BIDLO, 2011                                   //
//                                                          //
// POKUD NENI UVEDENO JINAK, TENTO KOD SMI BYT POUZIT       //
// VYHRADNE PRO POTREBY RESENI PROJEKTU V PREDMETECH        //
// BIOLOGII INSPIROVANE POCITACE, PRIPADNE                  //
// APLIKOVANE EVOLUCNI ALGORITMY NA FIT VUT V BRNE.         //
//////////////////////////////////////////////////////////////

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <time.h>

#include "params.h"
// #include "utilities.h"

// jednotky pro specifikaci pravdepodobnosti genetickych operatoru:
// 100 - procenta, 1000 - promile. NENI TREBA MENIT
const UINT unit = 100;
// delka chromozomu - s touto promennou pracuje GA
const UINT glength = CHLEN;
// maximalni fitness - zde odpovida delce jedince, protoze se pouze snazime
// nalezt retezec se samymi 1 geny
const double max_fit = CHLEN;

#ifdef DEBUG
const UINT generations = 5000000; // 0 - pocet generaci neni pri ladeni omezen
#else
const UINT generations = GENERATIONS; // po tomto poctu je GA zastaven
#endif

UINT locations[RSIZ][9];
Vector map_locations;
Vector vehicles;

// !!!! nas GA pracuje pouze se sudymi pocty jedincu v populaci !!!!
UINT _popsize = (POPSIZE & 1) ? POPSIZE + 1 : POPSIZE;

// ------------------- implementace genetickeho algoritmu ----------------------
// *****************************************************************************

GA_chromosome best; // njelepsi dosud nalezene reseni
double best_ever;   // fitness dosud nejlepsiho jedince

UINT generation; // pocitadlo generaci
GA_chromosome *population;
GA_chromosome *next_population;
// pracovni populace - pouze sudy pocet jedincu !!!!
GA_chromosome pool1[(POPSIZE & 1) ? POPSIZE + 1 : POPSIZE];
GA_chromosome pool2[(POPSIZE & 1) ? POPSIZE + 1 : POPSIZE];

// evolucni cyklus SGA: nova populace o _popsize jedincich je tvorena krizenimi
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
  //------------------------------------------------------------------------------------------------------------------------
  // print_array(pool1[1].routes[1].locations, pool1[1].routes[1].route_length);
  // swap_array_values(pool1[1].routes[1].locations, 2, 3);
  // print_array(pool1[1].routes[1].locations, pool1[1].routes[1].route_length);
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

// vypis chromozomu - ZMENTE SI DLE POTREBY
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
          printf("%d, ", genome->routes[i].locations[j]);
        }
        else
        {
          printf("%d", genome->routes[i].locations[j]);
        }
      }
      printf("\n");
      printf("cost: %d  --  duration: %d  --  distance: %d \n\n", genome->routes[i].cost, genome->routes[i].duration, genome->routes[i].distance);
      used_vehicles++;
    }
  }
  printf("TOTAL DISTANCE: %f, USED VEHICLES: %d\n", 1 / genome->fitness, used_vehicles);
}

// inicializace populace nahodne - ZMENTE SI DLE POTREBY
void initialize(GA_chromosome *genome)
{
  for (size_t i = 0; i < NUMBER_OF_VEHICLES; i++)
  {
    genome->routes[i].locations[0] = 0;
    genome->routes[i].route_length = 1;
  }
  for (size_t i = 1; i < size(&map_locations); i += 2)
  {
    UINT vehicle = urandom(0, NUMBER_OF_VEHICLES - 1);
    UINT next = genome->routes[vehicle].route_length;
    genome->routes[vehicle].locations[next] = i;
    genome->routes[vehicle].locations[next + 1] = i + 1;
    genome->routes[vehicle].route_length += 2;
  }
  for (size_t i = 0; i < NUMBER_OF_VEHICLES; i++)
  {
    genome->routes[i].locations[genome->routes[i].route_length] = 0;
    genome->routes[i].route_length++;
  }
}

// mutace - ZMENTE SI DLE POTREBY. je vsak treba zachovat navratovou hodnotu!
BOOL mutator(GA_chromosome *genome, UINT _pmut)
{
  if (urandom(0, unit) <= _pmut) // mutace s pravdepodobnosti _pmut
  {
    for (UINT i = 0; i < MUTAGENES; i++)
    {
      switch (urandom(1, NUMBER_OF_MUTATORS))
      {
      case 1:
        mutator_move_between_vehicles(genome);
        break;
      case 2:
        mutator_change_route_schedule(genome);
        break;
      default:
        break;
      }
    }

    return 1; // probehla-li mutace, vratim true...
  }

  return 0; // ...jinak vracim false
}

// test na zastaveni evoluce - V PRIPADE POTREBY ZMENTE
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

// evaluace fitness pro zadaneho jedince - ZMENTE PRO RESENI SVEHO PROBLEMU
double fitness(GA_chromosome *genome)
{
  // gprint(genome);
  double total_distance = 0;
  double total_route_distance;
  UINT fit = 0.0;

  for (UINT i = 0; i < NUMBER_OF_VEHICLES; i++)
  {
    total_route_distance = 0;
    for (size_t j = 0; j < genome->routes[i].route_length - 1; j++)
    {
      UINT loc_o = genome->routes[i].locations[j];
      UINT loc_d = genome->routes[i].locations[j + 1];
      // printf("j=%d(%d): loc_o: %d, loc_d: %d, l: %d\n", (int)j, genome->routes[i].route_length, loc_o, loc_d, get(&map_locations, loc_o));

      UINT origin_x = locations[get(&map_locations, loc_o)][1];
      UINT origin_y = locations[get(&map_locations, loc_o)][2];
      UINT destination_x = locations[get(&map_locations, loc_d)][1];
      UINT destination_y = locations[get(&map_locations, loc_d)][2];
      // printf("origin_x:%d, origin_y:%d, destination_x:%d, destination_y:%d\n", origin_x, origin_y, destination_x, destination_y);
      total_route_distance += calculate_distance_between_points(
          origin_x, origin_y, destination_x, destination_y);
    }

    genome->routes[i].distance = total_route_distance;
    total_distance += total_route_distance;
  }

  return 1 / total_distance;
}

void mutator_move_between_vehicles(GA_chromosome *genome)
{
  UINT pickup, delivery, index_2, index_new, vehicle_1;
  UINT v_1_size = 0;

  while (v_1_size == 0)
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

  if (v_1_size == 2)
  {
    // printf("Empty vehicle 1\n");
    return;
  }

  //------------------------------------------------------------------------------------------------------
  // printf("OLD:\n");
  // print_array(genome->routes[vehicle_1].locations, genome->routes[vehicle_1].route_length);
  // print_array(genome->routes[vehicle_2].locations, genome->routes[vehicle_2].route_length);
  //------------------------------------------------------------------------------------------------------

  UINT index_1 = urandom(1, v_1_size - 2);
  UINT value = genome->routes[vehicle_1].locations[index_1];
  if (value == 0)
  {
    return;
  }

  // printf("1: vehicle_1: %d  --  vehicle_2: %d  --  index_1: %d  --  value: %d\n", vehicle_1, vehicle_2, index_1, value);
  if (value % 2 == 0)
  {
    pickup = value - 1;
    delivery = value;
    index_2 = find_value_in_array(genome->routes[vehicle_1].locations, genome->routes[vehicle_1].route_length, value - 1);
    delete_from_array(genome->routes[vehicle_1].locations, genome->routes[vehicle_1].route_length, index_1);
    genome->routes[vehicle_1].route_length--;
    delete_from_array(genome->routes[vehicle_1].locations, genome->routes[vehicle_1].route_length, index_2);
    genome->routes[vehicle_1].route_length--;
  }
  else
  {
    pickup = value;
    delivery = value + 1;
    index_2 = find_value_in_array(genome->routes[vehicle_1].locations, genome->routes[vehicle_1].route_length, value + 1);
    delete_from_array(genome->routes[vehicle_1].locations, genome->routes[vehicle_1].route_length, index_2);
    genome->routes[vehicle_1].route_length--;
    delete_from_array(genome->routes[vehicle_1].locations, genome->routes[vehicle_1].route_length, index_1);
    genome->routes[vehicle_1].route_length--;
  }

  genome->routes[vehicle_2].locations[v_2_size - 1] = pickup;
  genome->routes[vehicle_2].locations[v_2_size] = delivery;
  genome->routes[vehicle_2].locations[v_2_size + 1] = 0;
  genome->routes[vehicle_2].route_length += 2;

  //------------------------------------------------------------------------------------------------------
  // printf("NEW:\n");
  // print_array(genome->routes[vehicle_1].locations, genome->routes[vehicle_1].route_length);
  // print_array(genome->routes[vehicle_2].locations, genome->routes[vehicle_2].route_length);
  //------------------------------------------------------------------------------------------------------
}

void mutator_change_route_schedule(GA_chromosome *genome)
{
  UINT vehicle;
  UINT v_size = 0;
  while (v_size <= 4)
  {
    vehicle = urandom(0, NUMBER_OF_VEHICLES - 1);
    v_size = genome->routes[vehicle].route_length;
  }

  UINT index = urandom(1, v_size - 3);
  UINT value = genome->routes[vehicle].locations[index];

  if (value % 2 == 0)
  {
    swap_array_values(genome->routes[vehicle].locations, index, index + 1);
  }
  else
  {
    swap_array_values(genome->routes[vehicle].locations, index, index - 1);
  }
}

// *****************************************************************************
// ------------------------------ hlavni program -------------------------------

int main(int argc, char *argv[])
{
  init(&map_locations);
  init(&vehicles);
  read_dataset(&map_locations, &vehicles, locations);

  srand(time(0)); // random seed - NUTNE
  evolve();

  return 0;
}
