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

  Chromosome best; // best found solution

  Solver(Task* task, Config* config);
  ~Solver();

  bool Solve();

private:
  void initialize(Chromosome *genome);
  double fitness(Chromosome *genome, Task *task);
  BOOL stop();
  BOOL mutator(Chromosome *genome, UINT _pmut, int mutagens);
  void gprint(Chromosome *genome);
  void mutatorMoveBetweenVehicles(Chromosome *genome);
  void mutatorChangeRouteSchedule(Chromosome *genome);
  void mutatorGuidedChange(Chromosome *genome);
  void mutatorRandomRealocate(Chromosome *genome);
  int selectIndividByWeight();

  double best_ever;   // fitness of the best found individual

  int generation; // generation counter

  // GA -----------
  Chromosome *population;
  Chromosome *next_population;
  Chromosome *pool1;
  Chromosome *pool2;
  // --------------

  // ES -----------
  Chromosome *pop;
  Chromosome *next_pop;
  Chromosome *offs;
  // --------------
};

#endif