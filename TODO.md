# TODO:

## Phase 1 (mvp - basic pickup and delivery VRP) - 6-7.11.
  * ~~parse input dataset~~
  * ~~helpers~~
    * ~~print generation~~
    * ~~calculate distance (euclid)~~
    * ~~print array~~
    * ~~array operations~~
  * ~~random initialization~~
  * ~~fitness calculation~~
  * ~~MUTATORS~~
    * ~~move between vehicles~~
    * ~~switch route schedule order~~
  * ~~add to git repository~~
  * ~~put it all togather into 1. functional version~~

---

## Phase 2 (improving) - 25.11.-2.12.
  * implement capacity restriction
  * improve initialization
  * make GA params as program input args + script for auto run with multiple configurations (100x per config) + summarize output to CSV
  * improve and add new mutators (mutation operator)
  * improve fitness evaluation
  * look at clustering of locations in dataset (think about using it somehow)

---

## Phase 3 (optimalization and flexin) - 16.12.-22.12.
  * add diversity checker (saw-tooth)
    * implement more sofisticated reproduction process
  * optimalization criterion by cost (weight/distance)
    * multicriterion ?
  * optimize
  * clean code
  * corssover ?
  * TW ?

---

## Phase 4 (finishing) - end of Jan - middle of Feb
  * generate program oputput report
  * implement experiments with different GA settings
  * finish - usable SW for master thesis use (all other improve is just bonus)
  * TW ?
  * add another metaheuristic for improving algorithm ?
