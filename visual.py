import sys
import re
import matplotlib._color_data as mcd
import matplotlib.path as path
import matplotlib.pyplot as plt
import matplotlib.patches as patches
import numpy as np

VERTEX_PROP = ['x', 'y', 'demand', 'start', 'end', 'service']
COLORS = ['black', 'g', 'b', 'purple', 'brown', 'orange']


def load(filename):
    parkings = []
    customers = []
    f = open(filename, "r")
    _ = int(f.readline().strip())
    RADIUS = int(f.readline().strip())
    num_v = int(f.readline().strip())
    num_w = int(f.readline().strip())
    depot_raw = f.readline().strip().split()
    depot = dict(zip(VERTEX_PROP, [int(x) for x in depot_raw[1:]]))

    def parse_str_route(res):
        route = []
        for str_tup in re.sub(", ", ",", res).split():
            x, y = re.sub(r'[()]', '', str_tup).split(",")
            route.append((int(x) - 1, int(y) - num_v - 1))
        return route

    def route_to_path(route, color):
        string_path_data = []
        string_path_data.append(
            (patches.Path.MOVETO, (depot['x'], depot['y'])))
        for visit in route:
            parking, customer = visit
            parking_path = (patches.Path.LINETO,
                            (parkings[parking]['x'], parkings[parking]['y']))
            string_path_data.append(parking_path)
            string_path_data.append(
                (patches.Path.LINETO, (customers[customer]['x'], customers[customer]['y'])))
            string_path_data.append(parking_path)

        string_path_data.append(
            (patches.Path.LINETO, (depot['x'], depot['y'])))
        codes, verts = zip(*string_path_data)
        string_path = path.Path(verts, codes)
        patch = patches.PathPatch(
            string_path, facecolor="none", ec=color, lw=1, alpha=1)
        return patch

    for _ in range(0, num_v):
        raw = f.readline().strip().split()
        vertex = dict(zip(VERTEX_PROP, [int(x) for x in raw[1:]]))
        parkings.append(vertex)
    for _ in range(0, num_w):
        raw = f.readline().strip().split()
        vertex = dict(zip(VERTEX_PROP, [int(x) for x in raw[1:]]))
        customers.append(vertex)

    parkings_x = [parking['x'] for parking in parkings]
    parkings_y = [parking['y'] for parking in parkings]

    customers_x = [customer['x'] for customer in customers]
    customers_y = [customer['y'] for customer in customers]

    _, axes = plt.subplots()

    # Add depot
    axes.scatter(depot['x'], depot['y'], s=8, c="b", alpha=1, marker="s",
                 label="Depot")

    # Add parking spots
    axes.scatter(parkings_x, parkings_y, s=8, c="y", alpha=1, marker="s",
                 label="Parking Spots")

    # Add coverage
    [axes.add_artist(patches.Circle((x, y), RADIUS, ec='y', alpha=0.6, fill=False))
     for (x, y) in zip(parkings_x, parkings_y)]

    # Add customers
    axes.scatter(customers_x, customers_y, s=8, c="r", alpha=1, marker="o",
                 label="Customers")

    axes.set_aspect(1)

    res = ["(20, 45) (20, 70) (22, 74) (22, 49) (22, 72) (22, 46) (22, 47) (22, 50) (22, 75) (8, 58) (8, 33) (8, 60) (8, 35) (8, 36) (8, 61) (8, 34) (6, 31) (6, 56) (4, 54) (4, 29) (2, 52) (2, 27) (2, 51) (2, 26)",
           "(5, 30) (5, 55) (5, 28) (5, 53) (5, 32) (5, 57) (19, 44) (19, 69) (15, 40) (15, 65) (14, 41) (14, 66) (14, 39) (14, 64) (12, 37) (12, 62) (21, 71)",
           "(13, 38) (13, 63) (13, 42) (17, 43) (17, 68) (6, 59) (23, 48) (23, 73)",
           "(17, 67)"
           ]
    routes = [parse_str_route(s) for s in res]
    path_patches = [route_to_path(route, COLORS[ind])
                    for ind, route in enumerate(routes)]

    [axes.add_patch(path) for path in path_patches]

    plt.xlabel("X Coordinate")
    plt.ylabel("Y Coordinate")
    plt.legend(bbox_to_anchor=(1.5, 1))
    plt.show()


if __name__ == "__main__":
    if len(sys.argv) < 2:
        raise Exception('No file given')
    filename = sys.argv[1]
    load(filename)
