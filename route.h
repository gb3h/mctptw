#ifndef ROUTE_H
#define ROUTE_H

#include <cstdio>
#include <list>
#include <vector>
#include <cassert>
#include "visit.h"
#include "problem.h"

class route{
private:
	visit initialise_insertion(int i_index, const customer &u, const problem &input);
	bool push_forward_helper(int i_index, const customer &u, const problem &input, bool set);
public:
	void print(FILE *fp) const;
	void clear();
	bool check_push_forward(int i_index, const customer &u, const problem &input); // check feasibility of u in between i and i + 1 on route
	bool set_push_forward(int i_index, const customer &u, const problem &input); // put u in between i and i + 1 on route
	double get_fitness(int i_index, const customer &u, const problem &input, double mu, double lambda, double alpha_1);
	double get_distance(const problem &input);
	bool check_feasibility(const problem &input);

	std::vector<visit> visits;
	double distance, waiting;
	int load, capacity;
	bool feasible, modified;
};

#endif