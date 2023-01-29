**Name : Guilhem JAZERON**

**Year 2022/2023**

# MITRO 209 Report 

This is the report for the linear implementation of the 2-approximation algorithm for computing densest subgraphs implemented in C++, by student Guilhem JAZERON.

## Table of contents

1. [Technical precisions](#precisions)
2. [Explanation of the implementation](#explanation)
3. [Benchmarks and results](#benchmarks)

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

The main ingredient of my implementation strategy is : keeping a vector of lists of nodes where `vec[i]` is the list of vectors with degree i, so that I could dynamically keep degrees updated as nodes get removed. In detail :
 
1. Precompute phase :
    - Initialize a vector `degrees` of lists of nodes, where the entry `i` is the list of nodes with degree `i`, an array `nodes_iterators` containing iterators representing the positions of nodes in the "degree lists", so we can do deletions in O(1) using these iterators and method `.erase()` of C++ lists. We aswell initalize an array `current_degrees` to store the current degree of each node. This is all done in ``O(number of nodes)``.
    - Loop through all nodes, to compute degrees of each node (done in `O(degree(v))` using adjacency lists), to fill the previously described data structures with correct data. We aswell compute the minimum degree and the total number of edges (using formula : `sum degrees = 2*nb of edges`). Each iteration of the loop takes `O(degree(v))` and since we loop through all nodes this all takes `O(sum (degrees))`= `O(nb edges)`
Precompute phase is thus in `O(nb edges + nb nodes)`.

2. Main loop :
    - Initialize `densities`, an array of float to keep track of the density at each step, and `deleted_nodes`, an array of ints to keep track of node deleted at each step. Store the initial density as `densities[0]` (density after 0 step) : this takes `O(nb nodes)`
    - Then, for `step` between `1` and `nb_nodes` :
        - Take the front element of the "degree list" corresponding to current minimum degree, pop it, compute new number of edges, and new density. Update arrays `densities` and `deleted_nodes` : `O(1)`
        - We need to update degrees of the node being deleted : loop through all neighbours of the node being deleted, get their degree, delete in `O(1)` them from their current "degree list" using the iterator and insert them in the list corresponding to their `degree-1`. Get a new iterator corresponding to the new position and update the iterator vector (`O(1)`). 
        
        So, this takes `O(nb of neighbours of node to remove)` = `O(degree of node to remove)`
        - Find new minimum degree : 
            - The new min degree must be at least min_degree-1, so we check for this case in `O(1)`
            - If it's not `min_degree-1`, increase `min_degree` until we find a "degree list" not empty (checking if a list is empty is `O(1)`). This takes `O(degree(node with new min degree))` 
So in total, for each iteration in the main loop we do `O(deg(v)) (update neighbours degree+ O(deg(v')) (find new min degree)` operations, where v and v' get removed. So in total, we do `O(sum(degree(v)))` operations, which is `O(nb edges)`. Adding the complexity of data structures initialization, we get `O(nb edges + nb nodes)`.

3. Find best density and nodes to keep : 

We do this by looping through the `densities` and `nodes_to_delete` arrays, and we return the best density, and the list of nodes to keep : this takes `O(nb nodes)` to do.

### Conclusion :

Combining phase 1, 2 and three we get a complexity of `O(nb edges + nb nodes)`, i.e linear in the size of the graph.

Let us now compare this theoritical complexity to the benchmarks effectively obtained.

## Benchmarks and results <a name="benchmarks"></a>

### Results 

In the following table are the intial densities and approximative densities for each one of the graphs from the datasets described in the introduction (see also `results.csv`) :

|edges  |verticies|density|optimal density approximation|number of nodes in the subgraph|time|
|-------|---------|-------|-----------------------------|-------------------------------|----|
|86858  |13866    |6.2641 |17.7619                      |42                             |0.019798|
|819306 |50515    |16.2191|58.018                       |1724                           |0.238899|
|52310  |14113    |3.70651|10.8969                      |97                             |0.017124|
|89455  |7057     |12.6761|36.4876                      |242                            |0.014969|
|206259 |27917    |7.38829|18.2217                      |1403                           |0.066963|
|67114  |11565    |5.8032 |34.5338                      |281                            |0.016919|
|17262  |3892     |4.43525|30.5938                      |64                             |0.004168|
|498202 |54573    |9.12909|16.3826                      |6775                           |0.187516|
|222887 |47538    |4.68861|8.58442                      |77                             |0.089362|
|125826 |41773    |3.01214|5.01235                      |162                            |0.057493|
|6797557|168114   |40.4342|137.204                      |4088                           |2.83759|
|92752  |28281    |3.27966|8.36782                      |87                             |0.038767|


### Benchmarks

I chose to include 3 differents plots that I found interesting

1. Time as function of nb edges + nb verticies, without the "big" graph from twitch dataset

I chose to include this first this graph because the points from the first graphs (meaning, without the graph with 7mil edges) are quite close one from another, allowing me to perform linear regression with sizes ranging from around 20k to 800k, and it looks like the complexity is indeed linear.

![Plot of time=f(number of edges + number of verticies), without twitch](/benchmarks_plots/edges+verticies_plot_first_points.png "Plot of time=f(number of edges + number of verticies), without twitch")

2. Time as function of nb edges + nb verticies, with the "big" graph from twitch dataset

Now I chose to add the graph from twitch, to see if even by multiplying the size of the input by 10, the time of execution would still be on the the line defined by the first points, which is the case. This conforts the fact that the theoritical complexity analysis is correct, and that the complexity is indeed **linear**

![Plot of time=f(number of edges + number of verticies)](/benchmarks_plots/edges+verticies_plot_all.png "Plot of time=f(number of edges + number of verticies)")

3. Time as function of nb edges

I chose to include this plot of the time as a function of only the number of edges, because in real life cases, the number of verticies is often small compared to the number of edges, and so I wanted to see if `O(nb edges)` could be a good complexity approximation for thoses cases where the number of verticies is small. We see below that we also get quite a nice line, which confirms my hypothesis.


![Plot of time=f(number of edges)](/benchmarks_plots/edges_plot.png "Plot of time=f(number of edges)")

## Conclusion

By looking at the theoritical analysis, and the benchmarks, it looks like the algorithm was successfully implemented in linear time in the input size, using doubly linked lists to update dynamically the degrees at each iteration.