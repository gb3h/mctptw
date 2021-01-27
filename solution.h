#ifndef SOLUTION_H
#define SOLUTION_H

#include <cstdio>
#include <list>
#include <vector>
#include "customer.h"
#include "route.h"
#include "problem.h"

class solution{
public:
	void print(FILE *fp) const;
	void clear();
	void random(const problem& input);
	void solomon(const problem& input, double mu = 1, double lambda = 2, double alpha1 = 0);
	static int cmp(const solution &solA, const solution &solB, const problem &input);
	static bool fdominate(const solution &solA, const solution &solB);
	bool operator == (const solution &another) const;
	bool operator < (const solution &another) const;

	std::list<route> routes;
	double totalDistance, totalWaiting;
	int unbalancedCapacity, exceededCapacity;
	bool feasible;
};

#endif