#ifndef SOLUTION_H
#define SOLUTION_H

#include <cstdio>
#include <list>
#include <vector>
#include "customer.h"
#include "route.h"
#include "problem.h"

class solution
{
private:
	std::vector<std::vector<int>> generate_partitions(int n);
	std::vector<std::vector<int>> perm_generator(std::vector<int> &arr, int k);

public:
	void print(FILE *fp) const;
	void clear();
	void solomon(const problem &input, int insertionCriteria = 1, double mu = 1, double lambda = 2, double alpha_1 = 0.5);
	void brute_force(const problem &input);

	std::vector<route> routes;
	double totalDistance, totalWaiting;
	int unbalancedCapacity, exceededCapacity;
	bool feasible;
};

#endif