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
	for (int id = 1; id <= input.getNumCustomer(); id++)
		unrouted.insert(input.getNumParking() + id);

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

void solution::brute_force(const problem &input)
{
	clear();
	set<int> unrouted;
	for (int id = 1; id <= input.getNumCustomer(); id++)
		unrouted.insert(input.getNumParking() + id);
	vector<int> arr(unrouted);
	vector<vector<int>> partitions = generate_partitions(5);
	vector<vector<int>> perms = perm_generator(arr, 2);
}

vector<vector<int>> solution::perm_generator(vector<int> &arr, int k)
{
	vector<vector<int>> perms;
	do
	{
		vector<int> curr;
		for (int i = 0; i < k; i++)
		{
			curr.push_back(arr[i]);
		}
		perms.push_back(curr);
		reverse(arr.begin() + k, arr.end());
	} while (next_permutation(arr.begin(), arr.end()));
	return perms;
}

vector<vector<int>> solution::generate_partitions(int n)
{
	vector<int> p(1);
	int k = 0;
	p[k] = n;
	vector<vector<int>> partitions;
	while (1)
	{
		vector<int> curr(p);
		partitions.push_back(curr);
		int rem_val = 0;
		while (k >= 0 && p[k] == 1)
		{
			rem_val += p[k];
			k--;
		}
		if (k < 0)
			return partitions;
		p[k]--;
		rem_val++;
		while (rem_val > p[k])
		{
			if ((k + 1) >= p.size())
				p.push_back(0);
			p[k + 1] = p[k];
			rem_val = rem_val - p[k];
			k++;
		}

		if ((k + 1) >= p.size())
			p.push_back(0);
		p[k + 1] = rem_val;
		k++;
	}
	return partitions;
}
