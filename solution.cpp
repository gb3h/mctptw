#include <algorithm>

#include "solution.h"

using namespace std;

void solution::print(FILE *fp) const {
	fprintf(fp, "[%d routes, distance = %.3f, unbalance = %d, %s]\n",
		routes.size(), totalDistance, unbalancedCapacity, (feasible) ? "feasible" : "infeasible");
	
	for(list<route>::const_iterator it = routes.begin(); it != routes.end(); ++it){
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
void solution::solomon(const problem& input, double mu, double lambda, double alpha1){
	clear();

	set<customer> unrouted;
	for(int id = 1; id <= input.getNumCusto(); id++) unrouted.insert(id);
	while (!unrouted.empty()) {
		customer bestUnroutedCustomer;
		double bestFitness;
		route bestRoute;
		int bestPositionOnRoute;

		for (auto currCustomer: unrouted) {
			for (auto route: routes) {
				for (int prev = 0; i < route.size(); i++) {
				


				}
			}
		}
		// insert bestUnroutedCustomer in respective pos


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