# Script to run algorithm on a variety of parameters over different test sets and collate data

import os
import re
import subprocess


solomon_parameters = {
    "0.00": {"lambda": 0, "alpha1": 0},
    "0.50": {"lambda": 0, "alpha1": 0.5},
}


counter = {}
params_str = []
for name in solomon_parameters:
    params_str.append(name)
    counter[name] = 0

row_format = "{:<26}" + "{:<14}" + "{:>12}" * len(params_str) + "{:>16}"
print(row_format.format("", "Best", *params_str, "AVG TIME"))

regex = re.compile('(0025_0200)+')
commands = [[f'../mctptw.out'], [f'../mctptw_overlap.out']]
files = []
for filename in os.listdir('../covering_problems'):
    filepath = os.path.join('../covering_problems', filename)
    if os.path.isdir(filepath):
        full_input = os.path.join(filepath, f'{filename}.txt')
        if not re.match(regex, filename):
            continue
        files.append(full_input)

total_total = 0
counter_1 = 0
files.sort()
for filename in files:
    counter_1 += 1
    input_file_string = filename
    output_list = []
    best = None
    best_n = -1
    best_dist = -1
    total_time = 0
    for name in solomon_parameters:
        param = solomon_parameters[name]
        params = [str(param["lambda"]), str(param["alpha1"])]
        full_input = commands[0] + [input_file_string] + params
        output = subprocess.check_output(
            full_input).decode("utf-8").split('\n')
        cpu = float(output[0].split(' ')[0])
        total_time += cpu
        output = output[1:]
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
    total_total += total_time
    print(row_format.format(os.path.split(filename)[-1].zfill(22),
                            f'[{best}: ({best_n}, {round(best_dist)})]', *output_list, round(total_time/len(output_list), 3)))

counter_output = []
for param in params_str:
    for key in counter:
        if key == param:
            counter_output.append(counter[key])

print(row_format.format("", "", *counter_output, round(total_total/counter_1, 3)))
