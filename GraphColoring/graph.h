#pragma once
#include "RangeSplitter.h"
#include <iostream>
#include <list>
#include <fstream>
#include <vector>
#include <unordered_set>
#include <thread>
#include <mutex>
#include <random>
#include <set>
#include <numeric>
#include <map>
#define MAX 1000
using namespace std;


class Graph
{
public:
	vector<vector<int>> adj; // A dynamic array of adjacency lists
	vector<int> weights;
	vector<int> colored;
	
	// Constructor and destructor
	int V;
	Graph(int V) { 
		this->V = V; 
		for (int i=0;i<V;i++){
			vector<int> v;
			adj.push_back(v); 
		}
			
	}

	Graph(string str){
		fstream fin(str);
		char line[MAX];
		fin.getline(line, MAX);
		int n = line[0] - '0';
		this->V = n;
		for (int i = 0; i < n; i++) {
			fin.getline(line, MAX);
			int j = 3;
			while (line[j] != '#') {
				if (isdigit(line[j])) {
					int v = (int)(line[j] - '0');
					cout << v;
					adj[v].push_back(i);
					adj[i].push_back(v);
				}

				j++;
			}
		}
		fin.close();
	}
	Graph(Graph && other) noexcept{
		V = other.V;
		adj = move(other.adj);
		weights = move(other.weights);
		colored=move(other.colored);
	
	}

	~Graph() {
		adj.clear();
		weights.clear();
		colored.clear();
	}

	int degree_of(int v);
	
	// function to add an edge to graph

	void addEdge(int v, int w);

	int weight(int v);
	// Prints greedy coloring of the vertices
};

int Graph::degree_of(int v){
	return adj[v].size();
}

int Graph::weight(int v){
	return weights [v];
}
void Graph::addEdge(int v, int w)
{
	adj[v].push_back(w);
	adj[w].push_back(v); // Note: the graph is undirected
}
//Jones Plassman Algorithm implementation 
void search_max(int thread_idx, set<int>*U, Graph *G,int from, int to);

vector<int> temp_c;

void jonesPlassman(Graph& G, int num_threads) {
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
					search_max(idx, &U, &G, from, to);
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
//mutexes used in search_max and search_ldf
mutex m;
mutex c;

void search_max(int thread_idx, set<int>*U, Graph *G, int from, int to) {
    
	
	for (int k = from; k < to; k++) {
		int v =*next(U->begin(),k);
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
			temp_c.push_back(v);
		}
	}
}




// Largest-Degree-First Algorithm implementation 
void search_ldf(int thread_idx, Graph *G,set<int> *U, int from, int to ) ;

void largestDegreeFirst(Graph& G, int num_threads){
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
				for (vector<int>::iterator a = temp_c.begin(); a != temp_c.end(); a++) {
					U.erase(*a);
				}

				// Clear the vector of vertices to color in this iteration
				temp_c.erase(temp_c.begin(), temp_c.end());

			}
		}
		 	
	

}

void search_ldf(int thread_idx, Graph *G,set<int> *U, int from, int to) {

	
	for (int k = from; k < to; k++) {
		int v =*next(U->begin(),k);
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
			temp_c.push_back(v);
		}
	} 
}