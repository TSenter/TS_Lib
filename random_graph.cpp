#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#include <iostream>
#include <string>
#include <vector>
#include <set>
#include <algorithm>

using namespace std;

#define ARG "      %-10s  %s\n"

// Helper functions {{{1
void usage(char **argv, int argc, FILE *out = stdout, bool kill = false) {
	fprintf(out, "Usage: %s -N=#\n", argv[0]);
	fprintf(out, ARG, "-N=#", "Number of vertices");

	fprintf(out, "\n  Options:\n");
	fprintf(out, ARG, "-a", "Force graph to contain at no cycles [must have -D]");
	fprintf(out, ARG, "-c", "Force graph to contain at least 1 cycle [must have -D]");
	fprintf(out, ARG, "-D", "Make graph directed");
	fprintf(out, ARG, "-e=#", "Minimum number of edges per vertex (default to 0)");
	fprintf(out, ARG, "-E=#", "Maximum number of edges per vertex (default to N-1)");
	fprintf(out, ARG, "-seed=#", "Seed the random generator");
	fprintf(out, ARG, "-stdin", "Reads vertex labels in from stdin");
	fprintf(out, ARG, "-w=#", "Minimum weight between two vertices");
	fprintf(out, ARG, "-W=#", "Maximum weight between two vertices");
	fprintf(out, ARG, "-v", "Print verbose output");

	fprintf(out, "\n  Output:\n");
	fprintf(out, "      First line is number of vertices.\n");
	fprintf(out, "      Next N lines are vertex labels.\n");
	fprintf(out, "      Remainder of lines are in the following format:\n");
	fprintf(out, "         v0 v1 [w]     // [w] is the weight between the vertices\n");
	fprintf(out, "      Note: The output will differ if -v is passed.\n");

	fflush(out);

	if (kill) exit(0);
}

bool is_number(char *str) {
	int len = strlen(str);

	for (int i = 0; i < len; i++)
		if (str[i] > 57 || str[i] < 48) return false;

	return true;
}

