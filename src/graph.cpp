#include "../include/graph.hh"
#include <iostream>
#include <algorithm>

Graph::Graph(long node_count) {
    adjacency_list.resize(node_count); // Platz für alle Knoten reservieren
}

// Hier muss ich drauf achten wie genau das Dateiformat die Graphen spezifiziert.
// Wenn die nodes bei index 1 anfangen statt 0, könnte das alles zerschießen!
void Graph::add_edge(long start_node, long destination_node){
    adjacency_list.at(start_node).push_back(destination_node);
    adjacency_list.at(destination_node).push_back(start_node);
}

void Graph::remove_edge(long start_node, long destination_node) {
    adjacency_list.at(start_node).erase(std::remove(adjacency_list.at(start_node).begin(), adjacency_list.at(start_node).end(), destination_node), adjacency_list.at(start_node).end());
    adjacency_list.at(destination_node).erase(std::remove(adjacency_list.at(destination_node).begin(), adjacency_list.at(destination_node).end(), start_node), adjacency_list.at(destination_node).end());
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



