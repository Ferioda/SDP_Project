# SDP_Project

# Algorithms

## Jonson Plassman
The Jones-Plassmann algorithm then proceeds very much like the MIS algorithm, except
that it does not find a maximal independent set at each step. It just finds an
independent set in parallel using Luby's method of choosing vertices whose weights are local
maxima. The other difference is that the vertices in the independent set are not
assigned the same new color, as they are in the MIS algorithm. Instead, the vertices are
colored individually using the smallest available color, i.e. the smallest color that has not
already been assigned to a neighboring vertex.

## Largest Degree First
The Largest-Degree-First algorithm can be parallelized using a very similar method to
the Jones{Plassmann algorithm. The only difference is that instead of using random weights
to create the independent sets, the weight is chosen to be the degree of the vertex in the
induced subgraph. Random numbers are only used to resolve conflicts between neighboring
vertices having the same degree. In this method, vertices are not colored in random order,
but rather in order of decreasing degree, with those of largest degree being colored first.
This approach aims to use fewer colors than the Jones Plassmann algorithm. A vertex
with i colored neighbors will require at most color i+1. The Largest-Degree-First algorithm
aims to keep the maximum value of i as small as possible throughout the computation, so
that there is a better chance of using only a small number of colors.

## //TODO:

##  Smallest Degree Last
The Smallest-Degree-Last algorithm [24] tries to improve upon the Largest-Degree-First
algorithm by using a more sophisticated system of weights. In order to achieve this the
algorithm operates in two phases, a weighting phase and a coloring phase.
The weighting phase begins by finding all vertices with degree equal to the smallest
degree d presently in the graph. These are assigned the current weight and removed from
the graph, thus changing the degree of their neighbors. The algorithm repeatedly removes
vertices of degree d, assigning successively larger weights at each iteration. When there are
no vertices of degree d left, the algorithm looks for vertices of degree d+1. This continues
until all vertices have been assigned a weight.
Once the weights have been assigned, coloring proceeds as in the Jones-Plassmann
and Largest-Degree-First algorithms. This coloring procedure works using the weights
assigned by the first stage in the same way that the Largest-Degree-First algorithm uses
the degree of the vertices. In other words the coloring phase has each vertex look around
at its uncolored neighbors and when it discovers it has the highest weight (conflicts once
again being resolved by a random number), it colors itself using the lowest available color
in its neighborhood.
