#include <cstdio>
#include <ctime>
// #include <cmath>
// #include <cstdlib>
// #include <vector>
// #include <list>
// #include <algorithm>
#include "solution.h"
#include "problem.h"

using namespace std;

int main(int argc, char *argv[]){
	// commands: ./MCTPTW.out inputFileName
	if (argc < 2){
		puts("Unknown commands.");
		return 1;
	}

	clock_t start = clock();

	problem input;
	if (!input.load(argv[1])){
		puts("Unable to open input file!");
		return 1;
	}

	solution solu;
	solu.solomon(input);
	solu.print(stdout);

	clock_t end = clock();

	printf("%.3f second\n", ((double)(end)-start) / CLOCKS_PER_SEC);
	return 0;
}