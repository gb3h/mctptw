#ifndef VISIT_H 
#define VISIT_H 
#include "customer.h"

class visit{
public:
	visit(const customer &cust, int arrival);

	int get_next_push_forward(int pushForward); // return next PF 
	bool check_push_forward_feasiblity(int pushForward); // return true if PF is still feasible for delivery
	int push_forward(int pushForward); // updates current visit with PF and returns next PF

	customer cust;
	int arrival, departure;
	bool feasible;
};

#endif