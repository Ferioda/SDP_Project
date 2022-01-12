#pragma once
#include "graph.h"
#include <ctime>
//Jones Plassman Algorithm implementation 
void search_max(int thread_idx, set<int>* U, Graph* G, int from, int to);

vector<int> temp;

clock_t jonesPlassman(Graph& G, int num_threads) {
	// compute time 
	

	set<int> U;
	
	//initialization
	
	srand(static_cast <unsigned int> (time(0)));
	for (int i = 0; i < G.V; i++) {
		int compGuess = rand() % 100 + 1;
		G.weights[i]=compGuess;
		G.colored[i] = -1;
	}
	
	//U subset of V, intially U = V
	for (int i = 0; i < G.V; i++)
		U.insert(i);


	const clock_t begin_time = clock();
	while (U.size() > 0) {
		RangeSplitter rs(U.size(), num_threads);
		// run num_threads threads to color the subset of vertices 
		{
			vector<thread> threads;
			for (int idx = 0; idx < num_threads && idx < U.size(); idx++) {
				threads.emplace_back(thread([idx, &U, rs, &G] {
					int from = rs.get_min(idx);
					int to = rs.get_max(idx);
					search_max(idx, &U, &G, from, to);
					}));
			}
			for (auto& t : threads)
				t.join();


			// Remove from the set U the vertices to color in the current iteration 
			for (vector<int>::iterator a = temp.begin(); a != temp.end(); a++) {
				U.erase(*a);
			}

			// Clear the vector of vertices to color in this iteration
			temp.erase(temp.begin(), temp.end());

		}
	}

	const clock_t end_time = clock();
	return end_time - begin_time;
}
//mutexes used in search_max and search_ldf
mutex m;
mutex c;

void search_max(int thread_idx, set<int>* U, Graph* G, int from, int to) {


	for (int k = from; k < to; k++) {
		int v = *next(U->begin(), k);
		int w = G->weights[v];
		bool max = true;

		for (int j = 0; j < G->adj[v].size() && max == true; j++) {
			int neighbor = G->adj[v][j];

			if (U->find(neighbor) != U->end()) {
				// Look at neighboors to know if there is an adjacient higher vertex
				// If there is a conflict, choose the vertex with the highest index
				if (G->weights[neighbor] > w || (G->weights[neighbor] == w && neighbor > v))
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
					unique_lock<mutex> lock{ c };
					G->colored[v] = j;
					break;
				}
			}
			unique_lock<mutex> lock{ m };
			temp.push_back(v);
		}
	}
}