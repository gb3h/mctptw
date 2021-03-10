#ifndef VISIT_H
#define VISIT_H
#include <cstdio>
#include "customer.h"

class visit
{
public:
	void print(FILE *fp) const;
	visit(const customer &park, const customer &cust, double arrival, double distance, double departure);
	double getEarliestStart();
	double getLatestEnd();
	double getEffectiveServiceTime();
	double getEarliestDeparture();
	bool feasible();

	double get_next_push_forward(double pushForward);		// return next PF
	bool check_push_forward_feasiblity(double pushForward); // return true if PF is still feasible for delivery
	double push_forward(double pushForward);				// updates current visit with PF and returns next PF

	customer cust;
	customer park;
	double arrival, departure, distance;
};

#endif