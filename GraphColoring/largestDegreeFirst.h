#pragma once
#include "graph.h"
#include <ctime>
// Largest-Degree-First Algorithm implementation 
void search_ldf(int thread_idx, Graph* G, set<int>* U, int from, int to);

vector<int> v_t;

mutex m_;
mutex c_;

clock_t largestDegreeFirst(Graph& G, int num_threads) {
	// compute time
	const clock_t begin_time = clock();

	set<int> U;

	//initialization
	srand(static_cast <unsigned int> (time(0)));
	for (int i = 0; i < G.V; i++) {
		int compGuess = rand() % 100 + 1;
		G.weights.push_back(compGuess);
		G.colored.push_back(-1);
	}

	//U subset of V, intially U = V
	for (int i = 0; i < G.V; i++)
		U.insert(i);



	while (U.size() > 0) {
		RangeSplitter rs(U.size(), num_threads);
		// run num_threads threads to color the subset of vertices 
		{
			vector<thread> threads;
			for (int idx = 0; idx < num_threads && idx < U.size(); idx++) {
				threads.emplace_back(thread([idx, &U, rs, &G] {
					int from = rs.get_min(idx);
					int to = rs.get_max(idx);
					search_ldf(idx, &G, &U, from, to);
					}));
			}
			for (auto& t : threads)
				t.join();


			// Remove from the set U the vertices to color in the current iteration 
			for (vector<int>::iterator a = v_t.begin(); a != v_t.end(); a++) {
				U.erase(*a);
			}

			// Clear the vector of vertices to color in this iteration
			v_t.erase(v_t.begin(), v_t.end());

		}
	}

	const clock_t end_time = clock();
	return end_time - begin_time;

}

void search_ldf(int thread_idx, Graph* G, set<int>* U, int from, int to) {


	for (int k = from; k < to; k++) {
		int v = *next(U->begin(), k);
		int w = G->weight(v);
		int d = G->degree_of(v);
		bool max = true;

		for (int j = 0; j < G->adj[v].size() && max == true; j++) {
			int neighbor = G->adj[v][j];

			if (U->find(neighbor) != U->end()) {
				// Look at neighboors to know if there is an adjacient vertex with higher degree
				// If there is a conflict, choose the vertex with the highest weight 
				if (G->degree_of(neighbor) > d || (G->degree_of(neighbor) == d && G->weight(neighbor) > w))
					max = false;
			}

		}

		if (max) {

			// Assign to the max weighted vertices the lowest possible color 
			set <int> S;
			for (int j = 0; j < G->adj[v].size(); j++) {
				if (G->colored[G->adj[v][j]] != -1)
					S.insert(G->colored[G->adj[v][j]]);
			}
			for (int j = 0; j < S.size() + 1; j++) {
				if (S.find(j) == S.end()) {
					unique_lock<mutex> lock{ c_ };
					G->colored[v] = j;
					break;
				}
			}
			unique_lock<mutex> lock{ m_ };
			v_t.push_back(v);
		}
	}
}


