#ifndef PROBLEM_H
#define PROBLEM_H

#include <vector>
#include <map>
#include "customer.h"

class problem
{
public:
	bool load(const char *filename);
	int getCapacity() const;
	int getNumCustomer() const;
	int getNumParking() const;
	double getDistance(int id1, int id2) const;
	const std::vector<int> &getCovering(int id) const;
	const customer &operator[](int id) const;
	int calMinRoute() const;

private:
	void calDistances();
	void calCovering();

	std::vector<customer> allCustomer;
	std::vector<std::vector<double>> distance;
	std::map<int, std::vector<int>> covering;
	int capacity, maxCoverDist;
	int numParking, numCustomer;
	bool shortHorizon;
};

#endif