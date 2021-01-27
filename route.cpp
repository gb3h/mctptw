#include <algorithm>

#include "route.h"

using namespace std;

void route::print(FILE *fp) const {
	fprintf(fp, "%d customers, distance = %.3f :", visits.size() - 1, distance);
	
	for(auto visit: visits){
		fprintf(fp, " %d", visit.cust.id);
	}

	fprintf(fp, "\n");
}

void route::clear(){
	visits.clear();
	distance = waiting = load = capacity = 0;
	modified = true;
}

bool route::push_forward_helper(int i_index, const customer &u, const problem &input, bool set) {
	double travelTime = input.getDistance(visits[i_index].cust.id, u.id);
	int arrival = visits[i_index].departure + travelTime;
	visit vis = visit(u, arrival);
	if (!vis.feasible) {
		return false;
	}

	visit &curr = vis;
	visit &next = vis;
	int newArrival, newDeparture;
	newDeparture = curr.departure;

	if (set) curr.push_forward(arrival);

	for (int i = i_index + 1; i <= visits.size(); i++) {
		if (i == visits.size()) {
			// final check from current node back to depot
			int latestDepotArrival = input[0].end - input[0].unload;
			travelTime = input.getDistance(0, curr.cust.id);
			if (!(newDeparture + travelTime <= latestDepotArrival)) return false;
		} else {
			next = visits[i];
			travelTime = input.getDistance(next.cust.id, curr.cust.id);
			newArrival = newDeparture + travelTime;

			if (!next.check_push_forward_feasiblity(newArrival)) {
				return false;
			}

			newDeparture = next.get_push_forward_new_departure(newArrival);
			if (set) next.push_forward(newArrival);

			curr = next;
		}
	}
	if (set) {
		vector<visit>::iterator it = visits.begin();
		it += i_index + 1;
		visits.insert(it, vis);
	}
	return true;
}

bool route::check_push_forward(int i_index, const customer &u, const problem &input) {
	return push_forward_helper(i_index, u, input, false);
}

bool route::set_push_forward(int i_index, const customer &u, const problem &input) {
	return push_forward_helper(i_index, u, input, true);
}
