# ## numpy is used for creating fake data
# import numpy as np 
# # import matplotlib as mpl 

# ## agg backend is used to create plot as a .png file
# # mpl.use('agg')

# import matplotlib.pyplot as plt 

# ## Create data
# np.random.seed(10)
# collectn_1 = np.random.normal(100, 10, 200)
# collectn_2 = np.random.normal(80, 30, 200)
# collectn_3 = np.random.normal(90, 20, 200)
# collectn_4 = np.random.normal(70, 25, 200)
# collectn_5 = np.random.normal(1, 1, 1)

# ## combine these different collections into a list    
# data_to_plot = [collectn_1, collectn_2, collectn_3, collectn_4, collectn_5]

# # Create a figure instance
# fig = plt.figure(1, figsize=(9, 6))

# # Create an axes instance
# ax = fig.add_subplot(111)

# # Create the boxplot
# bp = ax.boxplot(data_to_plot)
# plt.savefig('fig2.png')
# plt.show()

# # Save the figure
# # fig.savefig('fig1.png', bbox_inches='tight')

import matplotlib.pyplot as plt
import numpy as np
import csv

def Average(lst):
    return sum(lst) / len(lst)


results = {}
with open('../data.csv') as csv_file:
    csv_reader = csv.reader(csv_file, delimiter=';')
    line_count = 0
    for row in csv_reader:
        if results.get(row[0]) == None:
            results.update(
                {row[0]: {"bests": [], "averages": [], "generations": []}})
        population = []
        for i in range(1, len(row)):
            population.append(float(row[i]))
        results[row[0]]["generations"].append(population)

        line_count += 1
        if line_count % 100000 == 0:
            print("read ", line_count, " lines")

bests = []
boxplot_x = []
boxplot_labels = []

gen_index = 0
for key in results:
    generation = 1000
    axis_x = []
    ind_index = 0
    for gen in results[key]["generations"]:
        results[key]["bests"].append(max(gen))
        results[key]["averages"].append(Average(gen))
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
        if generation == 1000000:
            break
    gen_index += 1

    fig = plt.figure(1, figsize=(16, 12))
    # ax = fig.add_subplot(111)
    # ax.boxplot(boxplot_x, boxplot_labels)
    plt.plot(axis_x, results[key]["bests"])
    plt.ylabel('fitness')
    plt.xlabel('generations')
    plt.savefig("test/" + key + '.png')
    # plt.show()
    plt.cla()

boxplot_x = []
boxplot_labels = []
best_of_bests = []
axis_x = []
generation = 1000

for gen_of_bests in bests:
    best_of_bests.append(max(gen_of_bests))
    axis_x.append(generation)
    if generation%1000000 == 0:
            boxplot_x.append(gen_of_bests)
            boxplot_labels.append(generation)
    generation += 1000
fig = plt.figure(1, figsize=(16, 12))
plt.plot(axis_x, best_of_bests)
plt.ylabel('fitness')
plt.xlabel('generations')
plt.savefig('test/bests.png')
# plt.show()

plt.cla()

# fig = plt.figure(1, figsize=(16, 12))
# ax = fig.add_subplot(111)
# ax.boxplot(boxplot_x, boxplot_labels)
# plt.savefig('bests-boxplot.png')
# plt.show()

# -------------------------------2------------------------------------------
plt.cla()

# gen_index = 0
# for key in results:
#     generation = 1000
#     axis_x = []
#     ind_index = 0
#     for gen in results[key]["generations"]:
#         axis_x.append(generation)
#         generation += 1000
#         ind_index += 1
#     gen_index += 1

#     plt.plot(axis_x, results[key]["bests"])

# best_of_bests = []
# axis_x = []
# generation = 1000
# for gen_of_bests in bests:
#     best_of_bests.append(max(gen_of_bests))
#     axis_x.append(generation)
#     generation += 1000
# fig = plt.figure(1, figsize=(16, 12))
# plt.plot(axis_x, best_of_bests)
# plt.ylabel('fitness')
# plt.xlabel('generations')
# # plt.savefig('all.png')
# plt.show()
