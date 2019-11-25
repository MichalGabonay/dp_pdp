#include "utilities.h"

#ifndef PARAMS_H
#define PARAMS_H

// pro vypis prubehu evoluce na stdout
#define DEBUG

//----------------------- parametry genetickeho algoritmu ----------------------
// pravdepodobnost mutace
#define PMUT 70
// pocet mutovanych genu v chromozomu
#define MUTAGENES 5
// pravdepodobnost krizeni
// #define PCROSS 70
// pocet jedincu v turnajove selekce
#define TOUR 4
// velikost populace
#define POPSIZE 15
// maximalni pocet generaci
#define GENERATIONS 2000000
// number of mutation operators
#define NUMBER_OF_MUTATORS 2
//------------------------------------------------------------------------------

typedef unsigned int UINT;
typedef int BOOL;

typedef struct {
    std::vector<UINT> locations;
    std::vector<int> utilization;
    UINT route_length;
    UINT cost;
    UINT duration;
    UINT distance;
} Route;

// definice typu chromozomu GA
typedef struct {
    double fitness;   // fitness daneho jedince
    BOOL evaluate;  // ci je treba znovu vyhodnotit fitness
    std::vector<Route> routes;
    std::vector<UINT> map_route_position;
} GA_chromosome;

// prototypy funkci pro GA
UINT urandom(UINT low, UINT high);

void initialize(GA_chromosome *g);
// void crossover(GA_chromosome *parent1, GA_chromosome *parent2, 
//                 GA_chromosome *child1, GA_chromosome *child2);
BOOL mutator(GA_chromosome *genome, UINT _pmut);
double fitness(GA_chromosome *genome);
void gprint(GA_chromosome *genome);
void evolve();
BOOL stop();
void mutatorMoveBetweenVehicles(GA_chromosome *genome);
void mutatorChangeRouteSchedule(GA_chromosome *genome);

void insertToRoute (GA_chromosome *g, UINT vehicle, UINT index, UINT value, std::vector<int> location);
void deleteFromRoute (GA_chromosome *g, UINT vehicle, UINT index, std::vector<int> location);

#endif
