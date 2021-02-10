#include <algorithm>
#include <iostream>
#include <set>
#include "solution.h"

using namespace std;



void solution::print(FILE *fp) const {
	fprintf(fp, "[%d routes, distance = %.3f]\n",
		routes.size(), totalDistance);
	
	for(vector<route>::const_iterator it = routes.begin(); it != routes.end(); ++it){
		it->print(fp);
	}
}

void solution::clear(){
	routes.clear();
	totalDistance = totalWaiting = unbalancedCapacity = exceededCapacity = 0;
}

void solution::random(const problem& input){
}

// Solomon's I1 insertion heuristic (1987)
// Ref.: "Algorithms for the Vehicle Routing and Scheduling Problems with Time Window Constraints"
void solution::solomon(const problem& input, double mu, double lambda, double alpha_1){
	clear();

	set<int> unrouted;
	for(int id = 1; id <= input.getNumCusto(); id++) unrouted.insert(id);

	route initialRoute = *new route();
	customer depot = input[0];
	visit depotStop = visit(depot, 0);
	initialRoute.visits.push_back(depotStop);
	initialRoute.visits.push_back(depotStop);

	initialRoute.capacity = input.getCapacity();

	vector<route*> newRoutes;
	newRoutes.push_back(&initialRoute);

	while (!unrouted.empty()) {
		int bestUnroutedCustomer = 0;
		double bestFitness = 0;
		route *bestRoute = &initialRoute;
		int bestPositionOnRoute = -1;
		bool hasUpdated = false;

		for (auto currCustomer: unrouted) {
			for (auto route: newRoutes) {
				for (int prev = 0; prev < route->visits.size() - 1; prev++) {
					bool isFeasible = route->check_push_forward(prev, input[currCustomer], input);
					if (isFeasible) {
						double fitness = route->get_fitness(prev, input[currCustomer], input, mu, lambda, alpha_1);
						if ((hasUpdated == false) || (fitness < bestFitness)) {
							hasUpdated = true;
							bestFitness = fitness;
							bestRoute = route;
							bestPositionOnRoute = prev;
							bestUnroutedCustomer = currCustomer;
						}
					}
				}
			}
		}
		if (hasUpdated) {
			bool success = bestRoute->set_push_forward(bestPositionOnRoute, input[bestUnroutedCustomer], input);
			if (!success) {
				cout << "FAILURE ================================" << endl;
			}
			unrouted.erase(bestUnroutedCustomer);
		} else {
			route* newRoute = new route();
			newRoute->visits.push_back(depotStop);
			newRoute->visits.push_back(depotStop);
			newRoute->capacity = input.getCapacity();
			newRoutes.push_back(newRoute);
		}
	}

	for (auto route: newRoutes) {
		routes.push_back(*route);
		totalDistance += route->distance;
	}
}
