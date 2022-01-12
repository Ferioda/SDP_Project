<h1>Parallel Graph Coloring</h1>

This project investigates the implementation of several parallel algorithms for graph coloring and compares them to a well-known sequential algorithm in terms of time and memory.

# Introduction to graph coloring
In discrete mathematics, a graph is a structure consists of nodes called “Vertices” and links between these nodes called “Edges”. There are two main types of a graph, which are undirected graph and directed graphs, where an edge between 2 vertices has a direction. However, in our scope we focus only on undirected graphs.

Graph components such as vertex or edge can have a label and the process of assigning label to an edge or vertex is called “Graph Labeling” and a label is traditionally represented by an integer. In our project, we focus on vertex labeling only and the labels we are assigning are colors, then the process is called “Vertices Coloring” or generally “Graph Coloring”.

Graph Coloring is a well-known problem in the discrete mathematics field and its practical applications. It is a way to color the vertices of a graph in such a way that there are no two adjacent vertices have the same color. Therefore, there are many algorithms have been developed to solve this problem. Our Goal is to find the most appropriate on for a parallel environment.

In order to solve the problem of graph coloring in an efficient way, the parallel coloring is used by most of algorithms, which manipulate the multi-core ability of modern processors and multi-thread programming capabilities provided by operating systems to achieve higher performance.

Therefore, in this project, we implement a group of well-known graph-coloring algorithms to analyse and compare them with each other, to know the performance of each one compared to the other.

# Overview

The project is organized into parsers (`parse/`), solvers (`solve/`), parallelization utilities (`utils/`), and benchmarking utilities (`benchmark/`). Finally, `graph/` contains the data structure and some utility functions for storing graphs as an adjacency list.


## Solvers

### Greedy

`Greedy` implements the simple, well-known greedy strategy described in the specifications document, and mentioned in literature in *A parallel graph coloring heuristic*, M. Jones and P. Plassmann, 1992. It is single-threaded, i.e. sequential algorithm.

### Jones Plassman

`jonesPlassman` implements the algorithm from *A parallel graph coloring heuristic*, M. Jones and T. Plassmann, 1992. At its core, Jones' algorithm defines a total ordering on the vertices `rho`, and then colors each node for which all uncolored neighbors have a lower `rho`.

Although Jones' paper describes the algorithm in terms of message sending between different processors - one per vertex - we adopt a cleaner implementation. Notably, `rho` is generated once by a single thread, and message-passing is replaced by keeping a queue of "free vertices" that can be colored. Parallelizing this is trivial: we can have as many threads as we want acting as consumers on the free vertices queue. Because multiple threads may update the `num_wait` for a vertex (the number of uncolored neighbors with a lower rho), it must use an atomic int; no further synchronization is required.

### Largest Degree First (LDF)

`LDFsolver` implements the algorithm from *A Comparison of Parallel Graph Coloring Algorithms*, J. R. Allwright, 1995. As it is clear from it is name, the algorithm uses the degree of vertices in the subsets of the graph to decide which vertex to be colored before the other(Largest degree is colored firstly). This is a totlly different criterion from the independent set and random weights used in Jones and Lubys algorithms. In case of there are more than one vertex has the same degree, the priority of coloring is random in this case. The approach in LDF is to use the least number of colors. 

### Smallest Degree Last (SDL)

`SDL` implements the algorithm from *A Comparison of Parallel Graph Coloring Algorithms*, J. R. Allwright, 1995.This algorithm uses degrees also like LDF. However, unlike LDF it uses also weights to decide which vertex to be colored first. The algorithm goes into two phases. The first phase is the weighting phase, in which, all vertices in the subset have a degree equal to the smallest degree takes a weight equal the current weight, and this vertex is removed from our calculation for the comming vertices which decrease the degree of its neighbor. This processed is repeated many times until we have groups of vertices each one have a unique weight. Then, the second phase comes to color this groups starting from the largest weight to the smallest weight.

## Benchmarking

For benchmarking purposes we save the results of the solvers in a csv file called `xxx`. In this file, for each graph, we write computation time and number of colors used.

The Benchmark class is instantiated with a vector of solvers which implement the common interface `Solver`. For each run, the following parameters are measured:

# Results

The sequential algorithm and the three parallel algorithms have been implemented and their performance have been tested. We measured them with respect to the following measures:

 - Time elapsed
 - Number of colors used.

Thus, an efficient algorithm with high performance should achieve low time elapsed (high speed), low memory consumption, and few colors, which means the algorithm should use the least number of colors to color the whole graph.

We used an Intel Core i5-4300M 2.60GHz processor for benchmarking the graphs. Our report is available in `data-analysis/Data analysis.ipynb` (can be viewed in a browser at https://github.com/CapacitorSet/graph-coloring/blob/master/data-analysis/Data%20analysis.ipynb), as well as the raw data. These were our final findings:

>The choice of algorithm results in vastly different performances. Early multithread algorithms found in literature may perform significantly worse than simpler single-thread ones, and thus be only of theoretical interest; more recent ones typically provide a measurable speedup. Simple heuristics like FVF and SDL can outperform such algorithms, as they rarely miscolor the graph and such errors can be corrected quickly.