void error(string str, int kill = 1) {
	fprintf(stderr, "Error: %s\n", str.c_str());

	if (kill) exit(kill);
}
// 1}}}
// Usage message {{{1
// Required:
//   -N=#		(number of vertices)
//
// Optional:
//   -e=#		(min number of edges per vertex)
//   -E=#		(max number of edges per vertex)
//   -seed=#	(seed for srand [time(NULL) if not present)
//   -sorted	(sorts edges if present)
//   -D			(graph becomes directed [undirected by default])
//   -c			(force at least one cycle [must include -D])
//   -a			(force graph to be acyclic)
//   -w=#		(minimum weight between two vertices)
//   -W=#		(maximum weight between two vertices)
//   -v			(verbose output)
//   -stdin		(read vertices from stdin)
//   1}}}
// Driver {{{1
int main(int argc, char **argv) {
	// Initial setup {{{2
	vector<string> V;
	vector<int> ecount;
	string str;

	long N = -1;

	bool verbose, D, weighted, in;
	long seed;
	int e, E, w, W, c;

	verbose = D = weighted = in = false;
	e = E = w = W = c = seed = -1;

	// Parse command-line arguments {{{2
	string err;
	for (int i = 1; i < argc; i++) {
		if (!strcmp(argv[i], "--help")) usage(argv, argc, stdout, true);

		if (strstr(argv[i], "-seed=") == argv[i]) {
			if (is_number(argv[i] + 6))
				seed = atoi(argv[i] + 6);
			else
				err = "The seed must be a positive integer!";
		} else if (strstr(argv[i], "-N=") == argv[i]) {
			if (is_number(argv[i] + 3))
				N = atoi(argv[i] + 3);
			else
				err = "The number of vertices must be a positive integer!";
		} else if (strstr(argv[i], "-e=") == argv[i]) {
			if (is_number(argv[i] + 3))
				e = atoi(argv[i] + 3);
			else
				err = "The minimum number of edges must be a positive integer!";
		} else if (strstr(argv[i], "-E=") == argv[i]) {
			if (is_number(argv[i] + 3))
				E = atoi(argv[i] + 3);
			else
				err = "The maximum number of edges must be a positive integer!";
		} else if (!strcmp(argv[i], "-D"))				D = true;
		  else if (strstr(argv[i], "-w=") == argv[i]) {
			if (is_number(argv[i] + 3))
				w = atoi(argv[i] + 3);
			else
				err = "The minimum weight for an edge must be a positive integer!";
		} else if (strstr(argv[i], "-W=") == argv[i]) {
			if (is_number(argv[i] + 3))
				W = atoi(argv[i] + 3);
			else
				err = "The maximum weight for an edge must be a positive integer!";
		} else if (!strcmp(argv[i], "-v"))				verbose = true;
		  else if (!strcmp(argv[i], "-a"))				c = 0;
		  else if (!strcmp(argv[i], "-c"))				c = 1;
		  else if (!strcmp(argv[i], "-stdin"))			in = 1;
		else error(string("Invalid option: ") + argv[i]);
	}
	if (err != "") error(err);

	// 2}}}
	// Validate arguments and setup {{{2
	if (!D) {
		if (c == 1) error("Graph must include option \"-D\" to be cyclic");
		if (c == 0) error("Graph must include option \"-D\" to be acyclic");
	}

	if (e < -1 && E < -1 || E < e) error("Invalid min/max number of edges");
	

	if (in) {
		while (getline(cin, str))
			V.push_back(str);

		if (V.size() == 0) error("You must have at least 1 vertex");
	}

	if (N == -1 && V.size()) N = V.size();

	if (N != -1 && V.size() > N)
		V.resize(N);
	
	if (N < 1) error("Invalid number of vertices");

	if (e == -1) e = 0;
	if (E == -1) E = N - 1;

	char tmp[33];
	if (!V.size())
		for (int i = 0; i < N; i++) {
			sprintf(tmp, "%d", i);
			V.push_back(string(tmp));
		}

	if (w != -1 || W != -1) weighted = true;

	if (weighted && W == -1) W = w;
	if (weighted && w == -1) w = 0;

	if (seed == -1) seed = time(NULL);

	vector< set<int> > edges;
	vector< vector<int> > weights;

	srand(seed);
	// 2}}}
	// Print verbose stuff {{{2
	if (verbose) {
		printf("Options: \n");
		printf("   Vertices:      %d\n", N);
		printf("   Seed:          %d\n", seed);
		printf("   Edges (min):   %d\n   Edges (max):   %d\n", e, E);
		if (weighted)
			printf("   Weights (min): %d\n   Weights (max): %d\n", w, W);
		printf("   Type:          %s\n", D ? "Directed" : "Undirected");

		printf("------------------------------\n");
	}
	// 2}}}
	// Print vertex labels {{{2
	int length = 0;
	printf("%d\n", N);
	for (int i = 0; i < N; i++) {
		if (V[i].size() > length) length = V[i].size();
		printf("%s\n", V[i].c_str());
	}
	
	length++;
	// 2}}}
	// Generate graph {{{2
	edges.assign(N, set<int>());
	weights.assign(N, vector<int>(N, w-1));
	ecount.assign(N, 0);

	bool b = false;
	int num, r, weight, ecs = N;
	for (int i = 0; i < N && !b; i++) {
		num = e + (rand() % (E - e + 1));
		while (num--) {
			if (ecs == 0) {
				error("[Severe] All vertices are at max edge capacity, but not all vertices are at min edge capacity.");
				return -1;
			}

			if (ecount[i] >= E) break;

			if (!ecount.size()) break;

			r = rand() % N;

			if (ecount[r] >= E) { num++; continue; }

			if (i == r) { num++; continue; }
			if (edges[i].find(r) != edges[i].end()) { num++; continue; }

			if (weighted && weights[i][r] < w) {
				weight = w + rand() % (W - w + 1);
				weights[i][r] = weight;
				weights[r][i] = weight;
			}

			edges[i].insert(r);
			ecount[i]++;
			ecount[r]++;

			if (ecount[i] == E) ecs--; 
			if (ecount[r] == E) ecs--; 
		}

		for (int j = E; j < edges[i].size(); j++) {
			set<int>::iterator k = (--edges[i].end());
			edges[i].erase(*k);
			edges[*k].erase(i);
		}

	}

	for (int i = 0; i < N; i++) {
		set<int>::iterator i0 = edges[i].begin(), i1 = edges[i].end();
		for (; i0 != i1; ++i0) {
			if (weighted)
				printf("%-*s %-*s %d\n", length, V[i].c_str(), length, V[*i0].c_str(), weights[i][*i0]);
			else
				printf("%-*s %-*s\n", length, V[i].c_str(), length, V[*i0].c_str());
		}
	}
	// 2}}}
	return 0;
}
// 1}}}
