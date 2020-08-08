<!-- TODO: preložiť do AJ -->

---

**Problém smerovanie vozidiel s vyzdvihnutím a doručením (VRPPD)**, riešený pomocou evolučných algoritmov (genetický algoritmus a evolučné stratégie).

Program bol vytvorený v rámci mojej záverečnej/diplomovej práci na univerzite Vysoké učení technické v Brne, falkulty Informačných techník. Text práce na adrese https://www.vutbr.cz/www_base/zav_prace_soubor_verejne.php?file_id=216089. Výsledná známka za DP: B

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