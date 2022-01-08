#pragma once
#include <iostream>
#include <list>
#include <fstream>
#include <vector>
#include <unordered_set>
#include <thread>
#include <mutex>
#include <random>
#include <set>
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

	
	// function to add an edge to graph

	void addEdge(int v, int w);

	// Prints greedy coloring of the vertices
};


void Graph::addEdge(int v, int w)
{
	adj[v].push_back(w);
	adj[w].push_back(v); // Note: the graph is undirected
}
//Jones Plassman Algorithm implementation 
void search_max(int v, unordered_set<int>*U, Graph *G);

vector<int> temp_c;

void jonesPlassman(Graph& G){
	unordered_set<int> U;
	//initialization
	srand(static_cast <unsigned int> (time(0)));
	for (int i=0;i<G.V;i++){
        int compGuess = rand() % 100 +1 ; 
		G.weights.push_back(compGuess);
		G.colored.push_back(-1);
	}
		
	//U subset of V, intially U = V
	for (int i=0;i<G.V;i++)
		U.insert(i);

	while(U.size()>0){
		// run U threads to search max weight neighbour
		{
			vector<thread> threads;
			for(auto& i : U){
				threads.emplace_back(thread(search_max,i,&U,&G));
			}

			for (auto& t : threads)
				t.join();

			
			// Remove from the set U the vertices to color in the current iteration 
			for (vector<int>::iterator a = temp_c.begin(); a != temp_c.end();a++){
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

void search_max(int v, unordered_set<int>*U, Graph *G) {

	
	int w = G->weights[v];
	//dario
	bool max = true;
	for (int j = 0; j < G->adj[v].size() && max==true; j++) {
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

// Largest-Degree-First Algorithm implementation 
void search_ldf(int v, Graph *G,set<int> same_degree) ;

void largestDegreeFirst(Graph& G){
	unordered_set<int> U;
	//initialization
	map <int,set<int>,greater<int>> degrees;
	srand(static_cast <unsigned int> (time(0)));
	for (int i=0;i<G.V;i++){
        int compGuess = rand() % 100 +1 ; 
		G.weights.push_back(compGuess);
		G.colored.push_back(-1);
		
		// Create a map containing entries of type (vertex_degree, index vector)

		// Compute degree of the vertex
		int deg = G.adj[i].size();
		// Update the map
		map<int, set<int>>::iterator it = degrees.find(deg); 
		
    	// if any other vertex has the same degree, add the vertex index in the corresponding deg vector
		if (it != degrees.end()) {
			it->second.insert(i);			
		}
		// else, create a new vector and add the new entry for the corresponding deg
		else {
			set<int> s;
			s.insert(i);
			std::pair<int,set<int>> p (deg,s);
			degrees.insert(p);
		}
	}

	for (std::pair<int, set<int>> entry : degrees) {
		
		set <int> s = entry.second;
		
		while (!s.empty()) {
			vector<thread> threads;
			for (auto& i : s) {
				threads.emplace_back(thread(search_ldf,i, &G, s));
			}
			for (auto& t : threads)
				t.join();

			// Remove from vertices to color in the current iteration 
			for (vector<int>::iterator a = temp_c.begin(); a != temp_c.end(); a++) {
				s.erase(*a);
			}

			// Clear the vector of vertices to color in this iteration
			temp_c.erase(temp_c.begin(), temp_c.end());
		}
	}
	 	
	

}

void search_ldf(int v, Graph *G,set<int> same_degree) {

	
	int w = G->weights[v];
	//dario
	bool max = true;
	for(auto& n : same_degree){
		vector<int>::iterator e = find(G->adj[v].begin(), G->adj[v].end(), n);
		
		// If it finds itself or the end of the vector -> not found
		// else, there is a neighbor with the same degree -> check the random weight 
		// or, in the remote case that also the weights are equals, choose the vertex with the highest index
		if(!(e==G->adj[v].end() || *e==v)){
			if (G->weights[*e]> w || (G->weights[*e] == w && *e > v))
				max=false;
				break;
		} 
	}
	

	if (max) {
		
		// Assign to the selected vertices the lowest possible color 
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


