#include <algorithm>
#include <iostream>
#include <set>
#include <numeric>
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
	generate_partitions(4);
	// std::vector<std::vector<int>> set_of_sets = generate_partitions(5);
	cout << partitions.size() << endl;
	for (int i = 0; i < partitions.size(); i++)
	{
		for (int j = 0; j < partitions[i].size(); j++)
		{
			cout << partitions[i][j] << " ";
		}
		cout << endl;
	}

	std::vector<int> arr = {3, 4, 5, 6};
	perm_generator(arr, 2);
}

void solution::perm_generator(std::vector<int> &arr, int k)
{
	cout << "These are the Possible Permutations: " << endl;
	do
	{
		for (int i = 0; i < k; i++)
		{
			cout << arr[i] << " ";
		}
		cout << endl;
		std::reverse(arr.begin() + k, arr.end());
	} while (next_permutation(arr.begin(), arr.end()));
}

void solution::generate_partitions(int n)
{
	std::vector<int> p(n);
	int k = 0;
	p[k] = n;

	// Loop until all the array elements converted to 1 mean no further partition can be generated.
	while (1)
	{
		std::vector<int> curr(p);
		partitions.push_back(curr);
		// printArray(p, k + 1);
		int rem_val = 0;

		// Move the pointer to the index so that p[k] > 1.
		while (k >= 0 && p[k] == 1)
		{
			rem_val += p[k];
			k--;
		}
		// If k < 0 then the all the element are broken down to 1.
		if (k < 0)
			return;

		// If value greater than 1 is found then decrease it by 1 and increase rem_val to add it to other elements.
		p[k]--;
		rem_val++;

		// Loop until the number of 1's are greater than the value at k index.
		while (rem_val > p[k])
		{
			p[k + 1] = p[k];
			// Decrease the rem_val value.
			rem_val = rem_val - p[k];
			k++;
		}

		// Assign the remaining value to the index next to k.
		p[k + 1] = rem_val;
		k++;
	}
}
