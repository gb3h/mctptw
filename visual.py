import sys
import matplotlib.pyplot as plt
import matplotlib.patches as patches
import numpy as np

VERTEX_PROP = ['x', 'y', 'demand', 'start', 'end', 'service']


def load(filename):
    parkings = []
    customers = []
    f = open(filename, "r")
    _ = int(f.readline().strip())
    v = int(f.readline().strip())
    w = int(f.readline().strip())
    depot_raw = f.readline().strip().split()
    depot = dict(zip(VERTEX_PROP, [int(x) for x in depot_raw[1:]]))
    for _ in range(1, v + 1):
        raw = f.readline().strip().split()
        vertex = dict(zip(VERTEX_PROP, [int(x) for x in raw[1:]]))
        parkings.append(vertex)
    for _ in range(1, w + 1):
        raw = f.readline().strip().split()
        vertex = dict(zip(VERTEX_PROP, [int(x) for x in raw[1:]]))
        customers.append(vertex)

    parkings_x = [parking['x'] for parking in parkings]
    parkings_y = [parking['y'] for parking in parkings]

    customers_x = [customer['x'] for customer in customers]
    customers_y = [customer['y'] for customer in customers]

    _, axes = plt.subplots()

    axes.scatter([depot['x']], depot['y'], s=8, c="b", alpha=1, marker="s",
                 label="Depot")

    axes.scatter(parkings_x, parkings_y, s=8, c="g", alpha=0.5, marker="s",
                 label="Parking Spots")

    axes.scatter(customers_x, customers_y, s=8, c="r", alpha=0.5, marker="o",
                 label="Customers")

    [axes.add_artist(patches.Circle((x, y), 3, color='y', alpha=0.2, fill=True))
     for (x, y) in zip(parkings_x, parkings_y)]

    axes.set_aspect(1)

    plt.xlabel("X Coordinate")
    plt.ylabel("Y Coordinate")
    plt.legend(bbox_to_anchor=(1.5, 1))
    plt.show()


if __name__ == "__main__":
    if len(sys.argv) < 2:
        raise Exception('No file given')
    filename = sys.argv[1]
    load(filename)
