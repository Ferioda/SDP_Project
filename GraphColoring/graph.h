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
		colored=vector<int>(V);
		weights=vector<int>(V);
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
	
	void saveAsCSV(int n_thread, float time, string algorithm,string filepath);

	bool isWellColored();
};

bool Graph::isWellColored() {
	/*
	// For all vertices...
    for (size_t idx = 0; idx < neighbor_indices.size(); idx++) {
        color_t from_color = color_of(idx);
        // For all edges...
        for (const uint32_t &to_idx : neighbors_of(idx)) {
            color_t to_color = color_of(to_idx);
            // Check that the color matches
            if (from_color == to_color)
                return false;
        }
    }
	*/
    return true;
	
}

void Graph::saveAsCSV(int n_thread, float time, string algorithm,string filepath) {
	std::ofstream file;
	string line;
	file.open(filepath, std::ios::out | std::ios::app);
	if (file.fail())
		return;
	try{
		//make sure write fails with exception if something is wrong
		file.exceptions(file.exceptions() | std::ios::failbit | std::ifstream::badbit);
		int max=(*max_element(colored.begin(), (colored.end())));
		file << algorithm << "," << n_thread << "," << V << "," << max + 1 << "," << time << endl;
	} catch( const exception &e) {
		cout << "Error while printing output";
	}
	
	
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