#include <algorithm>
#include <iostream>
#include <set>
#include "solution.h"

using namespace std;

void solution::print(FILE *fp) const
{
	fprintf(fp, "%ld routes\n%.3f distance\n",
			routes.size(), totalDistance);

	// fprintf(fp, "[%ld routes, distance = %.3f]\n",
	// 		routes.size(), totalDistance);

	// for(vector<route>::const_iterator it = routes.begin(); it != routes.end(); ++it){
	// 	it->print(fp);
	// }
}

void solution::clear()
{
	routes.clear();
	totalDistance = totalWaiting = unbalancedCapacity = exceededCapacity = 0;
}

// Solomon's I1 insertion heuristic (1987)
// Ref.: "Algorithms for the Vehicle Routing and Scheduling Problems with Time Window Constraints"
void solution::solomon(const problem &input, int insertionCriteria, double mu, double lambda, double alpha_1)
{
	clear();

	set<int> unrouted;
	for (int id = 1; id <= input.getNumCusto(); id++)
		unrouted.insert(id);

	// Initialise first route with start and end of route as depots
	routes.emplace_back();
	customer depot = input[0];
	visit depotStop = visit(depot, 0);
	routes[0].visits.push_back(depotStop);
	routes[0].visits.push_back(depotStop);
	routes[0].capacity = input.getCapacity();

	while (!unrouted.empty())
	{
		int bestUnroutedCustomer = 0;
		double bestFitness = 0;
		route *bestRoute;
		int bestPositionOnRoute = -1;
		bool hasUpdated = false;

		for (auto currCustomer : unrouted)
		{
			for (int i = 0; i < routes.size(); i++)
			{
				for (int prev = 0; prev < routes[i].visits.size() - 1; prev++)
				{
					bool isFeasible = routes[i].check_push_forward(prev, input[currCustomer], input);
					if (isFeasible)
					{
						double fitness = routes[i].get_fitness(prev, input[currCustomer], input, mu, lambda, alpha_1);
						if ((hasUpdated == false) || (fitness < bestFitness))
						{
							hasUpdated = true;
							bestFitness = fitness;
							bestRoute = &routes[i];
							bestPositionOnRoute = prev;
							bestUnroutedCustomer = currCustomer;
						}
					}
				}
			}
		}
		if (hasUpdated)
		{
			if (!bestRoute->set_push_forward(bestPositionOnRoute, input[bestUnroutedCustomer], input))
			{
				// Should return true after successful insertion
				throw "Insertion failure";
			}
			unrouted.erase(bestUnroutedCustomer);
		}
		else
		{
			routes.emplace_back();
			routes[routes.size() - 1].visits.push_back(depotStop);
			routes[routes.size() - 1].visits.push_back(depotStop);
			routes[routes.size() - 1].capacity = input.getCapacity();

			int bestCustomer = -1;
			double bestCustomerFitness = 0;
			bool set = false;

			if (insertionCriteria == 1)
			{
				for (auto currCustomer : unrouted)
				{
					// farthest customer
					double currDist = input.getDistance(0, currCustomer);
					if ((!set) || currDist > bestCustomerFitness)
					{
						set = true;
						bestCustomerFitness = currDist;
						bestCustomer = currCustomer;
					}
				}
			}
			else if (insertionCriteria == 2)
			{
				for (auto currCustomer : unrouted)
				{
					// purely b_ju - b_j
					double currFitness = routes[routes.size() - 1].get_fitness(0, input[currCustomer], input, 1, 0, 0);
					if ((!set) || currFitness < bestCustomerFitness)
					{
						set = true;
						bestCustomerFitness = currFitness;
						bestCustomer = currCustomer;
					}
				}
			}

			if (set)
			{
				vector<visit>::iterator it = routes[routes.size() - 1].visits.begin();
				routes[routes.size() - 1].set_push_forward(0, input[bestCustomer], input);
				unrouted.erase(bestCustomer);
			}
		}
	}

	for (auto route : routes)
	{
		totalDistance += route.distance;
	}
}
