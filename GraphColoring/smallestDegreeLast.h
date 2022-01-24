#pragma once
#include "graph.h"
#include <ctime>
#include <mutex>

mutex d;
void thread_weight_sdl(Graph & G,int idx, int from,int to,vector<int> & degrees);
void thread_color_sdl(int thread_idx, vector<int>& wrong, Graph& G, int from, int to);


clock_t smallest_degree_last(Graph& G, int num_threads) {
	const clock_t begin_time = clock();

	//initialization
	vector <int> wrong;
    vector <int> degrees(G.V);
	for (int i = 0; i < G.V; i++){
        G.colored[i] = -1;
        G.weights[i] = 0;
        degrees[i]=G.degree_of(i);
    }

    RangeSplitter rs(G.V, num_threads);
	// run num_threads threads to color the subset of vertices 
	
	vector<thread> threads;
	for (int idx = 0; idx < num_threads && idx < G.V; idx++) {
		threads.emplace_back(thread([idx, &wrong, rs, &G, &degrees] {
			int from = rs.get_min(idx);
			int to = rs.get_max(idx);
			
			thread_weight_sdl(G,idx, from ,to,degrees);
	        
			thread_color_sdl(idx, wrong, G, from, to);
			}));
        
	}
    
	for (auto& t : threads)
		t.join();

	for (auto& w : wrong)
		G.color_vertex(w);

	const clock_t end_time = clock();
	return end_time - begin_time;
}

void thread_weight_sdl(Graph& G, int idx, int from, int to, vector<int>& degrees) {
	//i for assign the weights 
	int k = 1;
	int i = 1;
	int max = -1;

	for (int j = from; j < to; j++) {
		max = std::max(max, degrees[j]);
	}
	/*
	int index = 0;
	for (int j = from; j < to; j++)
		if (degrees[j] == max)
			index = j;
	*/
	//when k reaches the max, it means that every vertex is assigned a weight
	while (k <= max) {
		for (int v = from; v < to; v++) {
			unique_lock<mutex> lock{ d };
			if (degrees[v] <= k && degrees[v] >= 0) {
				//assign the weight ant then 				
				G.weights[v] = i;
				degrees[v] = -1;
				lock.unlock();
				//reduce the degrees of the neighbors of 1
				for (int n : G.neighbors(v)) {
					lock.lock();
					if (degrees[n] > 0) {
						degrees[n]--;
					}
					lock.unlock();
				}
			} 
		}
		k++;
		i++;
	}
}

void thread_color_sdl(int thread_idx, vector<int>& wrong, Graph& G, int from, int to) {
	std::vector<int> vertices_to_color(to - from);
	std::iota(vertices_to_color.begin(), vertices_to_color.end(), from);

	std::stable_sort(vertices_to_color.begin(), vertices_to_color.end(), [&](int i, int j) {
		return (G.weight(i) > G.weight(j));
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
