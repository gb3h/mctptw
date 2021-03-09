#include <cstdio>
#include <ctime>
#include <iostream>
#include "solution.h"
#include "problem.h"

using namespace std;

int main(int argc, char *argv[])
{
	// commands: ./MCTPTW.out inputFileName optional: insertionCriteria mu lambda alpha1
	if (argc < 2)
	{
		puts("Unknown commands.");
		return 1;
	}

	double mu = 1, lambda = 2, alpha1 = 0.5;
	int insertionCriteria = 0;
	if (argc > 5)
	{
		insertionCriteria = atoi(argv[2]);
		mu = strtod(argv[3], NULL);
		lambda = strtod(argv[4], NULL);
		alpha1 = strtod(argv[5], NULL);
		// printf("Insertion Criteria: %d, mu: %f, lambda: %f, alpha1: %f\n", insertionCriteria, mu, lambda, alpha1);
	}

	clock_t start = clock();

	problem input;
	if (!input.load(argv[1]))
	{
		puts("Unable to open input file!");
		return 1;
	}

	solution solu;
	solu.solomon(input, insertionCriteria, mu, lambda, alpha1);
	// solu.brute_force(input);
	// solu.print(stdout);

	clock_t end = clock();

	for (auto route : solu.routes)
	{
		route.check_feasibility(input);
	}

	solu.clear();

	printf("%.3f second\n", ((double)(end)-start) / CLOCKS_PER_SEC);
	return 0;
}