#include <cstdio>
#include <ctime>
#include "solution.h"
#include "problem.h"

using namespace std;

int main(int argc, char *argv[]){
	// commands: ./MCTPTW.out inputFileName optional: mu lambda alpha1
	if (argc < 2){
		puts("Unknown commands.");
		return 1;
	}

	double mu = 1, lambda = 2, alpha1 = 0.5;
	if (argc > 4) {
		mu = strtod(argv[2], NULL);
		lambda = strtod(argv[3], NULL);
		alpha1 = strtod(argv[4], NULL);
	}

	clock_t start = clock();

	problem input;
	if (!input.load(argv[1])){
		puts("Unable to open input file!");
		return 1;
	}

	solution solu;
	solu.solomon(input, mu, lambda, alpha1);
	solu.print(stdout);

	clock_t end = clock();

	for (auto route: solu.routes) {
		route.check_feasibility(input);
	}

	printf("%.3f second\n", ((double)(end)-start) / CLOCKS_PER_SEC);
	return 0;
}