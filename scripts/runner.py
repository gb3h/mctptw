# Script to run algorithm on a variety of parameters over different test sets and collate data

import os
import re
import subprocess


solomon_parameters = {
    "S_L1,0.00": {"lambda": 1, "alpha1": 0},
    "S_L1,0.25": {"lambda": 1, "alpha1": 0.25},
    "S_L1,0.5": {"lambda": 1, "alpha1": 0.5},
    "S_L2,0.00": {"lambda": 2, "alpha1": 0},
    "S_L2,0.25": {"lambda": 2, "alpha1": 0.25},
    "S_L2,0.5": {"lambda": 2, "alpha1": 0.5},

}

overlap_parameters = {
    "O_,0.00": {"lambda": 1, "alpha1": 0},
    "O_,0.25": {"lambda": 1, "alpha1": 0.25},
    "O_,0.5": {"lambda": 1, "alpha1": 0.5},
}


counter = {}
params_str = []
for name in solomon_parameters:
    params_str.append(name)
    counter[name] = 0

for name in overlap_parameters:
    params_str.append(name)
    counter[name] = 0

row_format = "{:<26}" + "{:<16}" + "{:>12}" * (len(params_str))
print(row_format.format("", "Best", *params_str))

# regex = re.compile('(0025_0075_R)+')
commands = [[f'../mctptw.out'], [f'../mctptw_overlap.out']]
files = []
for filename in os.listdir('../covering_problems'):
    filepath = os.path.join('../covering_problems', filename)
    if os.path.isdir(filepath):
        full_input = os.path.join(filepath, f'{filename}.txt')
        # if not re.match(regex, filename):
        # continue
        files.append(full_input)

files.sort()
for filename in files:
    input_file_string = filename
    output_list = []
    best = None
    best_n = -1
    best_dist = -1
    for name in solomon_parameters:
        param = solomon_parameters[name]
        params = [str(param["lambda"]), str(param["alpha1"])]
        full_input = commands[0] + [input_file_string] + params
        output = subprocess.check_output(
            full_input).decode("utf-8").split('\n')
        num_routes = int(output[0].split(' ')[0])
        distance = float(output[1].split(' ')[0])
        if not best:
            best = name
            best_dist = distance
            best_n = num_routes
        if num_routes == best_n:
            if distance < best_dist:
                best = name
                best_dist = distance
                best_n = num_routes
        elif num_routes < best_n:
            best = name
            best_dist = distance
            best_n = num_routes
        distance = round(distance)
        output_list.append(f'({num_routes}, {distance})')

    for name in overlap_parameters:
        param = overlap_parameters[name]
        params = [str(param["lambda"]), str(param["alpha1"])]
        full_input = commands[1] + [input_file_string] + params
        output = subprocess.check_output(
            full_input).decode("utf-8").split('\n')
        num_routes = int(output[0].split(' ')[0])
        distance = float(output[1].split(' ')[0])
        if not best:
            best = name
            best_dist = distance
            best_n = num_routes
        if num_routes == best_n:
            if distance < best_dist:
                best = name
                best_dist = distance
                best_n = num_routes
        elif num_routes < best_n:
            best = name
            best_dist = distance
            best_n = num_routes

        distance = round(distance)
        output_list.append(f'({num_routes}, {distance})')

    counter[best] += 1
    print(row_format.format(os.path.split(filename)[-1],
                            f'[{best}: ({best_n}, {round(best_dist)})]', *output_list))

counter_output = []
for param in params_str:
    for key in counter:
        if key == param:
            counter_output.append(counter[key])

print(row_format.format("", "", *counter_output))
