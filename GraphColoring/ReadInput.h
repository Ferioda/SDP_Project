#pragma once

#include "graph.h"
#include <random>
#include <filesystem>

namespace fs = std::filesystem;
Graph readFile(string str);

vector<string> readPath(string path) {
	
	//fs::current_path(path);
	vector<string> allGraphs;

	for (const auto& entry : fs::recursive_directory_iterator(path)) {
		if (fs::is_regular_file(entry)) {
			std::filesystem::path p = fs::absolute(entry.path());
			string a = p.string();
			std::replace(a.begin(), a.end(), '\\', '/'); 
			allGraphs.push_back(a);
		}
	}

	return allGraphs;
}
Graph readFile(string str) {
	ifstream fin;
	fin.open(str);
	string line;
	getline(fin,line);
	int n=stoi(line);
	
	Graph g1(n);
	for (int i = 0; i < n; i++) {
		getline(fin,line);
		istringstream iss(line);
		string word;
		while (iss >> word) {
			int j;
			if (std::all_of(word.begin(), word.end(), ::isdigit)){
				stringstream(word) >> j;
				g1.addEdge(i,j );
				g1.addEdge(j, i);
			}
					

		}
		
		
	}
	fin.close();
	return (g1);
}
