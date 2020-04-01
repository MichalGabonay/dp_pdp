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

  Solver(Task* task, Config* config);

  bool Solve();

private:
  void initialize(GA_chromosome *genome);
  double fitness(GA_chromosome *genome, Task *task);
  BOOL stop();
  BOOL mutator(GA_chromosome *genome, UINT _pmut, int mutagens);
  void gprint(GA_chromosome *genome);
  void mutatorMoveBetweenVehicles(GA_chromosome *genome);
  void mutatorChangeRouteSchedule(GA_chromosome *genome);
  void mutatorGuidedChange(GA_chromosome *genome);
  void mutatorRandomRealocate(GA_chromosome *genome);
  int selectIndividByWeight();

  double best_ever;   // fitness dosud nejlepsiho jedince

  int generation; // pocitadlo generaci
  GA_chromosome *population;
  GA_chromosome *next_population;
  // pracovni populace - parny pocet jedincov
  GA_chromosome *pool1;
  GA_chromosome *pool2;

  // ES -----------
  GA_chromosome *pop;
  GA_chromosome *next_pop;
  GA_chromosome *offs;
  // --------------
};

#endif