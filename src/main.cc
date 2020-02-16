#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <chrono>
#include <unistd.h>

#include "task.h"
#include "solver.h"
#include "config.h"

unsigned int seed;

unsigned int rand_init()
{
    unsigned int seed;
    FILE* fp = fopen("/dev/urandom","rb");
    if (fp == NULL) fp = fopen("/dev/random","rb");
    if (fp != NULL) {
       //printf("Init from /dev/{u}random\n");
       fread(&seed, sizeof(seed), 1, fp);
       fclose(fp);
    } else {
       seed = (unsigned int)((unsigned) time(NULL) + getpid());
    }
    srand(seed);
    return seed;
}

int main()
{
    seed = rand_init();
    std::cout << seed << std::endl;

    std::chrono::_V2::system_clock::time_point time_beggining;
    std::chrono::_V2::system_clock::time_point time_end;
    time_beggining = std::chrono::high_resolution_clock::now();
    Task task = Task();
    Config config = Config();

    if (!task.FetchTask(config.INPUT_FILE))
    {
        std::cout << "Failed to proccess input." << std::endl;
        return 1;
    }
    
    Solver solver = Solver(&task, &config);

    if (!solver.Solve())
    {
        std::cout << "Failed to solve the problem." << std::endl;
        return 1;
    }

    time_end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(time_end - time_beggining).count();
    duration = duration / 1000; // in miliseconds
    std::cout << "DURATION:" << duration << "ms" << std::endl;
    
    return 0;
}