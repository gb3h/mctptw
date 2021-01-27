#ifndef VISIT_H 
#define VISIT_H 
#include "customer.h"

class visit{
public:
	visit(customer &cust, int arrival);
	int get_push_forward_new_departure(int newArrival); // return new departure time given new arrival time (does not update state)
	bool check_push_forward_feasiblity(int newArrival); // return new departure time given new arrival time (does not update state)
	bool push_forward(int newArrival); // returns feasiblity of the updated push forward, and updates

	customer cust;
	int arrival, departure;
	bool feasible;
};

#endif