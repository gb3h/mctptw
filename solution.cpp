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

	initialRoute.capacity = input.getCapacity();

	vector<route*> newRoutes;
	newRoutes.push_back(&initialRoute);

	while (!unrouted.empty()) {
		int bestUnroutedCustomer = 0;
		double bestFitness = -1;
		route *bestRoute = &initialRoute;
		int bestPositionOnRoute = -1;

		for (auto currCustomer: unrouted) {
			for (auto route: newRoutes) {
				for (int prev = 0; prev < route->visits.size(); prev++) {
					bool isFeasible = route->check_push_forward(prev, input[currCustomer], input);
					double fitness = -1;
					if (isFeasible) {
						fitness = route->get_fitness(prev, input[currCustomer], input, mu, lambda, alpha_1);
						if ((bestFitness < 0) || (fitness > 0 && fitness < bestFitness)) {
							bestFitness = fitness;
							bestRoute = route;
							bestPositionOnRoute = prev;
							bestUnroutedCustomer = currCustomer;
						}
					}
				}
			}
		}
		if (bestFitness > 0) {
			bool success = bestRoute->set_push_forward(bestPositionOnRoute, input[bestUnroutedCustomer], input);
			if (!success) {
				cout << "FAILURE ================================" << endl;
			}
			unrouted.erase(bestUnroutedCustomer);
		} else {
			route* newRoute = new route();
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

int solution::cmp(const solution &solA, const solution &solB, const problem &input){
	if(solA.feasible != solB.feasible){
		if(solA.feasible) return -1;
	}else if(solA.feasible){
		if(solA.routes.size() != solB.routes.size() ) return (solA.routes.size() - solB.routes.size() );
		else if(solA.totalDistance < solB.totalDistance) return -1;
		else return 1;
	}
	return 1;
}

bool solution::fdominate(const solution &solA, const solution &solB){
	if( solA.totalDistance <= solB.totalDistance && solA.routes.size() <= solB.routes.size() ){
		return ( solA.totalDistance < solB.totalDistance || solA.routes.size() < solB.routes.size() );
	}else return false;
}

bool solution::operator == (const solution &another) const {
	return (routes.size() == another.routes.size() && 
			unbalancedCapacity == another.unbalancedCapacity &&
            abs(totalDistance - another.totalDistance) < 0.01);
}

bool solution::operator < (const solution &another) const {
	if(routes.size() != another.routes.size() ) return (routes.size() < another.routes.size());
	else return (totalDistance < another.totalDistance);
}