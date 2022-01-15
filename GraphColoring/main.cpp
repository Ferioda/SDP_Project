#include "graph.h"
#include "ReadInput.h"
#include "smallestDegreeLast.h"
#include "jonesPlassman.h"
#include "largestDegreeFirst.h"
#include "greedy.h"
#include <ctime>

#define MAX 1000
using namespace std;


int main(int argc, char** argv)
{
	vector <string> allGraphs = readPath("benchmark");
	
	string types[4] = {"jones_plassman", "largest_degree_first", "smallest_degree_last","greedy"};
	string filepath = "./graph_coloring.csv";
	bool correct = false;

	for (auto& g : allGraphs) {
		Graph graph = readFile(g);

		graph.write_graph_header(g, filepath);
		//apply the algorithm 
		//	void saveAsCSV(int n_thread, clock_t  time, string algorithm,string filepath);

		clock_t time_greedy = greedyColoring(graph);
		correct = graph.isWellColored();
			graph.saveAsCSV(0, (float) time_greedy/CLOCKS_PER_SEC, types[3], filepath,correct);

		for (int n_thread = 1; n_thread < 5; n_thread++) {
			clock_t time_jp= jonesPlassman(graph, n_thread);
			correct = graph.isWellColored();
				graph.saveAsCSV(n_thread,(float) time_jp/CLOCKS_PER_SEC,types[0], filepath, correct);

			clock_t time_ldf = largestDegreeFirst(graph, n_thread);
			correct = graph.isWellColored();
			graph.saveAsCSV(n_thread,(float) time_ldf/ CLOCKS_PER_SEC,types[1], filepath, correct);

			clock_t time_sdl = smallest_degree_last(graph, n_thread);
			correct = graph.isWellColored();
			graph.saveAsCSV(n_thread,(float) time_sdl/CLOCKS_PER_SEC,types[2], filepath, correct);
		}

		

	}

	return 0;
}