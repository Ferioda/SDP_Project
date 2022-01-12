#include "graph.h"
#include "ReadInput.h"
#include "smallestDegreeLast.h"
#include "jonesPlassman.h"
#include "largestDegreeFirst.h"
#include "greedy.h"

#define MAX 1000
using namespace std;


int main(int argc, char** argv)
{
	vector <string> allGraphs = readPath("benchmark");
	
	string types[4] = {"jones_plassman", "largest_degree_first", "smallest_degree_last","greedy"};
	string filepath = "./graph_coloring.csv";

	for (auto& g : allGraphs) {
		Graph graph = readFile(g);
		//apply the algorithm 
		//	void saveAsCSV(int n_thread, clock_t  time, string algorithm,string filepath);

		clock_t time_greedy = greedyColoring(graph);
		graph.saveAsCSV(0, time_greedy, types[3], filepath);

		for (int n_thread = 2; n_thread < 5; n_thread++) {
			clock_t time_jp= jonesPlassman(graph, 2);
			graph.saveAsCSV(n_thread,time_jp,types[0], filepath);

			clock_t time_ldf = largestDegreeFirst(graph, 2);
			graph.saveAsCSV(n_thread,time_jp,types[1], filepath);

			clock_t time_sdl = smallest_degree_last(graph, 2);
			graph.saveAsCSV(n_thread,time_jp,types[2], filepath);
		}

		

	}
	cout << "Coloring of graph 1 \n";

	return 0;
}