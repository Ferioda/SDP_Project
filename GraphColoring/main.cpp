#include "graph.h"
#include "ReadInput.h"
#define MAX 1000
using namespace std;



int main(int argc, char** argv)
{
	vector <Graph> allGraphs = readPath("/Users/bigmi/Desktop/sdp project/benchmark");
	Graph g1=readFile("./cuda.gra");

	
	cout << "Coloring of graph 1 \n";
	
	
	//jonesPlassman(g1, 4);
	//largestDegreeFirst(g1,4);
	smallest_degree_last(g1, 4);


	
	return 0;
}