#pragma once
#include "graph.h"
#include <ctime>

//Jones Plassman Algorithm implementation 

void thread_color(int thread_idx, vector<int>& wrong, Graph& G, int from, int to);

clock_t jonesPlassman(Graph& G, int num_threads) {
	
	//initialization
	vector <int> wrong;
	srand(static_cast <unsigned int> (time(0)));
	for (int i = 0; i < G.V; i++) {
		int compGuess = rand() % 100 + 1;
		G.weights[i] = compGuess;
		G.colored[i] = -1;
	}
	const clock_t begin_time = clock();
	RangeSplitter rs(G.V, num_threads);

	// run num_threads threads to color the subset of vertices 
	vector<thread> threads;
	for (int idx = 0; idx < num_threads && idx < G.V; idx++) {
		threads.emplace_back(thread([idx,&wrong, rs, &G] {
		int from = rs.get_min(idx);
		int to = rs.get_max(idx);
		thread_color(idx, wrong, G, from, to);
			}));
	}
	for (auto& t : threads)
		t.join();

	for (auto& w : wrong)
		G.color_vertex(w);
	
	const clock_t end_time = clock();
	return end_time - begin_time;
}


void thread_color(int thread_idx, vector<int> &wrong, Graph& G, int from, int to) {

	std::vector<int> vertices_to_color(to - from);
	std::iota(vertices_to_color.begin(), vertices_to_color.end(), from);
	std::stable_sort(vertices_to_color.begin(), vertices_to_color.end(), [&](int i, int j) {
		return G.weights[i] > G.weights[j];
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