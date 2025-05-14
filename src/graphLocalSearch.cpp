#include "../include/graphLocalSearch.hh"

#include "../include/fileUtils.hh"
#include <iostream>
#include <algorithm>
#include <limits>
#include <unordered_map>


void graphLocalSearch::set_imbalance_from_file(std::string configFile) {
    this->imbalance = fileUtils::readEpsilonFromFile(configFile);
}

void graphLocalSearch::set_number_partitions_from_file(std::string configFile) {
    this->numberPartitions = fileUtils::getNumberPartitions(configFile);
}

void graphLocalSearch::set_distance_from_file(std::string configFile) {
    this->distances = fileUtils::readDistancesFromFile(configFile);
}

void graphLocalSearch::set_hierarchy_from_file(std::string configFile) {
    this->hierarchy = fileUtils::readHierarchyFromFile(configFile);
}

void graphLocalSearch::initialise(std::string configFile) {
    set_imbalance_from_file(configFile);
    set_number_partitions_from_file(configFile);
    set_distance_from_file(configFile);
    set_hierarchy_from_file(configFile);
}




void graphLocalSearch::determineBalance() {

    nodesPerPartition.clear();

    for (long partition_value : this->partition) {

        if( nodesPerPartition.count(partition_value) > 0 ) {
            nodesPerPartition.at(partition_value) += 1;
        } else {
            nodesPerPartition[partition_value] = 1;
        }
    }
    
    // ! this could make mistakes
    this->numberPartitions = nodesPerPartition.size() ;
}

void graphLocalSearch::determine_initial_partition(std::string configFile) {
    this->partitionWithSharedMap(configFile);
    this->dirty_vertices.clear();

    // determine how many nodes are in each partition
    determineBalance();
    initialise(configFile);
}


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


// -------------------------------- here comes the gain computation with respect to hierarchy --------------------------

void graphLocalSearch::determine_location(
                        const long partition_id,
                        std::vector<long> &loc ) {
    
    long r_start = 0;
    long r_end = numberPartitions;

    const size_t s = hierarchy.size();
    for( size_t i = 0; i < s; ++i) {

        const long n_parts = hierarchy.at(s-1-i);
        const long add = (r_end - r_start) / n_parts;

        for ( long j = 0; j < n_parts; ++j) {
            if(r_start <= partition_id && partition_id < r_start + add) {
                loc.at(s - 1 - i) = j;
                r_end = r_start + add;
                break;
            }
            r_start += add;
        }

    }
    
}


long graphLocalSearch::determine_distance(
                        const long partiton_u,
                        const long partition_v,
                        std::vector<long> &u_loc,
                        std::vector<long> &v_loc) {
    
    if (partiton_u == partition_v) {
        return 0;
    }

    determine_location(partiton_u, u_loc);
    determine_location(partition_v, v_loc);

    const long s = hierarchy.size();

    for(long i = 0; i < s; ++i) {
        if(u_loc.at(s - 1 - i) != v_loc.at(s - 1 - i)) {
            return distances.at( s - 1 - i );
        }
    }
    
    abort();

}



std::unordered_map<long, int> graphLocalSearch::compute_gains(long from_node) {

    std::unordered_map<long, int> gains;

    long from_partition = this->partition.at(from_node); 
    std::vector<long> neighbors = this->adjacency_list.at(from_node);


    // find all neighboring partitions:
    for (long neighbor : neighbors) {
        long neighbor_partition = this->partition.at(neighbor);
        if( neighbor_partition != from_partition) {
            gains[neighbor_partition] = 0;
        }
    }

    std::vector<long> partition_u(hierarchy.size());
    std::vector<long> partition_u_changed(hierarchy.size());
    std::vector<long> partition_v(hierarchy.size());


    // iterate over each partition
    // for each partition iterate over all neighbors
    for (auto& [partition_id, gain] : gains) {

        for (long neighbor : neighbors) {

            long neighbor_partition = this->partition.at(neighbor);
            
            long distance_current, distance_if_swapped;

            distance_current = determine_distance(from_partition, neighbor_partition, partition_u, partition_v );
            distance_if_swapped = determine_distance(partition_id, neighbor_partition, partition_u_changed, partition_v);

            gain += distance_current - distance_if_swapped ; 
        }
    }




    return gains;


}



// ---------------------------------------------------- end of gain computation code -----------------------------------



//TODO: remember / log migration volume
void graphLocalSearch::move_vertex(long node, long to_partition) {

    long from_partition = partition.at(node);
    
    nodesPerPartition.at(from_partition) -= 1;
    nodesPerPartition.at(to_partition) += 1;

    numberMigratedNodes += 1;

    partition.at(node) = to_partition;
}

bool graphLocalSearch::canMove(long from_partition, long to_partition) {

    if (numberPartitions == 0) {
        std::cerr << "Partitionsvalue not set. aborting" << std::endl;
        return -1;
    }

    bool allowed = true;

    int nodeCount = this->getNumberNodes();

    double upperBound = (nodeCount / numberPartitions) * (1 + imbalance);


    if ( nodesPerPartition.at(to_partition) + 1 > upperBound) {
        allowed = false;
    }


    return allowed;
}


void graphLocalSearch::repartition() {

    // look at all dirty vertices and see if you want to move
    // them to another partition


    for (const auto& vertex : dirty_vertices) {

        int gain_best = 0;
        int best_partition = 0;

        std::unordered_map<long, int> gains = compute_gains(vertex);
        
        
        for (const auto& [partition_id, gain] : gains) {
            if (gain > gain_best) {
                gain_best = gain;
                best_partition = partition_id;
            }
        }

        if( gain_best > 0 && canMove(partition.at(vertex), best_partition)) {
            move_vertex(vertex, best_partition);
        }

    }


    dirty_vertices.clear();

}