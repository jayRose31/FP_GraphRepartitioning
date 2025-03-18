#include "../include/graph.hh"
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



