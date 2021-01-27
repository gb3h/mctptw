#include <cmath>
#include "visit.h"

using namespace std;

visit::visit(customer &cust, int arrival) : 
	cust(cust), arrival(arrival), departure(fmax(arrival, cust.start) + cust.unload), feasible(departure <= cust.end) {
}

int visit::get_push_forward_new_departure(int newArrival) {
	return fmax(newArrival, cust.start) + cust.unload;
}

bool visit::check_push_forward_feasiblity(int newArrival) {
	return get_push_forward_new_departure(newArrival) <= cust.end;
}

bool visit::push_forward(int newArrival) {
	departure = get_push_forward_new_departure(newArrival);
	feasible = departure <= cust.end;
	return feasible;
}
