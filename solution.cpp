#include <algorithm>
#include <iostream>
#include <set>
#include <limits>
#include <queue>
#include "solution.h"

using namespace std;

void solution::print(FILE *fp) const
{
	fprintf(fp, "%ld routes\n%.3f distance\n%.3f bot distance\n",
			routes.size(), totalDistance, totalBotDistance);

	// fprintf(fp, "[%ld routes, distance = %.3f]\n",
	// 		routes.size(), totalDistance);

	// for(vector<route>::const_iterator it = routes.begin(); it != routes.end(); ++it){
	// 	it->print(fp);
	// }
}

void solution::clear()
{
	routes.clear();
	totalDistance = totalWaiting = unbalancedCapacity = exceededCapacity = totalBotDistance = 0;
}

void solution::solomon(const problem &input, double lambda, double alpha_1)
{
	clear();

	set<int> unrouted;
	for (int id = 1; id <= input.getNumCustomer(); id++)
		unrouted.insert(input.getNumParking() + id);

	customer depot = input[0];
	visit depotStop = visit(depot, depot, 0, 0, 0);

	while (!unrouted.empty())
	{
		int bestUnroutedCustomer = 0;
		int bestParking = 0;
		double bestFitness = 0;
		route *bestRoute;
		int bestPositionOnRoute = -1;
		bool hasUpdated = false;

		for (auto currCustomer : unrouted)
		{
			double currBestFitness = 0;
			int currParking = 0;
			int currBestPositionOnRoute = -1;
			bool currHasUpdated = false;

			int i = routes.size() - 1;
			if (i < 0)
				break;

			for (int prev = 0; prev < routes[i].visits.size() - 1; prev++)
			{
				vector<int> covering = input.getCovering(currCustomer);

				for (int j = 0; j < covering.size(); j++)
				{
					bool isFeasible = routes[i].check_push_forward(prev, input[covering[j]], input[currCustomer], input);
					if (isFeasible)
					{
						double c11_fitness = routes[i].get_c11_fitness(prev, input[covering[j]], input[currCustomer], input, unrouted);
						double c12_fitness = routes[i].get_c12_fitness(prev, input[covering[j]], input[currCustomer], input);
						// fprintf(stdout, "(%d, %d) route: %d, c1 fitness: %.3f\n", covering[j], currCustomer - 25, i, c1_fitness);
						double c1_fitness = (alpha_1 * -c11_fitness) + ((1 - alpha_1) * c12_fitness);
						// if (c12_fitness == 0)
						// {
						// 	c1_fitness = -std::numeric_limits<double>::infinity();
						// }
						if ((currHasUpdated == false) || (c1_fitness < currBestFitness))
						{
							currHasUpdated = true;
							currBestFitness = c1_fitness;
							currBestPositionOnRoute = prev;
							currParking = covering[j];
						}
					}
				}
			}

			if (currHasUpdated)
			{
				double c2_fitness = routes[i].get_c2_fitness(currBestPositionOnRoute, input[currParking], input[currCustomer], input, lambda, currBestFitness);
				// fprintf(stdout, "(%d, %d) route: %d, c2 fitness: %.3f\n", currParking, currCustomer - 25, i, c2_fitness);
				if ((hasUpdated == false) || (c2_fitness > bestFitness))
				{
					hasUpdated = true;
					bestFitness = c2_fitness;
					bestRoute = &routes[i];
					bestPositionOnRoute = currBestPositionOnRoute;
					bestUnroutedCustomer = currCustomer;
					bestParking = currParking;
				}
			}
		}
		if (hasUpdated)
		{
			// printf("BestPos: %d, (%d, %d)\n", bestPositionOnRoute, bestParking, bestUnroutedCustomer);
			// bestRoute->check_push_forward(bestPositionOnRoute, input[bestParking], input[bestUnroutedCustomer], input);
			if (!bestRoute->set_push_forward(bestPositionOnRoute, input[bestParking], input[bestUnroutedCustomer], input))
			{
				// Should return true after successful insertion
				throw "Insertion failure";
			}
			// bestRoute->print(stdout);
			unrouted.erase(bestUnroutedCustomer);
		}
		else
		{
			routes.emplace_back();
			routes[routes.size() - 1].visits.push_back(depotStop);
			routes[routes.size() - 1].visits.push_back(depotStop);
			routes[routes.size() - 1].capacity = input.getCapacity();
			if (routes.size() > 1 && routes[routes.size() - 2].visits.size() == 2)
			{
				printf("Insertion Failure:");
				for (auto failed : unrouted)
				{
					printf(" %d", failed - input.getNumParking());
				}
				printf("\n");
				throw "Some unrouted infeasible";
			}

			// int bestParking = -1;
			// int bestCustomer = -1;
			// double bestCustomerFitness = 0;
			// bool set = false;

			// for (auto currCustomer : unrouted)
			// {
			// 	vector<int> covering = input.getCovering(currCustomer);
			// 	for (int j = 0; j < covering.size(); j++)
			// 	{
			// 		double currFitness = routes[routes.size() - 1].get_c11_fitness(0, input[covering[j]], input[currCustomer], input, unrouted);
			// 		if ((!set) || currFitness > bestCustomerFitness)
			// 		{
			// 			set = true;
			// 			bestCustomerFitness = currFitness;
			// 			bestCustomer = currCustomer;
			// 			bestParking = covering[j];
			// 		}
			// 	}
			// }

			// if (set)
			// {
			// 	vector<visit>::iterator it = routes[routes.size() - 1].visits.begin();
			// 	routes[routes.size() - 1].set_push_forward(0, input[bestParking], input[bestCustomer], input);
			// 	unrouted.erase(bestCustomer);
			// }
		}
	}

	for (auto route : routes)
	{
		totalDistance += route.distance;
		totalBotDistance += route.botDistance;
	}
}

void solution::brute_force(const problem &input)
{
	clear();

	customer depot = input[0];
	visit depotStop = visit(depot, depot, 0, 0, 0);

	set<int> unrouted;
	vector<int> arr;
	for (int id = 1; id <= input.getNumCustomer(); id++)
	{
		arr.push_back(input.getNumParking() + id);
		unrouted.insert(input.getNumParking() + id);
	}
	vector<vector<int>> partitions = generate_partitions(4);
	vector<int> first_route;
	queue<pair<vector<int>, pair<vector<int>, vector<int>>>> q;
	for (auto partition : partitions)
	{
		pair<vector<int>, pair<vector<int>, vector<int>>> node = make_pair(first_route, make_pair(partition, arr));
		q.push(node);
	}

	while (!q.empty())
	{
	}

	vector<vector<int>> perms = perm_generator(arr, 2);
	// cout << perms.size() << endl;
	// for (int i = 0; i < perms.size(); i++)
	// {
	// 	for (int j = 0; j < perms[i].size(); j++)
	// 	{
	// 		cout << perms[i][j] << " ";
	// 	}
	// 	cout << endl;
	// }
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
