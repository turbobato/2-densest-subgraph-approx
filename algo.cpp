#include <iostream>
#include <fstream>
#include <list>
#include <vector>
#include <utility>
#include <string>
#include <time.h>

//
// Type declarations
//

using namespace std;

using int_list = list<int>;

struct graph {
    int verticies_count {};
    int edges_number {};
    vector<int_list> adj_vec {};
};

using graph_t = struct graph;

//
// End of declarations
//


//
// Prototypes declarations
//

pair<float,int_list> two_approx(graph_t G);

list<graph_t> parse_files(list<string> filepaths);

//
// End of declarations
//

// main function, parse files and benchmark algo on the dataset from this link
// http://snap.stanford.edu/data/gemsec-Facebook.html
int main(){
    // graph_t G = {
    //     5,
    //     {{1,2},{0},{0},{4},{3}}
    // };
    // pair<float,int_list> res = two_approx(G);
    // cout << "density" << res.first << endl;
    // cout << "nodes to keep" << endl;
    // for (auto i : res.second){
    //     cout << i << endl;
    // }

    // Parse dataset
    list<graph_t> graphs = parse_files({
        "./data/facebook_clean_data/athletes_edges.csv",
        "./data/facebook_clean_data/artist_edges.csv",
        "./data/facebook_clean_data/company_edges.csv",
        "./data/facebook_clean_data/government_edges.csv",
        "./data/facebook_clean_data/new_sites_edges.csv",
        "./data/facebook_clean_data/public_figure_edges.csv",
        "./data/facebook_clean_data/tvshow_edges.csv"});
    list<pair<int,double>> size_time_list {};
    // Benchmark loop
    for (auto graph : graphs){
        clock_t start, end;
        // Size is O(nb verticies + nb edges)
        int size = graph.verticies_count + graph.edges_number;
        double time_used;
        start = clock();
        two_approx(graph);
        end = clock();
        time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
        size_time_list.push_back(pair<int,double>(size,time_used));
    }
    // Write benchmarks in a csv file
    ofstream bench_file ("./benchmarks.csv");
    bench_file << "size,time" << endl;
    for (auto p : size_time_list){
        bench_file << p.first << "," << p.second << endl;
    }
    return 0;
}

//Helper function to parse the csv files into graph_t structs
list<graph_t> parse_files(list<string> filepaths){
    list<graph_t> res {};
    for (auto path : filepaths){
        ifstream ifs {};
        string line;
        // We are parsing CSV
        string delimiter = ",";
        // Keep track of the max vertex number encoutered
        int max_vertex_index = 0; 
        int nb_edges = 0;
        ifs.open(path);
        if (ifs.is_open()){
            // First line is always node1, node2 so we ditch this
            getline(ifs,line);
            // First loop to get the number of verticies and edges
            while (getline(ifs,line)){
                size_t pos = line.find(delimiter);
                // Split between the comma
                int left = stoi(line.substr(0,pos));
                int right = stoi(line.substr(pos+1,line.length()));
                if (left > max_vertex_index){
                    max_vertex_index = left;
                }
                if (right > max_vertex_index){
                    max_vertex_index = right;
                }
                // Augment the number of edges
                nb_edges++;
            }
            // Initialize a vector the right size
            vector<int_list> adj_vec (max_vertex_index+1);
            graph_t graph {
                max_vertex_index+1,
                nb_edges,
                adj_vec
            };
            // Reset get position of ifs
            ifs.seekg(0);
            // First line is always node1, node2 so we ditch this
            getline(ifs,line);
            // Second loop to store edges
            while (getline(ifs,line)){
                size_t pos = line.find(delimiter);
                // Split between the comma
                int left = stoi(line.substr(0,pos));
                int right = stoi(line.substr(pos+1,line.length()));
                adj_vec[left].push_back(right);
                adj_vec[right].push_back(left);
            }
            // Close file
            ifs.close();
            // Add graph to the list
            res.push_back(graph);
        }
        else {
            cout << "Issue opening " << path << endl;
        }
    }
    return res;
}

