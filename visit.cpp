#include <cmath>
#include "visit.h"

using namespace std;

visit::visit(const customer &cust, int arrival) : 
	cust(cust), arrival(arrival), departure(fmax(arrival, cust.start) + cust.unload), feasible(arrival <= cust.end) {
}

int visit::get_next_push_forward(int pushForward) {
	int newArrival = arrival + pushForward;
	int oldStart = fmax(arrival, cust.start);
	int newStart = fmax(newArrival, cust.start);
	return newStart - oldStart; // should always be >= 0. if 0, no need to propagate.
}

bool visit::check_push_forward_feasiblity(int pushForward) {
	int newArrival = arrival + pushForward;
	int newStart = fmax(newArrival, cust.start);
	return newStart <= cust.end;
}

int visit::push_forward(int pushForward) {
	arrival = arrival + pushForward;
	departure = fmax(arrival, cust.start) + cust.unload;
	feasible = arrival <= cust.end;
	int nextPushForward = get_next_push_forward(pushForward);
	return nextPushForward;
}
