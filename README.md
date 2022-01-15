# SDP_Project Graph Coloring

This is a short "user's manual"; for information on the algorithms and the design choices refer to [Documentation](./Documentation.md).

## Compile (Windows)

Download the repository and open the solution with Visual Studio.


If compiler errors are displayed go to:

> Project --> Properties --> C/C++ --> Language 

At the line `standard of C++ language` select the `/std:c++latest` option from the dropdown menu.

 ## Run
 
the only parameter to provide to the algorithm is the path to the benchmark directory


## Notes and critics to the project
The program does not handle graphs stored in DIMACS10 format.
Only DIMACS graph are allowed.

Do not run Smallest Degree Last with large graphs, we leave it commented because our implementation is too slow.