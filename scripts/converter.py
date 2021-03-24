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
    "window_offset": 0,
    "depot": {},
    "capacity": 0
}

MODES = {
    "_CTW_CC": {
        "cust_per_parking": 3,
        "num_parking": 0,
        "num_cust": 0,
        "radius": 2,
        "window_offset": 10,
        "depot": {},
        "capacity": 0
    },
    "_CTW_SC": {
        "cust_per_parking": 3,
        "num_parking": 0,
        "num_cust": 0,
        "radius": 4,
        "window_offset": 10,
        "depot": {},
        "capacity": 0
    },
    "_STW_CC": {
        "cust_per_parking": 3,
        "num_parking": 0,
        "num_cust": 0,
        "radius": 2,
        "window_offset": 200,
        "depot": {},
        "capacity": 0
    },
    "_STW_SC": {
        "cust_per_parking": 3,
        "num_parking": 0,
        "num_cust": 0,
        "radius": 4,
        "window_offset": 200,
        "depot": {},
        "capacity": 0
    },
}

MODE = "_CTW_CC"
SETTINGS = MODES[MODE]


def load(filename):
    f = open(filename, "r")
    SETTINGS["capacity"] = int(f.readline().strip())
    SETTINGS["num_parking"] = int(f.readline().strip())
    depot_raw = f.readline().strip().split()
    SETTINGS["depot"] = dict(zip(VERTEX_PROP, [int(x) for x in depot_raw[1:]]))

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
            window_offset = random.randint(-SETTINGS["window_offset"],
                                           SETTINGS["window_offset"])

            cust['start'] = max(0, cust['start'] + window_offset)
            cust['end'] = min(SETTINGS["depot"]["end"],
                              cust['end'] + window_offset)

            if dist(SETTINGS["depot"], cust) > (cust['end'] - SETTINGS["radius"]):
                cust['end'] = min(dist(SETTINGS["depot"], cust) +
                                  2 * SETTINGS["radius"], SETTINGS["depot"]["end"])
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


if __name__ == "__main__":
    if len(sys.argv) < 3:
        raise Exception('No file given')
    filepath = sys.argv[1]
    outdir = sys.argv[2]
    load(filepath)

    generate()

    filename = os.path.split(filepath)[-1]
    num, rest = filename.split(".txt")[0].split("_")
    outfile = os.path.join(
        outdir, f'{num}_{str(SETTINGS["num_cust"]).zfill(4)}_{rest}{MODE}.txt')
    write_out(outfile)
