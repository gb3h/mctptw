# Script that takes in a solomon dataset and outputs a mctptw dataset
# Example usage: python3 converter.py ../solomon_problems/0025_C101.txt ../covering_problems/
import math
import sys
import re
import os
import random
import subprocess

# Repeatability
random.seed(98172)

VERTEX_PROP = ['x', 'y', 'demand', 'start', 'end', 'service']
PARKINGS = []
CUSTOMERS = []

SETTINGS = {
    "cust_per_parking": 0,
    "num_parking": 0,
    "num_cust": 0,
    "radius": 0,
    "depot": {},
    "capacity": 0
}

MODES = {
    "_S": {
        "cust_per_parking": 4,
        "num_parking": 0,
        "num_cust": 0,
        "radius": 2,
        "depot": {},
        "capacity": 0
    },
    "_D": {
        "cust_per_parking": 4,
        "num_parking": 0,
        "num_cust": 0,
        "radius": 3,
        "depot": {},
        "capacity": 0
    },
    # "_S": {
    #     "cust_per_parking": 8,
    #     "num_parking": 0,
    #     "num_cust": 0,
    #     "radius": 3,
    #     "depot": {},
    #     "capacity": 0
    # },
    # "_D": {
    #     "cust_per_parking": 8,
    #     "num_parking": 0,
    #     "num_cust": 0,
    #     "radius": 3,
    #     "depot": {},
    #     "capacity": 0
    # },
}

WINDOW_SETTINGS = {
    "_01": {
        "window_chance": 25,
        "window_proportion": 0.05,
    },
    "_02": {
        "window_chance": 50,
        "window_proportion": 0.05,
    },
    "_03": {
        "window_chance": 75,
        "window_proportion": 0.05,
    },
    "_04": {
        "window_chance": 100,
        "window_proportion": 0.05,
    },
    "_05": {
        "window_chance": 100,
        "window_proportion": 0.1,
    },
    "_06": {
        "window_chance": 100,
        "window_proportion": 0.2,
    },
}


def load(filename):
    f = open(filename, "r")
    SETTINGS["capacity"] = int(f.readline().strip())
    SETTINGS["num_parking"] = int(f.readline().strip())
    depot_raw = f.readline().strip().split()
    depot = dict(zip(VERTEX_PROP, [int(x) for x in depot_raw[1:]]))
    SETTINGS["depot"] = depot

    SETTINGS["num_cust"] = SETTINGS["num_parking"] * \
        SETTINGS["cust_per_parking"]

    for _ in range(0, SETTINGS["num_parking"]):
        raw = f.readline().strip().split()
        vertex = dict(zip(VERTEX_PROP, [int(x) for x in raw[1:]]))
        PARKINGS.append(vertex)


def dist(x, y):
    return math.ceil(math.sqrt(((x['x'] - y['x']) ** 2) +
                               ((x['y'] - y['y']) ** 2)))


def generate():
    for parking in PARKINGS:
        for _ in range(0, SETTINGS["cust_per_parking"]):
            cust = parking.copy()
            cust['x'] += random.randint(-SETTINGS["radius"],
                                        SETTINGS["radius"])
            cust['y'] += random.randint(-SETTINGS["radius"],
                                        SETTINGS["radius"])

            CUSTOMERS.append(cust)


def write_out(outfile):
    with open(outfile, "w") as f:
        f.write("10000\n")
        radius = math.ceil(math.sqrt(2 * (SETTINGS["radius"]**2)))
        f.write(f'{radius}\n')
        f.write(f'{SETTINGS["num_parking"]}\n')
        f.write(f'{SETTINGS["num_cust"]}\n')
        f.write(f'{parking_to_string(0, SETTINGS["depot"])}\n')
        for index, parking in enumerate(PARKINGS):
            f.write(f'{parking_to_string(index + 1, parking)}\n')
        for index, customer in enumerate(CUSTOMERS):
            f.write(f'{customer_to_string(index + 1, customer)}\n')


def customer_to_string(id, vertex):
    return str(id) + "\t" + "\t".join([str(vertex[x]) for x in VERTEX_PROP])


def parking_to_string(id, vertex):
    vertex = vertex.copy()
    vertex["start"] = 0
    vertex["end"] = SETTINGS["depot"]["end"]
    vertex["service"] = 0
    return str(id) + "\t" + "\t".join([str(vertex[x]) for x in VERTEX_PROP])


def create_windows(window_type):
    start = SETTINGS["depot"]["start"]
    end = SETTINGS["depot"]["end"]
    length = math.floor(window_type["window_proportion"] * end)
    end = end - length
    for cust in CUSTOMERS:
        chance = random.randint(0, 99)
        if chance >= window_type["window_chance"]:
            cust["start"] = start
            cust["end"] = SETTINGS["depot"]["end"] - cust["service"]
        else:
            start_point = random.randint(start, end)
            cust["start"] = start_point
            cust["end"] = start_point + length
            radius = math.ceil(math.sqrt(2 * (SETTINGS["radius"]**2)))
            min_travel = dist(SETTINGS["depot"], cust) + (2 * radius)
            cust["end"] = max(cust["end"], min_travel)
            cust["start"] = min(cust["start"], SETTINGS["depot"]
                                ["end"] - min_travel - cust["service"])


def run(filepath, outdir):
    for mode in MODES:
        for key in SETTINGS:
            SETTINGS[key] = MODES[mode][key]
        PARKINGS.clear()
        CUSTOMERS.clear()
        load(filepath)
        generate()
        for window_type in WINDOW_SETTINGS:
            create_windows(WINDOW_SETTINGS[window_type])
            filename = os.path.split(filepath)[-1]
            num, rest = filename.split(".txt")[0].split("_")
            fname = f'{num}_{str(SETTINGS["num_cust"]).zfill(4)}_{rest}{mode}{window_type}'
            newdir = os.path.join(
                outdir, fname)
            if not os.path.isdir(newdir):
                os.mkdir(newdir)
            outfile = os.path.join(
                newdir, f'{fname}.txt')
            write_out(outfile)


if __name__ == "__main__":
    if len(sys.argv) < 3:
        raise Exception('No file given')
    indir = sys.argv[1]
    outdir = sys.argv[2]
    # regex = re.compile('(0025_C1)+')
    for filename in os.listdir(indir):
        # filename = "0025_C101.txt"
        # if re.match(regex, filename):
        # print(filename)
        filepath = os.path.join(indir, filename)
        run(filepath, outdir)
