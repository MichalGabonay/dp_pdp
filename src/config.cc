#include "config.h"

Config::Config()
{
  INPUT_FILE = "dataset/200/LR1_2_1.txt";
  MAX_ROUTE_DURATION = 300;

  CONFIG_DEBUG = true;
  CONFIG_GENERATIONS_PRINT = false;
  CONFIG_RESULT_SUMMARY = false;

  unit = 100;
  CONFIG_PMUT = 50;
  CONFIG_TOUR = 4;
  CONFIG_POPSIZE = 30;

  CONFIG_EVOLUTION_TYPE = "ES"; // ES or GA
  CONFIG_USE_GUIDED_MUTS = true;
  CONFIG_USE_CENTROIDS = true;

  CONFIG_ES_PLUS = true;

  CONFIG_GENERATIONS = 2000;
  CONFIG_LAMBDA = 30;

  CONFIG_ES_ELITISM = false;
  CONFIG_MI = 3;
  CONFIG_MUTAGENE_PER_ROUTE = 4;
  CONFIG_MUTAGENES = 5;
}