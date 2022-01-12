#pragma once
#include "graph.h"
#include <ctime>

clock_t greedyColoring(Graph& G){

  const clock_t begin_time = clock();

  vector<int> result (G.V);

  // Assign the first color to first vertex
  result[0] = 0;

  // Initialize remaining V-1 vertices as unassigned
  for (int u = 1; u < G.V; u++)
    result[u] = -1; // no color is assigned to u

  // A temporary array to store the available colors. True
  // value of available[cr] would mean that the color cr is
  // assigned to one of its adjacent vertices
  vector <bool> available(G.V);
  for (int cr = 0; cr < G.V; cr++)
    available[cr] = false;

  // Assign colors to remaining V-1 vertices
  for (int u = 1; u < G.V; u++)
  {
    // Process all adjacent vertices and flag their colors
    // as unavailable
    vector<int>::iterator i;
    for (i = G.adj[u].begin(); i != G.adj[u].end(); ++i)
      if (result[*i] != -1)
        available[result[*i]] = true;

    // Find the first available color
    int cr;
    for (cr = 0; cr < G.V; cr++)
      if (available[cr] == false)
        break;

    result[u] = cr; // Assign the found color

    // Reset the values back to false for the next iteration
    for (i = G.adj[u].begin(); i != G.adj[u].end(); ++i)
      if (result[*i] != -1)
        available[result[*i]] = false;
  }
  G.colored = move(result);

 const clock_t end_time = clock();
 

 return end_time - begin_time;

}