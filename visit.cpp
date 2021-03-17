#include <cmath>
#include "visit.h"
#include <iostream>

using namespace std;

visit::visit(const customer &park, const customer &cust, double arrival, double distance, double departure) : park(park), cust(cust), arrival(arrival), distance(distance), departure(fmax(departure, getEarliestDeparture()))
{
}

void visit::print(FILE *fp) const
{
	fprintf(fp, "Park id: %d, Cust id: %d, arrival = %.2f, departure = %.2f, start = %d, end = %d, unload = %d\n",
			park.id, cust.id, arrival, departure, cust.start, cust.end, cust.unload);
}

double visit::get_next_push_forward(double pushForward)
{
	double newArrival = arrival + pushForward;
	double oldStart = fmax(arrival, cust.start);
	double newStart = fmax(newArrival, cust.start);
	return newStart - oldStart; // should always be >= 0. if 0, no need to propagate.
}

bool visit::check_push_forward_feasiblity(double pushForward)
{
	double newArrival = arrival + pushForward;
	return newArrival + distance <= cust.end;
}

double visit::push_forward(double pushForward)
{
	arrival = arrival + pushForward;
	departure = fmax(departure, getEarliestDeparture());
	return 1;
}

double visit::getEarliestStart()
{
	return cust.start - distance;
}

double visit::getEffectiveServiceTime()
{
	return cust.unload + (2 * distance);
}

double visit::getLatestEnd()
{
	return cust.end + getEffectiveServiceTime() + distance;
}

double visit::getEarliestDeparture()
{
	double serviceTime = getEffectiveServiceTime();
	double earliestStartTime = getEarliestStart();
	double serviceStart = fmax(arrival, earliestStartTime);
	return serviceStart + serviceTime;
}

bool visit::feasible()
{
	return arrival + distance <= cust.end;
}
