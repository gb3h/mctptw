# MCTPTW

The Multi-vehicle Covering Tour Problem with Time Windows

## Usage

Compilation: `g++ [-o filename] *.cpp`

E.g.: `g++ -o mctptw.out *.cpp`

Running: `./mctptw.out input_file [lambda alpha_1]`

E.g.: `./mctptw.out covering_problems/0025_0075_RC208_CTW_CC.txt 1 0.3`

## Problem Description

MCTPTW is a variant of the Vehicle Routing Problem with Time Windows (VRPTW) that splits vertices into the sets V and W, where vertices in W are covered by one or more vertices in V. See the CTP original formulation by Gendreau et al. (1972).

To deliver to a customer in W, a delivery van must park at a vertex V that covers W and travel to W from V whilst maintaining Time Window constraints.

Note:
When every vertex in W is covered by exactly one vertex in V, we get a reduction to an instance of VRPTW.

## Benchmark Problem Sets (MCTPTW)

- [MCTPTW problem sets] (25 parking spots, 75 customers)
  Generated from Solomon's problem sets using the python script in /scripts/coverter.py

## Benchmark Problem Sets (VRPTW)

- [Solomon's problem sets] (http://w.cba.neu.edu/~msolomon/problems.htm) (25, 50, and 100 customers)
- [Gehring & Homberger's extended benchmark] (http://www.sintef.no/Projectweb/TOP/VRPTW/Homberger-benchmark/) (200, 400, 600, 800, and 1000 customers)

## Best Known Solutions (VRPTW)

- [VRPTW best solutions for Solomon's benchmarks] (http://sun.aei.polsl.pl/~zjc/best-solutions-solomon.html) (from Zbigniew J. Czech, 100 customers only)
- [VRPTW best solutions for Gehring and Homberger's benchmarks] (http://sun.aei.polsl.pl/~zjc/BestSolutionsGH.html) (from Zbigniew J. Czech)

## Dataset Format (MCTPTW)

    vehicle_capacity
    covering_radius
    number_of_parking_spots
    number_of_customers

Remaining lines are each vertex's information:

    id  x_pos  y_pos  demand  ready_time  due_time  unloading_time
