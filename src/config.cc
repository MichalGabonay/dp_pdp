#include "config.h"

Config::Config()
{
  CONFIG_DEBUG = false;
  CONFIG_PMUT = 60;
  CONFIG_MUTAGENES = 5;
  CONFIG_TOUR = 4;
  CONFIG_POPSIZE = 20;
  CONFIG_GENERATIONS = 100000;
  INPUT_FILE = "dataset/LC1_2_1.txt";
  unit = 100;
}