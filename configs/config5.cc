#include "config.h"

Config::Config()
{
  INPUT_FILE = "dataset/LC1_2_1.txt";

  CONFIG_DEBUG = false;
  CONFIG_GENERATIONS_PRINT = true;
  CONFIG_RESULT_SUMMARY = false;

  unit = 100;
  CONFIG_PMUT = 80;
  CONFIG_TOUR = 3;
  CONFIG_POPSIZE = 20;

  CONFIG_EVOLUTION_TYPE = "ES"; // ES or GA
  CONFIG_ES_PLUS = false;

  CONFIG_GENERATIONS = 80000;
  CONFIG_LAMBDA = 40;

  CONFIG_ES_ELITISM = true;
  CONFIG_MUTAGENE_PER_ROUTE = 2;
  CONFIG_MUTAGENES = 3;
  CONFIG_MI = 4;
}