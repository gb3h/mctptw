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
		fprintf(fp, " %d", visits[i].cust.id);
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
		dist += input.getDistance(visits[i - 1].cust.id, visits[i].cust.id);
	}
	dist += input.getDistance(visits[visits.size() - 1].cust.id, 0);
	return dist;
}

visit route::initialise_insertion(int i_index, const customer &park, const customer &u, const problem &input)
{
	// Previous delivery's parking is the same as this
	visit vis = visit(park, u, visits[i_index].arrival, input.getDistance(park.id, u.id), visits[i_index].departure);

	// Previous delivery's parking is elsewhere
	if (visits[i_index].park.id != park.id)
	{
		double travelTime = input.getDistance(visits[i_index].park.id, park.id);
		double arrival = visits[i_index].departure + travelTime;
		vis = visit(park, u, arrival, input.getDistance(park.id, u.id), 0);
	}
	return vis;
}

double route::compute_new_leave(int index, int park_id, double arrivalPushForward)
{
	double ans = -1;
	for (int i = index; i < visits.size(); i++)
	{
		visit next = visits[i];
		if (next.park.id != park_id)
		{
			break;
		}
		if (!next.check_push_forward_feasiblity(arrivalPushForward))
			return -1;

		next.push_forward(arrivalPushForward);
		ans = fmax(ans, next.getEarliestDeparture());
	}
	return ans;
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

	visit *prev = &visits[i_index];
	visit *curr = &vis;
	visit *next = &visits[i_index + 1];
	double newArrival, newLeave;
	double travelTime;

	// Do work to get the first PushForward value
	next = &visits[i_index + 1];

	// In the case that we are delivering at the same parking spot as the next
	double pushForward = 0;
	newLeave = curr->getEarliestDeparture();
	pushForward = newLeave - curr->departure;

	for (int j = i_index + 1; j < visits.size(); j++)
	{
		// Early terminate PushForward loop if there's enough "slack"
		if (pushForward == 0)
		{
			break;
		}

		next = &visits[j];

		if (next->park.id == curr->park.id)
		{
			if (set)
			{
				next->departure += pushForward;
			}
		}
		else
		{
			if (!next->check_push_forward_feasiblity(pushForward))
				return false;
			if (!set)
			{
				visit copy = *next;
				next = &copy;
			}
			next->push_forward(pushForward);
			double nextPushForward = compute_new_leave(j + 1, next->park.id, pushForward);
			newLeave = fmax(next->getEarliestDeparture(), nextPushForward);
			next->departure = newLeave;
			pushForward = newLeave;
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

double route::get_c1_fitness(int i_index, const customer &park, const customer &u, const problem &input, double mu, double lambda, double alpha_1)
{
	visit prev = visits[i_index];
	visit next = visits[i_index + 1];

	visit vis = initialise_insertion(i_index, park, u, input);
	double d_iu = input.getDistance(prev.cust.id, vis.cust.id);
	double d_uj = input.getDistance(vis.cust.id, next.cust.id);
	double d_ij = input.getDistance(prev.cust.id, next.cust.id);
	double c_11 = d_iu + d_uj - mu * d_ij;

	int travelTime = input.getDistance(next.cust.id, vis.cust.id);
	int newArrival = vis.departure + travelTime;
	int b_j = fmax(next.arrival, next.cust.start);
	int b_ju = fmax(newArrival, next.cust.start);
	double c_12 = b_ju - b_j;

	double alpha_2 = 1 - alpha_1;
	double c_1 = alpha_1 * c_11 + alpha_2 * c_12;
	return c_1;
}

double route::get_c2_fitness(int i_index, const customer &park, const customer &u, const problem &input, double mu, double lambda, double alpha_1)
{
	double c_1 = route::get_c1_fitness(i_index, park, u, input, mu, lambda, alpha_1);
	double d_0u = input.getDistance(0, u.id);
	double c_2 = lambda * d_0u - c_1;
	return c_2;
}

bool route::check_feasibility(const problem &input)
{
	double newLoad = 0;

	for (int i = 1; i < visits.size() - 1; i++)
	{
		visit prev = visits[i - 1];
		visit curr = visits[i];
		visit next = visits[i + 1];

		newLoad += curr.cust.demand;

		// Travel from prev to curr is correct
		double d_iu = input.getDistance(prev.cust.id, curr.cust.id);
		if (prev.departure + d_iu - curr.arrival > 0.001)
		{
			print(stdout);
			prev.print(stdout);
			cout << d_iu << endl;
			curr.print(stdout);
			throw "Feasibility check failure: travel time is off!";
		}

		// Curr arrives after end of time window
		if (curr.arrival > curr.cust.end)
		{
			print(stdout);
			curr.print(stdout);
			throw "Feasibility check failure: arrival too late!";
		}

		// Curr departs once unloading is done
		if (fmax(curr.arrival, curr.cust.start) + curr.cust.unload - curr.departure > 0.001)
		{
			print(stdout);
			curr.print(stdout);
			throw "Feasibility check failure: unloading time is off!";
		}

		// Travel to next is correct
		double d_uj = input.getDistance(curr.cust.id, next.cust.id);
		if (curr.departure + d_uj - next.arrival > 0.001)
		{
			print(stdout);
			curr.print(stdout);
			cout << d_uj << endl;
			next.print(stdout);
			throw "Feasibility check failure: travel time is off!";
		}
	}

	if (newLoad > capacity)
	{
		throw "Feasibility check failure: exceeded capacity!";
	}

	return true;
}
