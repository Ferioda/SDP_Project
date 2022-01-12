#pragma once
#include "graph.h"
#include <ctime>

vector<int> temp_c;

mutex _m;
mutex _c;

// Smallest-Degree-Last Algorithm implementation 
void weighting_phase(Graph& G, int num_threads);
bool check_degree(vector<int>& degrees, int k);
void update_degrees(vector<int>& degrees, Graph& G);
void coloring_phase(Graph& G, int num_threads);
void search_sdl(int thread_idx, Graph* G, set<int>* U, int from, int to, vector<int>& rand_weights);

clock_t smallest_degree_last(Graph& G, int num_threads) {
	const clock_t begin_time = clock();

	weighting_phase(G, num_threads);
	coloring_phase(G, num_threads);
	
	const clock_t end_time = clock();
	return end_time - begin_time;
}

void parallel_weighting(int k, int i, set<int>& U, int from, int to, Graph& G, vector<int>& degrees) {
	//degrees contiene i gradi iniziali 

	for (int j = from; j < to; j++) {
		int v = *next(U.begin(), j);
		if (degrees[v] <= k) {
			G.weights[v] = i;
			unique_lock<mutex> lk{ _m };
			temp_c.push_back(v);
		}
	}
}

void weighting_phase(Graph& G, int num_threads) {
	set<int> U;
	int k = 1;
	int d = 1;
	vector<int> degrees(G.V);
	G.weights = vector<int>(G.V);
	for (int i = 0; i < G.V; i++) {
		U.insert(i);
		degrees[i] = G.adj[i].size();
		G.weights[i] = -1;
	}

	while (U.size() > 0) {

		while (check_degree(degrees, k)) {

			RangeSplitter rs(U.size(), num_threads > U.size() ? U.size() : num_threads);
			vector<thread> threads;

			for (int idx = 0; idx < num_threads && idx < U.size(); idx++) {

				threads.emplace_back(thread([rs, idx, &G, &U, k, d, &degrees] {

					int from = rs.get_min(idx);
					int to = rs.get_max(idx);
					parallel_weighting(k, d, U, from, to, G, degrees);
					}));
			}
			for (auto& t : threads)
				t.join();

			update_degrees(degrees, G);

			d++;

			for (vector<int>::iterator a = temp_c.begin(); a != temp_c.end(); a++) {
				U.erase(*a);
			}

			// Clear the vector of vertices to color in this iteration
			temp_c.erase(temp_c.begin(), temp_c.end());
		}
		k++;

	}

}

bool check_degree(vector<int>& degrees, int k) {
	for (int i = 0; i < degrees.size(); i++)
		if (degrees[i] <= k && degrees[i] > -1)
			return true;
	return false;
}

void update_degrees(vector<int>& degrees, Graph& G) {
	for (int v : temp_c) {
		// remove v from the set of considered vertices, that is, set the degree of the vertex to -1
		degrees[v] = -1;
		for (int neighbor : G.adj[v])
			degrees[neighbor]--;
	}
}

void coloring_phase(Graph& G, int num_threads) {
	set<int> U;
	vector<int> rand_weights;

	//initialization
	srand(static_cast <unsigned int> (time(0)));
	for (int i = 0; i < G.V; i++) {
		int compGuess = rand() % 100 + 1;
		rand_weights.push_back(compGuess);
		G.colored.push_back(-1);
		U.insert(i);
	}

	while (U.size() > 0) {
		RangeSplitter rs(U.size(), num_threads);
		// run num_threads threads to color the subset of vertices 
		{
			vector<thread> threads;
			for (int idx = 0; idx < num_threads && idx < U.size(); idx++) {
				threads.emplace_back(thread([idx, &U, rs, &G, &rand_weights] {
					int from = rs.get_min(idx);
					int to = rs.get_max(idx);
					search_sdl(idx, &G, &U, from, to, rand_weights);
					}));
			}
			for (auto& t : threads)
				t.join();


			// Remove from the set U the vertices to color in the current iteration 
			for (vector<int>::iterator a = temp_c.begin(); a != temp_c.end(); a++) {
				U.erase(*a);
			}

			// Clear the vector of vertices to color in this iteration
			temp_c.erase(temp_c.begin(), temp_c.end());

		}
	}
}
void search_sdl(int thread_idx, Graph* G, set<int>* U, int from, int to, vector<int>& rand_weights) {
	for (int k = from; k < to; k++) {
		int v = *next(U->begin(), k);
		int d = G->weight(v);
		int w = rand_weights[v];
		bool max = true;

		for (int j = 0; j < G->adj[v].size() && max == true; j++) {
			int neighbor = G->adj[v][j];

			if (U->find(neighbor) != U->end()) {
				// Look at neighboors to know if there is an adjacient vertex with higher degree
				// If there is a conflict, choose the vertex with the highest weight 
				if (G->weight(neighbor) > d || (G->weight(neighbor) == d && rand_weights[neighbor] > w))
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
					unique_lock<mutex> lock{ _c };
					G->colored[v] = j;
					break;
				}
			}
			unique_lock<mutex> lock{ _m };
			temp_c.push_back(v);
		}
	}
}