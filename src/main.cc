#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <chrono>
#include <unistd.h>
#include <inttypes.h>

#include "task.h"
#include "solver.h"
#include "config.h"

unsigned int seed;

unsigned int rand_init()
{
    unsigned int seed;
    FILE *fp = fopen("/dev/urandom", "rb");
    if (fp == NULL)
        fp = fopen("/dev/random", "rb");
    if (fp != NULL)
    {
        //printf("Init from /dev/{u}random\n");
        fread(&seed, sizeof(seed), 1, fp);
        fclose(fp);
    }
    else
    {
        seed = (unsigned int)((unsigned)time(NULL) + getpid());
    }
    srand(seed);
    return seed;
}

int main()
{
    seed = rand_init();
    // std::cout << seed << std::endl;
    // seed = 4169378759;
    // srand(seed);

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
        printf("%d;error;'Failed to solve the problem.'\n", getpid());
        printf("%d;config;%s;%d;%d;%d;%d;%d;%d;%d;%d;%s\n", getpid(), config.INPUT_FILE.c_str(), config.CONFIG_GENERATIONS, config.CONFIG_LAMBDA, config.CONFIG_MI, config.CONFIG_MUTAGENE_PER_ROUTE, config.CONFIG_MUTAGENES, config.CONFIG_USE_GUIDED_MUTS, config.CONFIG_USE_CENTROIDS, config.MAX_ROUTE_DURATION, config.CONFIG_EVOLUTION_TYPE.c_str());
        return 1;
    }

    time_end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(time_end - time_beggining).count();
    duration = duration / 1000000; // in seconds

    UINT used_vehicles = 0;
    UINT over_limit = 0;
    for (int i = 0; i < task.number_of_vehicles; i++)
    {
        Route route = solver.best.routes[i];
        if (route.route_length > 2)
        {
            std::ostringstream os;
            os << getpid() << ";vehicles;";
            // os << this->best_ever;
            for (size_t j = 0; j < route.route_length; j++)
            {
                if (j != route.route_length - 1)
                {
                    os << task.coords[route.locations[j]].first << "," << task.coords[route.locations[j]].second << ";";
                }
                else
                {
                    os << task.coords[route.locations[j]].first << "," << task.coords[route.locations[j]].second;
                }
            }

            std::string gen_summary = os.str();
            printf("%s\n", gen_summary.c_str());

            used_vehicles++;
            double distance = solver.best.routes[i].distance;
            if (distance > config.MAX_ROUTE_DURATION)
            {
                over_limit++;
            }
        }
    }

    if (config.CONFIG_RESULT_SUMMARY)
    {
        printf("%d;summary;%f;%f;%" PRId64 ";%u;%d;%d\n", getpid(), solver.best.fitness, solver.best.cost, duration, seed, used_vehicles, over_limit);
        printf("%d;config;%s;%d;%d;%d;%d;%d;%d;%d;%d;%s\n", getpid(), config.INPUT_FILE.c_str(), config.CONFIG_GENERATIONS, config.CONFIG_LAMBDA, config.CONFIG_MI, config.CONFIG_MUTAGENE_PER_ROUTE, config.CONFIG_MUTAGENES, config.CONFIG_USE_GUIDED_MUTS, config.CONFIG_USE_CENTROIDS, config.MAX_ROUTE_DURATION, config.CONFIG_EVOLUTION_TYPE.c_str());
    }

    return 0;
}