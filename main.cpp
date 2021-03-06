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

	double lambda = 1, alpha1 = 0;
	if (argc > 3)
	{
		lambda = strtod(argv[2], NULL);
		alpha1 = strtod(argv[3], NULL);
	}

	clock_t start = clock();

	problem input;
	if (!input.load(argv[1]))
	{
		puts("Unable to open input file!");
		return 1;
	}

	solution solu;
	solu.solomon(input, lambda, alpha1);

	clock_t end = clock();
	printf("%.3f second\n", ((double)(end)-start) / CLOCKS_PER_SEC);

	solu.print(stdout);

	for (auto route : solu.routes)
	{
		route.print(stdout);
		route.check_feasibility(input);
	}

	solu.clear();

	return 0;
}