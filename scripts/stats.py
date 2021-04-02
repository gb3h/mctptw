import sys
import re
import os


if __name__ == "__main__":
    if len(sys.argv) < 2:
        raise Exception('No file given')
    filename = sys.argv[1]
    solomon = [0, 0]
    heuristic = [0, 0]
    with open(filename, "r") as f:
        lines = f.readlines()
        for line in lines:
            line = line.strip().split()
            solo = (int(line[4][1:-1]), int(line[5][:-1]))
            ours = (int(line[6][1:-1]), int(line[7][:-1]))
            solomon[0] += solo[0]
            solomon[1] += solo[1]
            heuristic[0] += ours[0]
            heuristic[1] += ours[1]
    with open(filename, "a") as f:
        f.writelines([f'{solomon} {heuristic}'])
