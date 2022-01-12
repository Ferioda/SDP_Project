#pragma once

#include "graph.h"
#include <filesystem>
#include <random>

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
	fstream fin(str);
	char line[MAX];
	fin.getline(line, MAX);
	int n=stoi(line);
	
	Graph g1(n);
	for (int i = 0; i < n; i++) {
		fin.getline(line, MAX);
		string s = line;
		istringstream iss(s);
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
