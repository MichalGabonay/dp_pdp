#include "config.h"

Config::Config() {
  CONFIG_DEBUG = true;
  CONFIG_PMUT = 60;
  CONFIG_MUTAGENES = 5;
  CONFIG_TOUR = 4;
  CONFIG_POPSIZE = 20;
  CONFIG_GENERATIONS = 1000000;
  INPUT_FILE = "dataset/lc101.txt";
  unit = 100;
}