#include "../include/graph.hh"
#include "../third_party/SharedMap/include/libsharedmap.h"
#include "../include/fileUtils.hh"
#include <iostream>
#include <algorithm>
#include <limits>

Graph::Graph(long node_count) {
    adjacency_list.resize(node_count); // Platz für alle Knoten reservieren
    edge_count = 0;
}

// Hier muss ich drauf achten wie genau das Dateiformat die Graphen spezifiziert.
// Wenn die nodes bei index 1 anfangen statt 0, könnte das alles zerschießen!
void Graph::add_edge(long start_node, long destination_node){
    adjacency_list.at(start_node).push_back(destination_node);
    adjacency_list.at(destination_node).push_back(start_node);
    edge_count += 1;
}

void Graph::remove_edge(long start_node, long destination_node) {
    adjacency_list.at(start_node).erase(std::remove(adjacency_list.at(start_node).begin(), adjacency_list.at(start_node).end(), destination_node), adjacency_list.at(start_node).end());
    adjacency_list.at(destination_node).erase(std::remove(adjacency_list.at(destination_node).begin(), adjacency_list.at(destination_node).end(), start_node), adjacency_list.at(destination_node).end());
    edge_count -= 1;
}


CSR Graph::convertToCSR() {

    std::vector<int> adj_ptrs;
    std::vector<int> adj_weights;
    std::vector<int> adj;

    adj_ptrs.push_back(0);
    for(int i=0; i < adjacency_list.size(); i++) {
        for(int neighbor : adjacency_list.at(i)) {

            if (neighbor > std::numeric_limits<int>::max() || neighbor < std::numeric_limits<int>::min()) {
                std::cerr << "Warning: Value " << neighbor << " is too large to fit into an int and will be truncated." << std::endl;
            }

            adj.push_back(neighbor);
            adj_weights.push_back(1); // erstmal 1 weil wir noch keine Gewichte haben
        }
        if (adj.size() > static_cast<size_t>(std::numeric_limits<int>::max())) {
            std::cerr << "Warning: adj.size() exceeds the maximum value of int and may cause truncation." << std::endl;
        }
        adj_ptrs.push_back(adjacency_list.at(i).size()  + adj_ptrs.back()); // hier ist fehler
    }
    
        
    CSR csr;
    // Werte umwandeln und zuweisen
    csr.adj_ptrs = adj_ptrs;
    csr.adj_weights = adj_weights;
    csr.adj = adj;

    return csr;
}




//! Fürs erste: printe nur die Knoten die einen Nachbarn haben
void Graph::printGraph() {
    for (size_t i = 0; i < adjacency_list.size(); i++) {

        if(adjacency_list.at(i).size() == 0) {
            continue;
        }
        std::cout << "Knoten " << i << " ist verbunden mit: ";
        for (int neighbor : adjacency_list.at(i)) {
            std::cout << neighbor << " ";
        }
        std::cout << std::endl;
    }
}

std::vector<long> Graph::getPartition() {
    return partition;
}

void Graph::setPartition(std::vector<long> partition) {
    this->partition = partition;
}

void Graph::repartition() {
}

void Graph::setPartition(int* partition) {
    this->partition = std::vector<long>(partition, partition + this->adjacency_list.size());
}

int Graph::getNumberNodes() {
    return adjacency_list.size();
}

int Graph::getNumberEdges() {
    return edge_count;
}

std::vector<std::vector<long>> Graph::getGraph() {
    return adjacency_list;
}

void Graph::partitionWithSharedMap(std::string configFile){
    
    //1. Graph in CSR Format umwandeln
    CSR csr = this->convertToCSR();
    int n = this->getNumberNodes();
    
    //! ich setze die Knotengewichte standardmäßig auf 1
    std::vector<int> v_weights_vector(n, 1);
    int* v_weights = v_weights_vector.data();

    //2. Die anderen Paramter setzen 
    std::vector<int> hierarchy;
    std::vector<int> distance;
    int l;
    float imbalance;
    int   n_threads;
    int   seed;
    shared_map_strategy_type_t  strategy;                    
    shared_map_algorithm_type_t parallel_alg ;
    shared_map_algorithm_type_t serial_alg;
    bool verbose_error;
    bool verbose_statistics;

    bool configSucces = fileUtils::readConfigFileSharedMap(configFile, hierarchy, distance, l, imbalance, n_threads, seed, strategy, parallel_alg, serial_alg, verbose_error, verbose_statistics);

    if(!configSucces) {
        std::cerr << "Error reading config file. Exiting." << std::endl;
        return;
    }


    int comm_cost;
    int partition[n];


    // do the actual hierarchical multisection
    shared_map_hierarchical_multisection(n, v_weights, csr.adj_ptrs.data() ,  csr.adj_weights.data() , csr.adj.data(), hierarchy.data(), distance.data(), l, imbalance, n_threads, seed, strategy, parallel_alg, serial_alg, comm_cost, partition, verbose_statistics);

    std::cout << "Comm cost: " << comm_cost << std::endl;
    std::cout << "finished partitioning" << std::endl;
    this->setPartition(partition);

}



