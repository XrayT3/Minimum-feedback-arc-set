# Minimum-feedback-arc-set
Find best solution using local search (hill climbing)

This program implements a hill climbing algorithm to find a minimum feedback arc set. The algorithm considers the weights and degrees of the edges and uses heuristics to improve the solution iteratively.

## Usage

To run the program, you need to provide three command-line arguments:

1. Input file path
2. Output file path
3. Deadline in seconds (a float value)

```sh
./main input.txt output.txt 3.0
```

## Input Format
The input file should contain the following information:

1. The number of edges (an integer `n`).
2. For each edge, three integers: the start vertex, the end vertex, and the edge weight.

### Example:

```
7
1 2 2
1 3 5
2 4 4
3 1 3
3 4 3
4 1 3
4 2 7
```

## Output Format
The output file will contain:

1. The best objective value found.
2. A list of deleted edges in the format "start_vertex end_vertex".

### Example:
```
9
1 3
2 4
```

This indicates the best objective value found is 9, achieved by deleting edges (1, 3) and (2, 4).


## Algorithm Description

1. Sort edges by weight or by weight and degree
2. Find feasible solution
    1. Init empty graph `G`
    2. Put edge from the ordered list in the graph `G`
    3. Check if it creats a cycle in graph `G`
       1. If yes, remove edge and add it to the arc set
       2. Otherwise update cost
    4. Repeat step 2 and 3 for every edge in the list
3. Start local search using hill climbing algorithm
    1. Swap two random edges in the ordered list
    2. Find feasible solution (step 2)
    3. If solution is better, update the best result and the order of the edges, otherwise start again from the previous order. 

In other words, it generates an initial feasible solution and updates the best known solution. 
The program sorts the edges based on weights and degrees to improve the initial solution. 
The hill climbing algorithm iteratively swaps edges and evaluates new solutions to find the best one within the given deadline.


## Suggestions for improvement

- Multithreading for `hill_climbing` (every thread should run this function in parallel, threads have to share the best solution).
  - I already tested it and it works much better. I don't publish it because I cannot guarantee the correctness of my implementation. Despite the fact that it passed all the tests, it wasn't stable.
- Find Strong Connected Components and remove all edges from the list that are not in any of the components. These edges cannot create a cycle, so they cannot be in the minimum feedback arc set.
  - Not tested.

