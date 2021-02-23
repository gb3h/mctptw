import os
import re
import subprocess


parameters = {
    # Time heavy (alpha1 = 0, alpha2 = 1)
    "TIME_I0": {"insertionCriteria": 0, "mu": 1, "lambda": 1, "alpha1": 0},
    "TIME_I1": {"insertionCriteria": 1, "mu": 1, "lambda": 1, "alpha1": 0},
    "TIME_I2": {"insertionCriteria": 2, "mu": 1, "lambda": 1, "alpha1": 0},

    # Lambda heavy and time heavy
    "TIME_LAMBDA_I0": {"insertionCriteria": 0, "mu": 1, "lambda": 2, "alpha1": 0},
    "TIME_LAMBDA_I1": {"insertionCriteria": 1, "mu": 1, "lambda": 2, "alpha1": 0},
    "TIME_LAMBDA_I2": {"insertionCriteria": 2, "mu": 1, "lambda": 2, "alpha1": 0},

    # Lambda heavy and distance heavy
    "DISTANCE_LAMBDA_I0": {"insertionCriteria": 0, "mu": 1, "lambda": 2, "alpha1": 1},
    "DISTANCE_LAMBDA_I1": {"insertionCriteria": 1, "mu": 1, "lambda": 2, "alpha1": 1},
    "DISTANCE_LAMBDA_I2": {"insertionCriteria": 2, "mu": 1, "lambda": 2, "alpha1": 1},

    # Distance heavy
    "DISTANCE_I0": {"insertionCriteria": 0, "mu": 1, "lambda": 1, "alpha1": 1},
    "DISTANCE_I1": {"insertionCriteria": 1, "mu": 1, "lambda": 1, "alpha1": 1},
    "DISTANCE_I2": {"insertionCriteria": 2, "mu": 1, "lambda": 1, "alpha1": 1},
}


counter = {}
params_str = []
for name in parameters:
    param = parameters[name]
    params = str(param["insertionCriteria"]) + str(param["mu"]
                                                   ) + str(param["lambda"]) + str(param["alpha1"])
    params_str.append(params)
    counter[params] = 0
row_format = "{:<17}" + "{:<16}" + "{:>12}" * (len(params_str))
print(row_format.format("", "Best", *params_str))

regex = re.compile('(0025|0050)+')
command = [f'./mctptw.out']
for filename in os.listdir('solomon_problems'):
    if not re.match(regex, filename):
        continue
    input_file_string = f'solomon_problems/{filename}'
    output_list = []
    best = None
    best_n = -1
    best_dist = -1
    for name in parameters:
        param = parameters[name]
        params = [str(param["insertionCriteria"]), str(
            param["mu"]), str(param["lambda"]), str(param["alpha1"])]
        param_str = ''.join(params)
        full_input = command + [input_file_string] + params
        output = subprocess.check_output(
            full_input).decode("utf-8").split('\n')
        num_routes = int(output[0].split(' ')[0])
        distance = float(output[1].split(' ')[0])
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