// Algorithm to compute two approx
pair<float,int_list> two_approx(graph_t G){
    // Copy G into H
    graph_t H = G;
    // Collect input variables, vectors are copied in O(nb_verticies)
    int verticies_count = G.verticies_count;
    vector<int_list> adj_vec = G.adj_vec;

    // Create a vector of lists where degrees[i] is the list of nodes with degree i
    // We aswell want to keep a vector of iterators to the nodes in the (doubly) linked lists,
    // to ensure O(1) deletions
    // We also want to keep track of the current degrees of each node
    vector<int_list> degrees = vector<int_list>(verticies_count);
    vector<int_list::iterator> nodes_iterators = vector<int_list::iterator>(verticies_count);
    int current_degrees[verticies_count];

    // Put nodes in the good lists, keep track of the iterators, compute min degree 
    // at the same time, total number of edges, and 
    int min_degree = verticies_count;
    int number_of_edges = 0;
    for (int i=0; i<verticies_count; i++){
        // get degree of node i
        int degree = adj_vec[i].size();
        // update current degrees and min degree
        current_degrees[i] = degree;
        if (degree < min_degree){
            min_degree = degree;
        }
        // update number of edges
        number_of_edges+=degree;
        // add node i to the degree_list corresponding to its degree
        degrees[degree].push_front(i);
        // get an iterator to the position of the newly inserted vertix
        int_list::iterator it = degrees[degree].begin();
        nodes_iterators[i] = it;
        }
    number_of_edges=number_of_edges/2;

    // This all took O(sum (degrees)) = O(graph size) to do
    // Now we iterate through the nodes, removing the node with smallest degree,
    // and we keep track of the order in which we delete the nodes, and of the 
    // density of the subgraph at each step
    float initial_density =  ((float) number_of_edges/ (float) verticies_count);
    int deleted_nodes[verticies_count];
    // Array where densisties[i] is the density after i steps
    float densities[verticies_count];
    densities[0] = initial_density;
    //    
    // Main Loop 
    //
    for (int step = 1; step < verticies_count; step++){
        int_list deg_list = degrees[min_degree];
        int node_to_remove = deg_list.front();
        // Update deleted nodes, number of edges and densities
        deleted_nodes[step]=node_to_remove;
        number_of_edges-=min_degree;
        densities[step] = ((float) number_of_edges/ (float) (verticies_count-step));
        current_degrees[node_to_remove]=0;
        // Now we update degree_list
        degrees[min_degree].pop_front();
        
        // Now we update degrees of neighbours of the node removed
        int_list adj_list = adj_vec[node_to_remove];
        // Loop through the neighbours of the node to remove
        for (const auto & node : adj_list){
            // We get the degree before deletion
            int node_degree = current_degrees[node];
			// Verify that the node hasn't already been deleted
            if (node_degree !=0){
				// We get the iterator pointing to the node
				int_list::iterator it = nodes_iterators[node];
				// Remove it in O(1) using erase method
				degrees[node_degree].erase(it);
				// Update the degree
				node_degree-=1;
				current_degrees[node] = node_degree;
				// Add the node to the good list
				degrees[node_degree].push_front(node);
				// Get a new iterator
				it = degrees[node_degree].begin();
				nodes_iterators[node] = it;    
			}         
        }


        // Find new min degree
        // The new min degree must be at least min_degree-1, so we check for this case
        if (min_degree!=0 && !degrees[min_degree-1].empty()){
            min_degree = min_degree-1;
        }
        // If it's not min_degree-1, it's >= min_degree
        else {
            int_list l = degrees[min_degree];
            while (l.empty() && ++min_degree < verticies_count){
                l = degrees[min_degree];
            }
        }
    }
    // Get the last node, staying after main loop :
    int last_node = degrees[min_degree].front();
    // Loop through all the densities to find the best one
    int best_step=0;
    for (int step = 1; step<verticies_count; step++){
        if (densities[step] > densities[best_step]){
            best_step = step;
        }
    }
    // Generate the list of nodes to keep 
    int_list nodes_to_keep = int_list();
    // Add the last node which is never removed
    nodes_to_keep.push_front(last_node);
    // We wish to keep all the nodes deleted after the best step
    for (int i = best_step+1; i < verticies_count; i++){
        nodes_to_keep.push_front(deleted_nodes[i]);
    }
	return pair<float,int_list>(densities[best_step],nodes_to_keep);
}