#pragma once
#include "graph.h"
#include <ctime>

// Largest-Degree-First Algorithm implementation 
//void search_ldf(int thread_idx, Graph* G, set<int>* U, int from, int to);
void thread_color_ldf(int thread_idx, vector<int>& wrong, Graph& G, int from, int to);
vector<int> v_t;


clock_t largestDegreeFirst(Graph& G, int num_threads) {
	

	//initialization
	vector <int> wrong;

	srand(static_cast <unsigned int> (time(0)));
	for (int i = 0; i < G.V; i++) {
		int compGuess = rand() % 100 + 1;
		G.weights[i]= compGuess;
		G.colored[i] = -1;
		
	}

	// compute time
	const clock_t begin_time = clock();

	
	RangeSplitter rs(G.V, num_threads);
	// run num_threads threads to color the subset of vertices 
	
	vector<thread> threads;
	for (int idx = 0; idx < num_threads && idx < G.V; idx++) {
		threads.emplace_back(thread([idx, &wrong, rs, &G] {
			int from = rs.get_min(idx);
			int to = rs.get_max(idx);
			thread_color_ldf(idx, wrong, G, from, to);
			}));
	}
		
	for (auto& t : threads)
		t.join();

	for (auto& w : wrong)
		G.color_vertex(w);

	const clock_t end_time = clock();
	return end_time - begin_time;

}

/*
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
					unique_lock<mutex> lock{ c };
					G->colored[v] = j;
					break;
				}
			}
			unique_lock<mutex> lock{ m };
			v_t.push_back(v);
		}
	}
}
*/


void thread_color_ldf(int thread_idx, vector<int>& wrong, Graph& G, int from, int to) {

	std::vector<int> vertices_to_color(to - from);
	std::iota(vertices_to_color.begin(), vertices_to_color.end(), from);
	
	std::stable_sort(vertices_to_color.begin(), vertices_to_color.end(), [&](int i, int j) {
		return (G.degree_of(i) > G.degree_of(j)) ||
			(G.degree_of(i) == G.degree_of(j) && G.weight(i) > G.weight(j));
		});

	for (const auto& vertex_to_color : vertices_to_color) {
		int my_color = G.color_vertex(vertex_to_color);
		for (const auto& neighbor : G.neighbors(vertex_to_color)) {
			if (my_color == G.color_of(neighbor)) {
				unique_lock<mutex> lock{ c };
				wrong.emplace_back(vertex_to_color);
				break;
			}
		}
	}
}