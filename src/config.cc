#include "config.h"

Config::Config()
{
  CONFIG_DEBUG = false;
  CONFIG_GENERATIONS_PRINT = true;
  CONFIG_RESULT_SUMMARY = true;
  CONFIG_PMUT = 50;
  CONFIG_MUTAGENES = 5;
  CONFIG_TOUR = 4;
  CONFIG_POPSIZE = 20;
  CONFIG_GENERATIONS = 10000;
  INPUT_FILE = "dataset/LC1_2_1.txt";
  unit = 100;
}