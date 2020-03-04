#ifndef CONFIG_H
#define CONFIG_H 1

#include <stdio.h>
#include <stdlib.h>
#include <string>

class Config
{
public:
  // configuration data
  bool CONFIG_DEBUG;
  bool CONFIG_GENERATIONS_PRINT;
  bool CONFIG_RESULT_SUMMARY;
  bool CONFIG_ES_PLUS;
  bool CONFIG_ES_ELITISM;
  int CONFIG_PMUT; 
  int CONFIG_MUTAGENES;
  int CONFIG_MUTAGENE_PER_ROUTE;
  int CONFIG_TOUR;
  int CONFIG_POPSIZE;
  int CONFIG_MI;
  int CONFIG_LAMBDA;
  int CONFIG_GENERATIONS;
  std::string CONFIG_EVOLUTION_TYPE;
  std::string INPUT_FILE;
  unsigned int unit;
  double CONFIG_MAX_ROUTE_DURATION;

  Config();
};

#endif