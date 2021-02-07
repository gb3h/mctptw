MCTPTW
=====

"Multi-vehicle Covering Tour Problem with Time Windows"

## Problem Description

A variant of the Vehicle Routing Problem with Time Windows (VRPTW) that splits vertices into the set V and W, where vertices in W are covered by 1 or more vertices in V. See the CTP original formulation by Gendreau et al. (1972).

## Benchmark Problem Sets (VRPTW)

 * [Solomon's problem sets] (http://w.cba.neu.edu/~msolomon/problems.htm) (25, 50, and 100 customers)
 * [Gehring & Homberger's extended benchmark] (http://www.sintef.no/Projectweb/TOP/VRPTW/Homberger-benchmark/) (200, 400, 600, 800, and 1000 customers)

## Best Known Solutions (VRPTW)

 * [VRPTW best solutions for Solomon's benchmarks] (http://sun.aei.polsl.pl/~zjc/best-solutions-solomon.html) (from Zbigniew J. Czech, 100 customers only)
 * [VRPTW best solutions for Gehring and Homberger's benchmarks] (http://sun.aei.polsl.pl/~zjc/BestSolutionsGH.html) (from Zbigniew J. Czech)

## Dataset Format (VRPTW)

    vehicle_capacity
    number_of_customers
     
  Remaining lines are each customer's information:
  
    id  x_pos  y_pos  demand  ready_time  due_time  unloading_time
     
  