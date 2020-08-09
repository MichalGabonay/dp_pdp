**Problém smerovanie vozidiel s vyzdvihnutím a doručením (VRPPD)**, riešený pomocou evolučných algoritmov (genetický algoritmus a evolučné stratégie).

Program bol vytvorený v rámci mojej záverečnej/diplomovej práci na VUT v Brne, falkulty Informačných technológií. Text práce je na adrese https://www.vutbr.cz/www_base/zav_prace_soubor_verejne.php?file_id=216089. Výsledná známka za DP je B.

Program sa preloží jednoduchým príkazom `make`. Program využíva len štandardné knihovny C++.

Po preložení sa program spúšťa príkazom `make run` alebo `./pdp_xgabon00`.

Konfigurácia spustenia programu je v súbore `src/config.cc`. Pre ukážkové spustenie nie je potreba meniť. V prípade potreby zmeniť niečo, je tu priložený stručný popis parametrov:

* `INPUT_FILE` - vstupný súbor
* `MAX_ROUTE_DURATION` - maximálna dĺžka trasy pre jedno vozidlo
* `CONFIG_DEBUG` - na štandartný výstup je zaznamenané generácia vždy keď nastane zlepšenie, po skončení sa vypíše najlepšie nájdené riešenie vo forme: 
vehicle (počet miest): ID_miesta[naloženie][prejdená vzdialenosť], ... 
distance: celková vzdialenosť trasy
* `CONFIG_GENERATIONS_PRINT` - každých 100 generácií vypíše fitness hodnotu najlepšieho jedinca
* `CONFIG_RESULT_SUMMARY` - výstup programu, oddelený bodkočiarkami
* `CONFIG_EVOLUTION_TYPE` - hodnota "ES" pre použitie evolučnej stratégie alebo hodnota "GA" pre použitie genetického algoritmu
* `CONFIG_USE_GUIDED_MUTS` - pri mutáciách sa pracuje s cielenými úpravami trias
* `CONFIG_USE_CENTROIDS` - pri mutáciách sa pracuje s ťažiskom trás
* `CONFIG_GENERATIONS` - počet generácií
* `CONFIG_TOUR` - počet vstupujúcich jedincov do turnaja (pri použití GA)
* `CONFIG_POPSIZE` - veľkosť populácie (pri použití GA)
* `CONFIG_PMUT` - pravdepodobnosť mutovania génu
* `CONFIG_LAMBDA` - hodnota λ (pri použití ES)
* `CONFIG_MI` - hodnota μ (pri použití ES)
* `CONFIG_ES_PLUS` - ak je nastavený na `true`, je použitý typ evolučnej stratégie (μ + λ), v opačnom prípade je použitý typ (μ, λ) 

---

**Vehicle Routing Problem with Pickups and Deliveries (VRPPD)**, solved with evolutionary algorithms (Genetic Algorithm and Evolution Strategies).

The program was created as a part of my master's thesis at the Brno University of Technology, the Faculty of Information Technology.

Compile the program with the command `make`. The program uses only standard C++ libraries.

After compilation, run the program with the command `make run` or `./pdp_xgabon00`.

Configurations of the program are in the file `src/config.cc`. For the exemplary running, there is no need to change anything there. List of config parameters:

* `INPUT_FILE` - path to input file
* `MAX_ROUTE_DURATION` - maximal distance (duration) for 1 vehicle
* `CONFIG_DEBUG` - on `stdout` is printed generation in which there is improvement in the solution, at the end the best solution is outputed in the form: 
vehicle (number of stops): ID_stop[load][distance], ... 
distance: total route distance
* `CONFIG_GENERATIONS_PRINT` - every 100 generations is outputed the fitness value of the best member (founded solution)
* `CONFIG_RESULT_SUMMARY` - result of the program (separated with semicoloms)
* `CONFIG_EVOLUTION_TYPE` - value "ES" for using evolution strategy or value "GA" for using genetic algorithm
* `CONFIG_USE_GUIDED_MUTS` - mutations uses targeted operations for manipulationg with routes
* `CONFIG_USE_CENTROIDS` - mutations use also centroids of routes for founding better solutions
* `CONFIG_GENERATIONS` - number of generations
* `CONFIG_TOUR` - number of individuals entering the tournament (used only in GA)
* `CONFIG_POPSIZE` - population size (used only in GA)
* `CONFIG_PMUT` - probability of mutations
* `CONFIG_LAMBDA` - value λ (used only in ES)
* `CONFIG_MI` - value μ (used only in ES)
* `CONFIG_ES_PLUS` - if set to `true`, (μ + λ) is used in evolution strategy, if `false` ES uses (μ, λ) method
