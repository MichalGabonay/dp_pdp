#include "config.h"

Config::Config()
{
  INPUT_FILE = "dataset/1000/LC1_10_1.txt";
  MAX_ROUTE_DURATION = 600;

  CONFIG_DEBUG = false;
  CONFIG_GENERATIONS_PRINT = true;
  CONFIG_RESULT_SUMMARY = true;

  unit = 100;
  CONFIG_PMUT = 80;
  CONFIG_TOUR = 4;
  CONFIG_POPSIZE = 30;

  CONFIG_USE_GUIDED_MUTS = true;
  CONFIG_USE_CENTROIDS = true;

  CONFIG_EVOLUTION_TYPE = "ES"; // ES or GA
  CONFIG_ES_PLUS = true;

  CONFIG_GENERATIONS = 200000;
  CONFIG_LAMBDA = 30;

  CONFIG_ES_ELITISM = false;
  CONFIG_MI = 2;
  CONFIG_MUTAGENE_PER_ROUTE = 3;
  CONFIG_MUTAGENES = 3;
}