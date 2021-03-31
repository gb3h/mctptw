# Script to run algorithm on a variety of parameters over different test sets and collate data

import os
import re
import subprocess


parameters = {
    # Solomon's Default (alpha1 = 0, alpha2 = 1)
    "DEFAULT_L0": {"lambda": 0, "alpha1": 0},
    "DEFAULT_L1": {"lambda": 1, "alpha1": 0},
    "DEFAULT_L2": {"lambda": 2, "alpha1": 0},
    "DEFAULT_L3": {"lambda": 3, "alpha1": 0},

    # POINT1 Weighted Overlap
    "POINT1_L0": {"lambda": 0, "alpha1": 0.1},
    "POINT1_L1": {"lambda": 1, "alpha1": 0.1},
    "POINT1_L2": {"lambda": 2, "alpha1": 0.1},
    "POINT1_L3": {"lambda": 3, "alpha1": 0.1},

    # POINT2 Weighted Overlap
    "POINT2_L0": {"lambda": 0, "alpha1": 0.2},
    "POINT2_L1": {"lambda": 1, "alpha1": 0.2},
    "POINT2_L2": {"lambda": 2, "alpha1": 0.2},
    "POINT2_L3": {"lambda": 3, "alpha1": 0.2},

    # POINT3 Weighted Overlap
    "POINT3_L0": {"lambda": 0, "alpha1": 0.3},
    "POINT3_L1": {"lambda": 1, "alpha1": 0.3},
    "POINT3_L2": {"lambda": 2, "alpha1": 0.3},
    "POINT3_L3": {"lambda": 3, "alpha1": 0.3},

    # POINT4 Weighted Overlap
    "POINT4_L0": {"lambda": 0, "alpha1": 0.4},
    "POINT4_L1": {"lambda": 1, "alpha1": 0.4},
    "POINT4_L2": {"lambda": 2, "alpha1": 0.4},
    "POINT4_L3": {"lambda": 3, "alpha1": 0.4},
}


counter = {}
params_str = []
for name in parameters:
    param = parameters[name]
    params = f'{param["lambda"]},{param["alpha1"]}'
    params_str.append(params)
    counter[params] = 0
row_format = "{:<26}" + "{:<16}" + "{:>12}" * (len(params_str))
print(row_format.format("", "Best", *params_str))

regex = re.compile('(0025_0075_R)+')
command = [f'../mctptw.out']
files = []
for filename in os.listdir('../covering_problems'):
    if not re.match(regex, filename):
        continue
    files.append(filename)
files.sort()
for filename in files:
    input_file_string = f'../covering_problems/{filename}'
    output_list = []
    best = None
    best_n = -1
    best_dist = -1
    for name in parameters:
        param = parameters[name]
        params = [str(param["lambda"]), str(param["alpha1"])]
        full_input = command + [input_file_string] + params
        output = subprocess.check_output(
            full_input).decode("utf-8").split('\n')
        num_routes = int(output[0].split(' ')[0])
        distance = float(output[1].split(' ')[0])
        param_str = ','.join(params)
        if not best:
            best = param_str
            best_dist = distance
            best_n = num_routes
        if num_routes == best_n:
            if distance < best_dist:
                best = param_str
                best_dist = distance
                best_n = num_routes
        elif num_routes < best_n:
            best = param_str
            best_dist = distance
            best_n = num_routes

        distance = round(distance)
        output_list.append(f'({num_routes}, {distance})')

    counter[best] += 1
    print(row_format.format(filename,
                            f'[{best}: ({best_n}, {round(best_dist)})]', *output_list))

counter_output = []
for param in params_str:
    for key in counter:
        if key == param:
            counter_output.append(counter[key])

print(row_format.format("", "", *counter_output))
