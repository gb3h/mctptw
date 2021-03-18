#ifndef PROBLEM_H
#define PROBLEM_H

#include <vector>
#include <map>
#include "customer.h"

class problem
{
public:
	void printCovering(FILE *fp) const;
	void printCoverage(FILE *fp) const;
	bool load(const char *filename);
	int getCapacity() const;
	int getNumCustomer() const;
	int getNumParking() const;
	double getDistance(int id1, int id2) const;
	const std::vector<int> &getCovering(int id) const;
	const std::vector<int> &getCoverage(int id) const;
	const customer &operator[](int id) const;
	int calMinRoute() const;

private:
	void calDistances();
	void calCovering();

	std::vector<customer> allCustomer;
	std::vector<std::vector<double>> distance;
	std::vector<std::vector<int>> covering; // Vector of parking spaces covering a customer
	std::vector<std::vector<int>> coverage; // Vector of customers covered by a parking spot
	int capacity;
	double maxCoverDist = 0.2;
	int numParking, numCustomer;
	bool shortHorizon;
};

#endif