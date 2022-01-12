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

	vector <int> colors();

	int weight(int v);
	
	void saveAsCSV(int n_thread, clock_t  time, string algorithm,string filepath);
};


void Graph::saveAsCSV(int n_thread, clock_t  time, string algorithm,string filepath) {
	std::ofstream file;
	string line;
	file.open(filepath, std::ios::out | std::ios::app);
	if (file.fail())
		return;

	//make sure write fails with exception if something is wrong
	file.exceptions(file.exceptions() | std::ios::failbit | std::ifstream::badbit);
	
	for (int i = 0; i < V; i++) 
		//Algorithm n_thread n_v n_colors time
		file<< "%s ,%d ,%d ,%d ,%d \n" <<algorithm, n_thread, V, (* max_element(colored.begin(), colored.end())+1),time;
	
	
}
vector <int> Graph:: colors() {
	return colored;
}

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