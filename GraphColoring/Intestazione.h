#pragma once
// A class that represents an undirected graph
#include <iostream>
#include <list>
#include <fstream>
#include <vector>
#define MAX 1000
using namespace std;

class Graph
{
	
	list<int>* adj; // A dynamic array of adjacency lists
public:
	// Constructor and destructor
	int V;
	Graph(int V) { this->V = V; adj = new list<int>[V]; }
	Graph(string str){
		fstream fin(str);
		char line[MAX];
		fin.getline(line, MAX);
		int n = line[0] - '0';
		this->V = n;
		adj = new list<int>[V];
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
		adj = other.adj;
		other.adj = nullptr;
	}

	~Graph() { delete[] adj; }

	
	// function to add an edge to graph

	void addEdge(int v, int w);

	void JonesPlassman();
};


void Graph::addEdge(int v, int w)
{
	adj[v].push_back(w);
	adj[w].push_back(v); // Note: the graph is undirected
}

void JonesPlassman(){
    //U subset of vertexes
    //Dario dichiara un vettore 
    for (int i=0;i<V;i++)
        U.push_back(i)
}