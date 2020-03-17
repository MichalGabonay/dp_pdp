# import matplotlib.pyplot as plt
import numpy as np
import csv
import glob, os

SUFFCIENT_FITNESS = 0.426 # cca 5% worse than Or-tools LC1_2_1

def Average(lst):
    return sum(lst) / len(lst)

results = {}
for file in glob.glob('./*.[0-9]'):
    # print(file)
    with open(file) as csv_file:
        csv_reader = csv.reader(csv_file, delimiter=';')
        line_count = 0
        for row in csv_reader:
            if results.get(row[0]) == None:
                results.update(
                    {row[0]: {"bests": [], "averages": [], "generations": [], "best": 0}})
            population = []
            for i in range(1, len(row)):
                population.append(float(row[i]))
            results[row[0]]["generations"].append(population)
            line_count += 1

# print(results)

bests = []
boxplot_x = []
boxplot_labels = []

suffcient_solutions = 0

gen_index = 0
for key in results:
    generation = 1000
    axis_x = []
    ind_index = 0
    for gen in results[key]["generations"]:
        results[key]["bests"].append(max(gen))
        results[key]["averages"].append(Average(gen))
        if max(gen) > results[key]["best"]:
            results[key]["best"] = max(gen)
        axis_x.append(generation)
        generation += 1000
        if gen_index == 0:
            bests.append([max(gen)])
        else:
            bests[ind_index].append(max(gen))
        ind_index += 1
        # if generation%1000000 == 0:
        #     boxplot_x.append(gen)
        #     boxplot_labels.append(generation)
    if results[key]["best"] >= SUFFCIENT_FITNESS:
        suffcient_solutions += 1
    gen_index += 1

    # fig = plt.figure(1, figsize=(16, 12))
    # ax = fig.add_subplot(111)
    # ax.boxplot(boxplot_x, boxplot_labels)
    # plt.plot(axis_x, results[key]["bests"])
    # plt.ylabel('fitness')
    # plt.xlabel('generations')
    # plt.savefig("gens/" + key + '.png')
    # plt.show()
    # plt.cla()

boxplot_x = []
boxplot_labels = []
best_of_bests = []
worst_of_bests = []
avrgs = []
axis_x = []
generation = 1000

for gen_of_bests in bests:
    best_of_bests.append(max(gen_of_bests))
    avrgs.append(Average(gen_of_bests))
    worst_of_bests.append(min(gen_of_bests))
    axis_x.append(generation)
    if generation%1000000 == 0:
            boxplot_x.append(gen_of_bests)
            boxplot_labels.append(generation)
    generation += 1000
    
# fig = plt.figure(1, figsize=(16, 12))
# plt.plot(axis_x, best_of_bests)
# plt.plot(axis_x, avrgs)
# plt.plot(axis_x, worst_of_bests)
# plt.ylabel('fitness')
# plt.xlabel('generations')
# plt.savefig('bests.png')

# plt.cla()

print("BEST: ", max(best_of_bests), ", AVRG: ", max(avrgs), ", WORST: ", max(worst_of_bests), ", SUFFCIENT: ", (suffcient_solutions/len(results))*100, "%")