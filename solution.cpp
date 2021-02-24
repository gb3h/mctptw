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

	customer depot = input[0];
	visit depotStop = visit(depot, 0);

	while (!unrouted.empty())
	{
		int bestUnroutedCustomer = 0;
		double bestFitness = 0;
		route *bestRoute;
		int bestPositionOnRoute = -1;
		bool hasUpdated = false;

		for (auto currCustomer : unrouted)
		{
			double currBestFitness = 0;
			int currBestPositionOnRoute = -1;
			bool currHasUpdated = false;

			int i = routes.size() - 1;
			if (i < 0)
				break;

			for (int prev = 0; prev < routes[i].visits.size() - 1; prev++)
			{
				bool isFeasible = routes[i].check_push_forward(prev, input[currCustomer], input);
				if (isFeasible)
				{
					double c1_fitness = routes[i].get_c1_fitness(prev, input[currCustomer], input, mu, lambda, alpha_1);
					if ((currHasUpdated == false) || (c1_fitness < currBestFitness))
					{
						currHasUpdated = true;
						currBestFitness = c1_fitness;
						currBestPositionOnRoute = prev;
					}
				}
			}

			if (currHasUpdated)
			{
				double c2_fitness = routes[i].get_c2_fitness(currBestPositionOnRoute, input[currCustomer], input, mu, lambda, alpha_1);
				if ((hasUpdated == false) || (c2_fitness > bestFitness))
				{
					hasUpdated = true;
					bestFitness = c2_fitness;
					bestRoute = &routes[i];
					bestPositionOnRoute = currBestPositionOnRoute;
					bestUnroutedCustomer = currCustomer;
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
					double currFitness = routes[routes.size() - 1].get_c1_fitness(0, input[currCustomer], input, 1, 0, 0);
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
