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
  int CONFIG_PMUT; 
  int CONFIG_MUTAGENES;
  int CONFIG_TOUR;
  int CONFIG_POPSIZE;
  int CONFIG_GENERATIONS;
  std::string INPUT_FILE;
  unsigned int unit;

  Config();
};

#endif