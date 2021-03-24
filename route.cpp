#include <algorithm>
#include <iostream>
#include <cmath>
#include "route.h"

using namespace std;

void route::print(FILE *fp) const
{
	fprintf(fp, "%ld customers, distance = %.3f, load = %d, capacity = %d:",
			visits.size() - 2, distance, load, capacity);

	for (int i = 1; i < visits.size() - 1; i++)
	{
		fprintf(fp, " (%d, %d)", visits[i].park.id, visits[i].cust.id);
	}

	fprintf(fp, "\n");
}

void route::clear()
{
	visits.clear();
	distance = waiting = load = capacity = 0;
	modified = true;
}

double route::get_distance(const problem &input)
{
	double dist = 0;
	for (int i = 1; i < visits.size(); i++)
	{
		dist += input.getDistance(visits[i - 1].cust.id, visits[i].cust.id - input.getNumParking());
	}
	dist += input.getDistance(visits[visits.size() - 1].cust.id, 0);
	return dist;
}

visit route::initialise_insertion(int i_index, const customer &park, const customer &u, const problem &input)
{
	// Previous delivery's parking is elsewhere
	if (visits[i_index].park.id != park.id)
	{
		double travelTime = input.getDistance(visits[i_index].park.id, park.id);
		double arrival = visits[i_index].departure + travelTime;
		visit vis = visit(park, u, arrival, input.getDistance(park.id, u.id), 0);
		return vis;
	}
	else
	{
		visit vis = visit(park, u, visits[i_index].arrival, input.getDistance(park.id, u.id), visits[i_index].departure);
		return vis;
	}
}

bool route::push_forward_helper(int i_index, const customer &park, const customer &u, const problem &input, bool set)
{
	if (load + u.demand > capacity)
	{
		return false;
	}

	visit vis = initialise_insertion(i_index, park, u, input);

	if (!vis.feasible())
	{
		return false;
	}

	visit *curr = &vis;
	visit *next = &vis;

	double newLeave, oldLeave;
	bool updateLeave = false;

	double newArrival, oldArrival, travelTime;
	double pushForward = 0;

	for (int j = i_index + 1; j < visits.size(); j++)
	{
		next = &visits[j];
		// printf("Curr: %d, next: %d\n", curr->cust.id, next->cust.id);
		if (curr->park.id != next->park.id)
		{
			travelTime = input.getDistance(curr->park.id, next->park.id);
			newArrival = travelTime + curr->departure;
			oldArrival = next->arrival;
			pushForward = fmax(0, newArrival - oldArrival);
		}

		if (!next->check_push_forward_feasiblity(pushForward))
		{
			return false;
		}

		oldLeave = next->departure;
		if (set)
		{
			next->push_forward(pushForward);
			newLeave = next->departure;
			curr = next;
		}
		else
		{
			visit copy = *next;
			copy.push_forward(pushForward);
			newLeave = copy.departure;
			curr = &copy;
		}

		updateLeave = (newLeave > oldLeave);

		// Early terminate PushForward loop if there's enough "slack"
		if (pushForward == 0 && updateLeave == false)
		{
			break;
		}
	}

	if (set)
	{
		double minusTravelTime = input.getDistance(visits[i_index].park.id, visits[i_index + 1].park.id);
		double travelTime = input.getDistance(visits[i_index].park.id, park.id) + input.getDistance(visits[i_index + 1].park.id, park.id);
		vector<visit>::iterator it = visits.begin();
		it += i_index + 1;
		visits.insert(it, vis);
		load += vis.cust.demand;
		distance += travelTime - minusTravelTime;
		botDistance += 2 * vis.distance;
	}
	return true;
}

bool route::check_push_forward(int i_index, const customer &park, const customer &u, const problem &input)
{
	return push_forward_helper(i_index, park, u, input, false);
}

bool route::set_push_forward(int i_index, const customer &park, const customer &u, const problem &input)
{
	return push_forward_helper(i_index, park, u, input, true);
}

