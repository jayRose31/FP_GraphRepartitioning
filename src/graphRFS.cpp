#include <iostream>
#include <vector>
#include <string>
#include <tuple>

#include "../include/graphRFS.hh"
#include "../include/fileUtils.hh"
#include "../third_party/SharedMap/include/libsharedmap.h"

std::vector<std::vector<int>> graphRFS::createSimilarityMatrix(const std::vector<long>& old_partition, const std::vector<long>& new_partition, int k) {
    
    std::vector<std::vector<int>> simMatrix(k, std::vector<int>(k, 0));
    
    for (size_t i=0; i < old_partition.size(); i++) {
        simMatrix[old_partition[i]][new_partition[i]]++;
    }

    return simMatrix;
}


std::vector<std::tuple<int, int>> graphRFS::heuristicAssignment(const std::vector<std::vector<int>>& simMatrix) {
    
    //1. Erstelle sortierte Edgelist anhand der Similarity Matrix

    // edges implizit dargestellt als tuple< startNode, endNode, similarity>
    // wobei startNode aus der alten Partition und endNode aus der neuen Partition kommt
    std::vector<std::tuple<int, int, int>> edgeList;

    for (size_t i = 0; i < simMatrix.size(); i++) {
        for (size_t j = 0; j < simMatrix.size(); j++) {
            //TODO: nochmal händisch prüfen ob das so stimmt
            edgeList.push_back(std::make_tuple(i, j, simMatrix[i][j]));
        }
    }
    // Sort edgeList in descending order based on the similarity value (third element of the tuple)
    std::sort(edgeList.begin(), edgeList.end(), [](const std::tuple<int, int, int>& a, const std::tuple<int, int, int>& b) {
        return std::get<2>(a) > std::get<2>(b);
    });


    //2. Gehe durch die Edgelist durch und probiere die Knoten zu matchen
    // d.h. ich brauch noch eine DS um zu speichern welche Knoten schon gematched sind
    // und eine für die tatsächlichen assignments
    std::vector<bool> oldPartitionMatched(simMatrix.size(), false);
    std::vector<bool> newPartitionMatched(simMatrix.size(), false);
    //! explizite darstellung der assignments als tuple<olPartition, newPartition>
    //TODO: eine implizite Darstellung hinzufügen für speed
    std::vector<std::tuple<int, int>> assignments;

    for(const auto& edge : edgeList) {
        int oldPartition = std::get<0>(edge);
        int newPartition = std::get<1>(edge);

        if (!oldPartitionMatched[oldPartition] && !newPartitionMatched[newPartition]) {
            assignments.push_back(std::make_tuple(oldPartition, newPartition));
            oldPartitionMatched[oldPartition] = true;
            newPartitionMatched[newPartition] = true;
        }
    }

    return assignments;

}



void graphRFS::repartition() {
    
    //TODO eventuell auslagern in die Klasse
    std::string configFile = "./res/sharedMapConfigs/sharedMap_config1.json";
    
    // 1. Berechne neue Partition mit shared map
    
    std::vector<long> old_partition = this->partition;
    this->partitionWithSharedMap(configFile);
    std::vector<long> new_partition = this->partition;
 

    // 2. erstelle similarity matrix
    int k = fileUtils::getNumberPartitions(configFile);
    std::vector<std::vector<int>> simMatrix =  this->createSimilarityMatrix(old_partition, new_partition, k);

    // Print the similarity matrix
    std::cout << "Similarity Matrix:" << std::endl;
    for (const auto& row : simMatrix) {
        for (const auto& value : row) {
            std::cout << value << " ";
        }
        std::cout << std::endl;
    }

    
    // 3. Löse assignment problem
    std::vector<std::tuple<int, int>> matching = this->heuristicAssignment(simMatrix);



    // Order the matching entries by the first value of the tuple
    std::sort(matching.begin(), matching.end(), [](const std::tuple<int, int>& a, const std::tuple<int, int>& b) {
        return std::get<0>(a) < std::get<0>(b);
    });

    // Print all matching values
    std::cout << "Matching:" << std::endl;
    for (const auto& match : matching) {
        std::cout << "(" << std::get<0>(match) << ", " << std::get<1>(match) << ")" << std::endl;
    }

    // 4. Permutiere die Partition
    
}