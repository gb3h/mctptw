#ifndef ROUTE_H
#define ROUTE_H

#include <cstdio>
#include <list>
#include <vector>
#include "visit.h"
#include "problem.h"

class route{
private:
	bool push_forward_helper(int i_index, customer &u, problem &input, bool set); // put u in between i and i + 1 on route
public:
	void print(FILE *fp) const;
	void clear();
	bool get_push_forward_feasibility(int i_index, customer &u, problem &input); // put u in between i and i + 1 on route
	bool set_push_forward(int i_index, customer &u, problem &input); // put u in between i and i + 1 on route

	std::vector<visit> visits;
	double distance, waiting;
	int load;
	bool feasible, modified;
};

#endif