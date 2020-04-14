#ifndef SOLVER_H
#define SOLVER_H 1

#include "task.h"
#include "config.h"
#include "helpers.h"
#include "individual.h"
#include "route.h"

#include <iostream>
#include <bits/stdc++.h>
#include <unistd.h>
#include <algorithm>


class Solver
{
public:
  Task *task;
  Config *config;

  Individual best; // best found solution

  Solver(Task* task, Config* config);
  ~Solver();

  bool Solve();

private:
  void initialize(Individual *genome);
  double fitness(Individual *genome, Task *task);
  BOOL stop();
  BOOL mutator(Individual *genome, UINT _pmut, int mutagens);
  void gprint(Individual *genome);
  void mutatorMoveBetweenVehicles(Individual *genome);
  void mutatorChangeRouteSchedule(Individual *genome);
  void mutatorGuidedChange(Individual *genome);
  void mutatorGuidedChange2(Individual *genome);
  void mutatorRandomRealocate(Individual *genome);
  int selectIndividByWeight();

  double best_ever;   // fitness of the best found individual

  int generation; // generation counter

  // GA -----------
  Individual *population;
  Individual *next_population;
  Individual *pool1;
  Individual *pool2;
  // --------------

  // ES -----------
  Individual *pop;
  Individual *next_pop;
  Individual *offs;
  // --------------
};

#endif