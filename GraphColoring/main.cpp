#include "graph.h"
#include "ReadInput.h"
#include "smallestDegreeLast.h"
#include "jonesPlassman.h"
#include "largestDegreeFirst.h"
#include "greedy.h"
#include <ctime>
#include "memoryUsage.h"


#define MAX 1000
#define N_ALG 4
using namespace std;

const string types[N_ALG] = {"greedy" , "jones_plassman", "largest_degree_first", "smallest_degree_last"};
const string filepath = "./graph_coloring.csv";

void run_single(int t, const string types[N_ALG], Graph& graph, int n_thread);

int main(int argc, char** argv)
{
	if (argc != 2) {
		cout << "Wrong parameters on command line." << endl << "Correct usage <filename> <benchmark_directory_path>" << endl;
		return 1;
	}

	vector <string> allGraphs = readPath(argv[1]);
	
	for (auto& g : allGraphs) {
		
		// read the graph
		Graph graph = readFile(g);

		// write the name of the graph on the csv file
		graph.write_graph_header(g, filepath);
		
		//run the algorithms

		run_single(0, types, graph, 0);

		for (int n_thread = 1; n_thread < 5; n_thread++) {
			run_single(1, types, graph, n_thread);		// JP
			run_single(2, types, graph, n_thread);		// LDF
			//run_single(3, types, graph, n_thread);		// SDL
		}
	}

	return 0;
}

void run_single(int t,const string types[N_ALG],Graph& graph, int n_thread) {
	
	MemoryMonitor monitor;
	clock_t time;
	switch (t) {
	case 0:
		time = greedyColoring(graph);
		break;
	case 1:
		time = jonesPlassman(graph, n_thread);
		break;
	case 2:
		time = largestDegreeFirst(graph, n_thread);
		break;
	case 3:
		time = smallest_degree_last(graph, n_thread);
		break;
	default:
		cout << "Error, type not expected" << endl;
	}

	monitor.stop();
	bool correct = graph.isWellColored();
	double mem_usage = double(monitor.total());
	graph.saveAsCSV(n_thread, (float)time / CLOCKS_PER_SEC, types[t], filepath, mem_usage, correct);


}