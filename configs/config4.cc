#include "config.h"

Config::Config()
{
  INPUT_FILE = "dataset/LR1_2_1.txt";
  MAX_ROUTE_DURATION = 250;

  CONFIG_DEBUG = false;
  CONFIG_GENERATIONS_PRINT = true;
  CONFIG_RESULT_SUMMARY = true;

  unit = 100;
  CONFIG_PMUT = 80;
  CONFIG_TOUR = 4;
  CONFIG_POPSIZE = 30;

  CONFIG_USE_GUIDED_MUTS = false;
  CONFIG_USE_CENTROIDS = false;

  CONFIG_EVOLUTION_TYPE = "ES"; // ES or GA
  CONFIG_ES_PLUS = true;

  CONFIG_GENERATIONS = 100000;
  CONFIG_LAMBDA = 30;

  CONFIG_ES_ELITISM = false;
  CONFIG_MI = 3;
  CONFIG_MUTAGENE_PER_ROUTE = 4;
  CONFIG_MUTAGENES = 5;
}