#ifndef SOLVER_H
#define SOLVER_H 1

#include "task.h"
#include "config.h"
#include "vrp_helpers.h"


class Solver
{
public:
  Task *task;
  Config *config;

  GA_chromosome best; // najlepsi dosud nalezene reseni
  double best_ever;   // fitness dosud nejlepsiho jedince

  int generation; // pocitadlo generaci
  GA_chromosome *population;
  GA_chromosome *next_population;
  // pracovni populace - parny pocet jedincov
  GA_chromosome *pool1;
  GA_chromosome *pool2;

  // ES -----------
  GA_chromosome *pop;
  GA_chromosome *offs;
  // --------------

  Solver(Task* task, Config* config);
  bool Solve();
};

void initialize(GA_chromosome *genome, Solver *solver);
double fitness(GA_chromosome *genome, Task *task);
BOOL stop(Config *config, Solver *solver);
BOOL mutator(GA_chromosome *genome, UINT _pmut, Solver *solver);
void gprint(GA_chromosome *genome, Solver *solver);
void mutatorMoveBetweenVehicles(GA_chromosome *genome, Solver *solver);
void mutatorChangeRouteSchedule(GA_chromosome *genome, Solver *solver);

void test(GA_chromosome *genome, Solver *solver);
#endif