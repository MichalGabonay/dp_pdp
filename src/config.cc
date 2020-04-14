#include "config.h"

Config::Config()
{
  INPUT_FILE = "dataset/LRC2_2_1.txt";
  MAX_ROUTE_DURATION = 420;

  CONFIG_DEBUG = false;
  CONFIG_GENERATIONS_PRINT = true;
  CONFIG_RESULT_SUMMARY = true;

  unit = 100;
  CONFIG_PMUT = 80;
  CONFIG_TOUR = 3;
  CONFIG_POPSIZE = 20;

  CONFIG_EVOLUTION_TYPE = "ES"; // ES or GA
  CONFIG_ES_PLUS = false;

  CONFIG_GENERATIONS = 1000;
  CONFIG_LAMBDA = 20;

  CONFIG_ES_ELITISM = false;
  CONFIG_MI = 3;
  CONFIG_MUTAGENE_PER_ROUTE = 4;
  CONFIG_MUTAGENES = 4;
}