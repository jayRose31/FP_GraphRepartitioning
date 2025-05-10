#include "../include/graphLocalSearch.hh"

#include "../include/fileUtils.hh"
#include <iostream>
#include <algorithm>
#include <limits>
#include <unordered_map>


void graphLocalSearch::mark_dirty(long vertex) {
    dirty_vertices.insert(vertex); // Fügt den Knoten hinzu, falls er noch nicht enthalten ist
}

// Überschreibe die add_edge-Methode
void graphLocalSearch::add_edge(long start_node, long destination_node) {

    Graph::add_edge(start_node, destination_node);

    mark_dirty(start_node);
    mark_dirty(destination_node);

}

void graphLocalSearch::remove_edge(long start_node, long destination_node) {

    Graph::remove_edge(start_node, destination_node);

    mark_dirty(start_node);
    mark_dirty(destination_node);
}

void graphLocalSearch::print_dirty_vertices() {
    for (const auto& vertex : dirty_vertices) {
        std::cout << vertex << " ";
    }
    std::cout << std::endl;
}


std::unordered_map<long, int> graphLocalSearch::compute_gains_simplified(long from_node) {
    
    long from_partition = this->partition.at(from_node); 
    std::unordered_map<long, int> external_degrees;
    int internal_degree = 0;

    std::vector<long> neighbors = this->adjacency_list.at(from_node);

    for (long neighbor : neighbors) {
        long neighbor_partition = this->partition.at(neighbor);
        if( neighbor_partition == from_partition) {
            internal_degree++;
        } else {
            // increase external degree to that partition

            // case distinction, wether entry already exists or not
            if( external_degrees.count(neighbor_partition) > 0 ) {
                external_degrees.at(neighbor_partition) += 1;
            } else {
                external_degrees[neighbor_partition] = 1;
            }
        }
    }

    std::unordered_map<long, int> gains;
    for (const auto& [partition_id, degree] : external_degrees) {
        gains[partition_id] = degree - internal_degree;
    }

    return gains;

}



void graphLocalSearch::repartition() {

    // look at all dirty vertices and see if you want to move
    // them to another partition
    for (const auto& vertex : dirty_vertices) {
        int gain_best = 0;
        int best_partition = 0;
        std::unordered_map<long, int> gains = compute_gains_simplified(vertex);
    }

}