#include <cmath>
#include "visit.h"
#include<iostream>

using namespace std;

visit::visit(const customer &cust, double arrival) :
	cust(cust), arrival(arrival), departure(fmax(arrival, cust.start) + cust.unload), feasible(arrival <= cust.end) {
}

void visit::print(FILE *fp) const {
	fprintf(fp, "Cust id: %d, arrival = %.2f, departure = %.2f, start = %d, end = %d, unload = %d\n", 
		cust.id, arrival, departure, cust.start, cust.end, cust.unload);
}

double visit::get_next_push_forward(double pushForward) {
	double newArrival = arrival + pushForward;
	double oldStart = fmax(arrival, cust.start);
	double newStart = fmax(newArrival, cust.start);
	return newStart - oldStart; // should always be >= 0. if 0, no need to propagate.
}

bool visit::check_push_forward_feasiblity(double pushForward) {
	double newArrival = arrival + pushForward;
	double newStart = fmax(newArrival, cust.start);
	return newStart <= cust.end;
}

double visit::push_forward(double pushForward) {
	arrival = arrival + pushForward;
	departure = fmax(arrival, cust.start) + cust.unload;
	double nextPushForward = get_next_push_forward(pushForward);
	return nextPushForward;
}
