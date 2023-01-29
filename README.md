**Name : Guilhem JAZERON**

**Year 2022/2023**

# MITRO 209 Report 

This is the report for the linear implementation of the 2-approximation algorithm for computing densest subgraphs implemented in C++, by student Guilhem JAZERON.

## Table of contents

1. [Technical precisions](#precisions)
2. [Explanation of the implementation](#explanation)
3. [Benchmarks](#benchmarks)

## Technical precisions <a name="precisions"></a>

- I chose to use 4 different datasets to test my algorithm on a total of 11 graphs. The sources of the datasets are :
    - http://snap.stanford.edu/data/gemsec-Facebook.html
    - http://snap.stanford.edu/data/gemsec-Deezer.html
    - http://snap.stanford.edu/data/feather-deezer-social.html
    - http://snap.stanford.edu/data/twitch_gamers.html

The first three datasets contain graphs with size of the same order, namely size between 20k and 800k edges, while I chose the graph from twitch because it has around 7 million edges and I wanted to test the algorithm on a bigger input.

- To compile the main algorithm, use `g++ -std=c++17 main.cpp -o target` with g++ installed. This will : 
    - Run the algorithm on the graphs given above
    - Output the results in the file results.csv
    - Produce three benchmarks in the benchmarks folder, one with size being the number of edges, an other one with size being the number of verticies, and the last with size being the number of edges + the number of verticies (which interests us the most).

- The script `produce_plots.py` is the script I used to produced the complexity graphs presented in third part.

- Inside the file `main.cpp`, there are three functions : 
    - `main`, which gathers the data from the csv files, runs the algorithm and produces benchmarks.
    - `parse_files`, which is a helper function to convert the csv files into graph structs.
    - `two_approx`, which is the approximation algorithm.

## Explanation of the implementation <a name="explanation"></a>

As precised above, the code that is interesting for us in only the code contained in the function `two_approx`. The source code is highly commented, and I will mainly rephrase here what is described in the comments, so for seeing the implementation while having the explanations, you might want to look directly at the code. 

In all the following, when talking about lists, I will mean linked lists, which ensure O(1) deletions and insertions.

A first important remark is that I made sure to use C++ references and pointers, so that no copy is made in the algorithm, and all operations can be supposed atomic i.e taking O(1). Also, I chose to represent graphs as vector of adjacency lists

The main ingredients of my implementation strategy is : keeping a vector of lists of nodes where `vec[i]` is the list of vectors with degree i, so that I could dynamically keep degrees updated as nodes get removed. In detail :
 
1. Precompute phase :
    - Initialize a vector `degrees` of lists of nodes, where the entry `i` is the list of nodes with degree `i`, an array `nodes_iterators` containing iterators representing the positions of nodes in the "degree lists", so we can do deletions in O(1) using these iterators and method `.erase()` of C++ lists. We aswell initalize an array `current_degrees` to store the current degree of each node. This is all done in ``O(number of nodes)``.
    - Lop through all nodes, to compute degrees of each node (done in `O(degree(v))` using adjacency lists), to fill the previously described data structures with correct data. We aswell compute the minimum degree and the total number of edges (using formula : sum degrees = 2*nb of edges). This is all done in `O(sum (degrees))`= `O(nb edges)`
Precompute phase is thus in `O(nb edges + nb nodes)`

2. Main loop :

## Benchmarks <a name="benchmarks"></a>

![Plot of time=f(number of edges + number of verticies), without twitch](/benchmarks_plots/edges+verticies_plot_first_points.png "Plot of time=f(number of edges + number of verticies), without twitch")

![Plot of time=f(number of edges + number of verticies)](/benchmarks_plots/edges+verticies_plot_all.png "Plot of time=f(number of edges + number of verticies)")

![Plot of time=f(number of edges)](/benchmarks_plots/edges_plot.png "Plot of time=f(number of edges)")