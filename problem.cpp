#include <cstdio>
#include <cmath>
#include "problem.h"

void problem::printCovering(FILE *fp) const
{
	for (unsigned int i = 1; i <= numCustomer; i++)
	{
		fprintf(fp, "Customer %d covered by parking spots:", i + numParking);
		for (unsigned int j = 0; j < covering[i].size(); j++)
		{
			fprintf(fp, " %d", covering[i][j]);
		}
		fprintf(fp, "\n");
	}
}

void problem::printCoverage(FILE *fp) const
{
	for (unsigned int i = 1; i <= numParking; i++)
	{
		fprintf(fp, "Parking spot %d covers cusomers:", i);
		for (unsigned int j = 0; j < coverage[i].size(); j++)
		{
			fprintf(fp, " %d", coverage[i][j]);
		}
		fprintf(fp, "\n");
	}
}

bool problem::load(const char *filename)
{
	FILE *fp = fopen(filename, "r");

	if (fp == NULL)
		return false;

	if (fscanf(fp, "%d", &capacity) != 1)
	{
		fclose(fp);
		return false;
	}

	if (fscanf(fp, "%lf", &maxCoverDist) != 1)
	{
		fclose(fp);
		return false;
	}

	int numParking;
	if (fscanf(fp, "%d", &numParking) != 1)
	{
		fclose(fp);
		return false;
	}
	int numCustomer;
	if (fscanf(fp, "%d", &numCustomer) != 1)
	{
		fclose(fp);
		return false;
	}

	problem::numParking = numParking;
	problem::numCustomer = numCustomer;
	allCustomer.reserve(numParking + numCustomer + 1); // with depot

	int totalDemand = 0;
	for (int i = 0; i <= numParking + numCustomer; i++)
	{
		double x, y;
		int d, s, e, u;

		if (fscanf(fp, "%*d%lf%lf%d%d%d%d", &x, &y, &d, &s, &e, &u) != 6)
		{
			fclose(fp);
			return false;
		}

		totalDemand += d;
		customer newCustomer(i, x, y, d, s, e, u);
		allCustomer.push_back(newCustomer);
	}
	fclose(fp);

	calDistances();

	calCovering();

	double avgCusPerVehicle = capacity / (totalDemand / numCustomer);
	shortHorizon = (calMinRoute() > avgCusPerVehicle);

	return true;
}

int problem::getCapacity() const
{
	return capacity;
}

int problem::getNumCustomer() const
{
	return problem::numCustomer; // ignore depot
}

int problem::getNumParking() const
{
	return problem::numParking; // ignore depot
}

double problem::getDistance(int id1, int id2) const
{
	return distance[id1][id2];
}

const std::vector<int> &problem::getCovering(int id) const
{
	return covering[id - numParking];
}

const std::vector<int> &problem::getCoverage(int id) const
{
	return coverage[id];
}

const customer &problem::operator[](int id) const
{
	return allCustomer[id];
}

// minimum # of route = ceil(total demand / vehicle's capacity);
int problem::calMinRoute() const
{
	int totalDemand = getCapacity() - 1; // ceiling
	for (int i = 1; i <= getNumCustomer(); ++i)
		totalDemand += allCustomer[i].demand;
	return totalDemand / getCapacity();
}

double hypot(double x, double y)
{
	return sqrt((x * x) + (y * y));
}

void problem::calDistances()
{
	distance.resize(allCustomer.size());

	for (unsigned int i = 0; i < allCustomer.size(); ++i)
	{
		distance[i].resize(allCustomer.size());
	}

	for (unsigned int x = 0; x < allCustomer.size(); ++x)
	{
		for (unsigned int y = x + 1; y < allCustomer.size(); ++y)
		{
			double ans = hypot(allCustomer[x].x_pos - allCustomer[y].x_pos,
							   allCustomer[x].y_pos - allCustomer[y].y_pos);
			distance[x][y] = distance[y][x] = ans;
		}
	}
}

void problem::calCovering()
{
	covering.resize(numCustomer + 1);
	coverage.resize(numParking + 1);
	for (unsigned int i = 1; i <= numCustomer; i++)
	{
		for (unsigned int j = 1; j <= numParking; j++)
		{
			if (distance[i + numParking][j] > maxCoverDist)
			{
				continue;
			}
			covering[i].push_back(j);
			coverage[j].push_back(i + numParking);
		}
	}
}
