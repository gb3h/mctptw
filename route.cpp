#include <algorithm>
#include <iostream>
#include <cmath>
#include "route.h"

using namespace std;

void route::print(FILE *fp) const {
	fprintf(fp, "%d customers, distance = %.3f, load = %d, capacity = %d:", visits.size() - 2, distance, load, capacity);
	
	for(int i = 1; i < visits.size() - 1; i++){
		fprintf(fp, " %d", visits[i].cust.id);
	}

	fprintf(fp, "\n");
}

void route::clear(){
	visits.clear();
	distance = waiting = load = capacity = 0;
	modified = true;
}

double route::get_distance(const problem &input) {
	double dist = 0;
	for (int i = 1; i < visits.size(); i++) {
		dist += input.getDistance(visits[i - 1].cust.id, visits[i].cust.id);
	}
	dist += input.getDistance(visits[visits.size() - 1].cust.id, 0);
	return dist;
}

visit route::initialise_insertion(int i_index, const customer &u, const problem &input) {
	double travelTime = input.getDistance(visits[i_index].cust.id, u.id);
	int arrival = visits[i_index].departure + travelTime;
	visit vis = visit(u, arrival);
	return vis;
}

bool route::push_forward_helper(int i_index, const customer &u, const problem &input, bool set) {
	if (load + u.demand > capacity) {
		return false;
	}

	visit vis = initialise_insertion(i_index, u, input);
	if (!vis.feasible) {
		return false;
	}

	visit curr = vis;
	visit next = vis;
	int oldArrival, newArrival, pushForward, nextPushForward;
	double travelTime;
	int oldStart, newStart;

	// Do work to get the first PushForward value
	next = visits[i_index + 1];
	travelTime = input.getDistance(next.cust.id, curr.cust.id);
	oldArrival = next.arrival;
	newArrival = curr.departure + travelTime;
	oldStart = fmax(oldArrival, next.cust.start);
	newStart = fmax(newArrival, next.cust.start);
	pushForward = newStart - oldStart;

	// Early terminate pushforward if there's enough "slack"
	for (int j = i_index + 1; j < visits.size(); j++) {
		if (pushForward <= 0) {
			if (pushForward < 0) {
				cout << "Something is wrong" << endl;
			}
			break;
		}

		next = visits[j];
		bool feasible = next.check_push_forward_feasiblity(pushForward);
		if (!feasible) {
			return false;
		}
		nextPushForward = next.get_next_push_forward(pushForward);
		if (set) {
			next.push_forward(pushForward);
			curr = next;
		} else {
			visit copy = visit(next);
			copy.push_forward(pushForward);
			curr = copy;
		}
		pushForward = nextPushForward;
	}

	if (set) {
		double minusTravelTime = input.getDistance(visits[i_index].cust.id, visits[i_index + 1].cust.id);
		double travelTime = input.getDistance(visits[i_index].cust.id, u.id) + input.getDistance(visits[i_index + 1].cust.id, u.id);
		vector<visit>::iterator it = visits.begin();
		it += i_index + 1;
		visits.insert(it, vis);
		load += vis.cust.demand;
		distance += travelTime - minusTravelTime;
	}

	return true;
}

bool route::check_push_forward(int i_index, const customer &u, const problem &input) {
	return push_forward_helper(i_index, u, input, false);
}

bool route::set_push_forward(int i_index, const customer &u, const problem &input) {
	return push_forward_helper(i_index, u, input, true);
}

double route::get_fitness(int i_index, const customer &u, const problem &input, double mu, double lambda, double alpha_1) {
	visit prev = visits[i_index];
	visit next = visits[i_index + 1];

	visit vis = initialise_insertion(i_index, u, input);
	double d_iu = input.getDistance(prev.cust.id, u.id);
	double d_uj = input.getDistance(u.id, next.cust.id);
	double d_ij = input.getDistance(prev.cust.id, next.cust.id);
	double c_11 = d_iu + d_uj - mu*d_ij;

	int travelTime = input.getDistance(next.cust.id, vis.cust.id);
	int newArrival = vis.departure + travelTime;
	int b_j = fmax(next.arrival, next.cust.start);
	int b_ju = fmax(newArrival, next.cust.start);
	double c_12 = b_j - b_ju;

	double alpha_2 = 1 - alpha_1;
	double c_1 = alpha_1*c_11 + alpha_2*c_12;

	double d_0u = input.getDistance(0, u.id);
	double c_2 = lambda*d_0u - c_1;
	return c_2;
}

bool route::check_feasibility(const problem &input) {
	return false;
}
