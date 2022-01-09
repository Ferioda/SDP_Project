#include "graph.h"
#include <cstring>
#include <string>
#include <climits>
#include <fstream>
#include <iostream>
#include <vector>
#include <time.h>
#include <iomanip>
#include <algorithm>
#include <tuple>
#include <set>
#define MAX 1000
using namespace std;


Graph readFile(string str) {
	fstream fin(str);
	char line[MAX];
	fin.getline(line, MAX);
	int n = line[0] - '0';
	Graph g1(n);
	for (int i = 0; i < n; i++) {
		fin.getline(line, MAX);
		int j = 3;
		while (line[j] != '#') {
			if (isdigit(line[j])) {
				int v = (int)(line[j] - '0');
				cout << v;
				g1.addEdge(i, v);
			}

			j++;
		}
	}
	fin.close();
	return (g1);
}

int main(int argc, char** argv)
{
	Graph g1=readFile("./cuda.gra");

	
	cout << "Coloring of graph 1 \n";
	//jonesPlassman(g1, 4);
	largestDegreeFirst(g1,4);


	
	return 0;
}