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
    // Parse dataset
    list<graph_t> graphs = parse_files({
        "./data/facebook_clean_data/athletes_edges.csv",
        "./data/facebook_clean_data/artist_edges.csv",
        "./data/facebook_clean_data/company_edges.csv",
        "./data/facebook_clean_data/government_edges.csv",
        "./data/facebook_clean_data/new_sites_edges.csv",
        "./data/facebook_clean_data/public_figure_edges.csv",
        "./data/facebook_clean_data/tvshow_edges.csv",
        "./data/deezer_clean_data/HR_edges.csv",
        "./data/deezer_clean_data/HU_edges.csv",
        "./data/deezer_clean_data/RO_edges.csv"});
    
    // List 1 is for size = nb edges + nb verticies
    list<pair<int,double>> size_time_list1 {};
    // List 2 for size = nb edges
    list<pair<int,double>> size_time_list2 {};
    // List 3 for size = nb verticies
    list<pair<int,double>> size_time_list3 {};
    // CSV to store densities obtained
    ofstream densityFile ("./results.csv");
    densityFile << "edges," << "verticies," << "density," << "clique density," << "optimal density approximation," << "number of nodes in the subgraph," << "time" << endl;
    // Benchmark loop
    for (auto graph : graphs){
        clock_t start, end;
        // Three sizes for list 1, 2, 3
        int size1 = graph.edges_number+ graph.verticies_count;
        int size2 = graph.edges_number;
        int size3 = graph.verticies_count;
        double time_used;
        start = clock();
        pair<float,int_list> res = two_approx(graph);
        end = clock();
        float approx_density = res.first;
        int subgraph_nodes = res.second.size();
        float initial_density = ((float) graph.edges_number)/ graph.verticies_count;
        float clique_density = ((float) 2*graph.edges_number)/(graph.verticies_count*(graph.verticies_count-1));
        time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
        densityFile << 
            graph.edges_number << "," <<
            graph.verticies_count << "," <<
            initial_density << "," <<
            clique_density << "," <<
            approx_density << "," <<
            subgraph_nodes << "," <<
            time_used << endl;
        size_time_list1.push_back(pair<int,double>(size1,time_used));
        size_time_list2.push_back(pair<int,double>(size2,time_used));
        size_time_list3.push_back(pair<int,double>(size3,time_used));
    }
    densityFile.close();
    // Write benchmarks in a 3 csv files
    ofstream bench_file1 ("./benchmarks/benchmarks_edges+verticies.csv");
    bench_file1 << "size,time" << endl;
    for (auto p : size_time_list1){
        bench_file1 << p.first << "," << p.second << endl;
    }
    bench_file1.close();

    ofstream bench_file2 ("./benchmarks/benchmarks_edges.csv");
    bench_file2 << "size,time" << endl;
    for (auto p : size_time_list2){
        bench_file2 << p.first << "," << p.second << endl;
    }
    bench_file2.close();

    ofstream bench_file3 ("./benchmarks/benchmarks_verticies.csv");
    bench_file3 << "size,time" << endl;
    for (auto p : size_time_list3){
        bench_file3 << p.first << "," << p.second << endl;
    }
    bench_file3.close();

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
            vector<int_list> vec (max_vertex_index+1);
            graph_t graph {
                max_vertex_index+1,
                nb_edges,
                vec
            };
            // Reset get position of ifs
            ifs.clear();
            ifs.seekg(0, std::ios::beg);
            // First line is always node1, node2 so we ditch this
            getline(ifs,line);
            // Second loop to store edges
            while (getline(ifs,line)){
                size_t pos = line.find(delimiter);
                // Split between the comma
                int left = stoi(line.substr(0,pos));
                int right = stoi(line.substr(pos+1,line.length()));
                graph.adj_vec[left].push_back(right);
                graph.adj_vec[right].push_back(left);
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
    // After 0 steps, we have the starting density
    densities[0] = initial_density;
    //    
    // Main Loop, nb_verticies iterations
    //
    for (int step = 1; step < verticies_count; step++){
        int_list deg_list = degrees[min_degree];
        int node_to_remove = deg_list.front();
        // Update deleted nodes, number of edges and densities, all O(1)
        deleted_nodes[step]=node_to_remove;
        number_of_edges-=min_degree;
        densities[step] = ((float) number_of_edges/ (float) (verticies_count-step));
        current_degrees[node_to_remove]=0;
        // Now we update degree_list, we remove the node currently examinated
        // We will never examine it again as it is now in none of the lists
        // Composing the vector degrees
        degrees[min_degree].pop_front();
        
        // Now we update degrees of neighbours of the node removed
        int_list adj_list = adj_vec[node_to_remove];
        // Loop through the neighbours of the node to remove
        // This takes O(degree(node_removed))
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
        // Finding the new degree takes O(degree(v)), where v is the new node with min degree
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
    // End of main loop : performance analysis
    // We loop through all nodes of the graph, and we do two costly things while doing that : 
    // - First, loop through neigbours of the node to remove and update the vector "degrees" : O(deg(node to remove))
    // - Second, find the new min degree, by finding the first non-empty list in the vector "degrees". This non-empty list
    // is at the index new_min_degree, so we find it in O(new_min_degree) = O(deg(v)) for some v that we will remove
    // at the next iteration since it has new min degree
    // Finally, main loop takes O(sum_{v vertex} deg(v)) + O(sum_{v vertex} deg(v)) = O(2*nb_edges) = O(nb_edges) 


    // Finding best density, and putting nodes to keep in a list
    // This all takes O(nb of verticies)

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

    // Final performance analysis : 
    // O(nb_edges) for the main loop, O(nb_vertices) for the precomputation/work at the end, so :
    // O(nb_edges + nb_verticies) total, i.e linear in the size of the graph

	return pair<float,int_list>(densities[best_step],nodes_to_keep);
}