double route::get_c11_fitness(int i_index, const customer &park, const customer &u, const problem &input, set<int> unrouted)
{
	visit prev = visits[i_index];
	visit next = visits[i_index + 1];
	visit vis = initialise_insertion(i_index, park, u, input);
	vector<int> coverage = input.getCoverage(park.id);
	double c_11 = 0;
	for (int i = 0; i < coverage.size(); i++)
	{
		customer k = input[coverage[i]];
		if (!unrouted.count(k.id) || k.id == u.id)
		{
			continue;
		}
		double d_pk = input.getDistance(park.id, k.id);
		double curr = fmin(k.end + d_pk, vis.getEarliestDeparture()) - fmax(fmax(k.start - d_pk, 0), vis.arrival);
		if (curr > 0)
			c_11 += curr;
	}
	return c_11;
}

double route::get_c12_fitness(int i_index, const customer &park, const customer &u, const problem &input)
{
	visit prev = visits[i_index];
	visit next = visits[i_index + 1];
	visit vis = initialise_insertion(i_index, park, u, input);

	double travelTime = input.getDistance(next.park.id, vis.park.id);
	double oldArrival = next.arrival;
	double newArrival = vis.departure + travelTime;
	double oldLeave = next.departure;
	next.push_forward(newArrival - oldArrival);
	double newLeave = next.departure;
	double c_12 = newLeave - oldLeave;
	return c_12;
}

double route::get_c2_fitness(int i_index, const customer &park, const customer &u, const problem &input, double lambda, double c_1)
{
	double d_0u = input.getDistance(0, park.id);
	double c_2 = lambda * d_0u - c_1;
	return c_2;
}

bool route::check_feasibility(const problem &input)
{
	double newLoad = 0;
	double distance = 0;
	double botDistance = 0;

	for (int i = 1; i < visits.size() - 1; i++)
	{
		visit prev = visits[i - 1];
		visit curr = visits[i];
		visit next = visits[i + 1];

		newLoad += curr.cust.demand;
		botDistance += 2 * curr.distance;

		// Travel from prev to curr is correct
		if (prev.park.id != curr.park.id)
		{
			double d_iu = input.getDistance(prev.park.id, curr.park.id);
			distance += d_iu;
			if (prev.departure + d_iu - curr.arrival > 0.001)
			{
				print(stdout);
				prev.print(stdout);
				cout << d_iu << endl;
				curr.print(stdout);
				throw "Feasibility check failure: travel time is off!";
			}
		}
		else
		{
			if (prev.arrival != curr.arrival)
			{
				print(stdout);
				prev.print(stdout);
				curr.print(stdout);
				throw "Feasibility check failure: adjacent parking visitations have different arrival!";
			}
		}

		// Curr arrives too late
		if (!curr.feasible())
		{
			print(stdout);
			curr.print(stdout);
			throw "Feasibility check failure: arrival too late!";
		}

		// Travel to next is correct
		if (curr.park.id != next.park.id)
		{
			double d_uj = input.getDistance(curr.park.id, next.park.id);
			if (curr.departure + d_uj - next.arrival > 0.001)
			{
				print(stdout);
				curr.print(stdout);
				cout << d_uj << endl;
				next.print(stdout);
				throw "Feasibility check failure: travel time is off!";
			}
		}
		else
		{
			if (curr.arrival != next.arrival)
			{
				print(stdout);
				curr.print(stdout);
				next.print(stdout);
				throw "Feasibility check failure: adjacent parking visitations have different arrival!";
			}
		}
	}

	double d_uj = input.getDistance(visits[visits.size() - 2].park.id, visits[visits.size() - 1].park.id);
	distance += d_uj;

	if (newLoad > capacity)
	{
		throw std::runtime_error("Feasibility check failure: exceeded capacity!");
	}

	if (abs(distance - route::distance) > 0.001)
	{
		throw std::runtime_error("Feasibility check failure: distance wrong!");
	}

	if (abs(botDistance - route::botDistance) > 0.001)
	{
		throw std::runtime_error("Feasibility check failure: distance wrong!");
	}

	return true;
}
