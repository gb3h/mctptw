# Script to display a mctptw test set and result from a run
# Example use: python visualiser.py ../mctptw.out ../covering_problems/0025_0050_C101.txt 1 0.4
import sys
import re
import subprocess
import matplotlib._color_data as mcd
import matplotlib.path as path
import matplotlib.pyplot as plt
import matplotlib.patches as patches
import numpy as np

VERTEX_PROP = ['x', 'y', 'demand', 'start', 'end', 'service']
COLORS = ['orange', 'g', 'b', 'purple', 'brown', 'black']
num_v_w = []
parkings = []
customers = []
depot = []


def load(filename, axes):
    f = open(filename, "r")
    _ = int(f.readline().strip())
    RADIUS = float(f.readline().strip())
    num_v = int(f.readline().strip())
    num_w = int(f.readline().strip())
    num_v_w.append(num_v)
    num_v_w.append(num_w)
    depot_raw = f.readline().strip().split()
    depot.append(dict(zip(VERTEX_PROP, [int(x) for x in depot_raw[1:]])))

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

    # Add depot
    axes.scatter(depot[0]['x'], depot[0]['y'], s=8, c="b", alpha=1, marker="s",
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


def parse_str_route(res):
    route = []
    for str_tup in re.sub(", ", ",", res).split():
        x, y = re.sub(r'[()]', '', str_tup).split(",")
        route.append((int(x) - 1, int(y) - num_v_w[0] - 1))
    return route


def route_to_path(route, color):
    string_path_data = []
    string_path_data.append(
        (patches.Path.MOVETO, (depot[0]['x'], depot[0]['y'])))
    for visit in route:
        parking, customer = visit
        parking_path = (patches.Path.LINETO,
                        (parkings[parking]['x'], parkings[parking]['y']))
        string_path_data.append(parking_path)
        string_path_data.append(
            (patches.Path.LINETO, (customers[customer]['x'], customers[customer]['y'])))
        string_path_data.append(parking_path)

    string_path_data.append(
        (patches.Path.LINETO, (depot[0]['x'], depot[0]['y'])))
    codes, verts = zip(*string_path_data)
    string_path = path.Path(verts, codes)
    patch = patches.PathPatch(
        string_path, facecolor="none", ec=color, lw=1, alpha=1)
    return patch


def draw_routes(res, axes):
    routes = [parse_str_route(s) for s in res[0]]
    path_patches = [route_to_path(route, COLORS[ind])
                    for ind, route in enumerate(routes)]
    [axes.add_patch(path) for path in path_patches]
    textstr = '\n'.join((
        f'Num routes={len(res[0])}',
        f'Total distance={res[1]}',
        f'Bot distance={res[2]}'))

    axes.text(1.02, 0.5, textstr, transform=axes.transAxes,
              va="center", ha="left")


def run_mctptw(command, filename, params):
    full_input = [command, filename] + params
    output = subprocess.check_output(
        full_input).decode("utf-8").split('\n')

    n_routes = int(output[0].split()[0])
    dist = float(output[1].split()[0])
    bot_dist = float(output[2].split()[0])
    routes = []
    for i in range(3, n_routes + 3):
        curr = output[i].split(":")[1].strip()
        routes.append(curr)
    return (routes, dist, bot_dist)


if __name__ == "__main__":
    if len(sys.argv) < 3:
        raise Exception('No file given')
    command = sys.argv[1]
    filename = sys.argv[2]
    params = ["1", "0.1"]

    if len(sys.argv) == 5:
        params[0] = sys.argv[3]
        params[1] = sys.argv[4]

    _, axes = plt.subplots()
    axes.set_aspect(1)

    load(filename, axes)
    # res = run_mctptw(command, filename, params)
    # draw_routes(res, axes)

    plt.xlabel("X Coordinate")
    plt.ylabel("Y Coordinate")
    plt.legend(bbox_to_anchor=(1.5, 1))
    plt.show()
