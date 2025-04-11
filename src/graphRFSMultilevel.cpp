#include <iostream>
#include <vector>
#include <string>
#include <tuple>
#include <cassert>
#include <cmath>
#include <numeric>

#include "../include/graphRFSMultilevel.hh"
#include "../include/graph_io.hh"
#include "../include/fileUtils.hh"
#include "../third_party/SharedMap/include/libsharedmap.h"


std::vector<std::vector<int>> graphRFSMultilevel::createSimilarityMatrix(const std::vector<long>& old_partition, const std::vector<long>& new_partition, int k) {
    
    std::vector<std::vector<int>> simMatrix(k, std::vector<int>(k, 0));
    
    for (size_t i=0; i < old_partition.size(); i++) {
        simMatrix[old_partition[i]][new_partition[i]]++;
    }

    return simMatrix;
}


std::vector<std::tuple<int, int>> graphRFSMultilevel::heuristicAssignment(const std::vector<std::vector<int>>& simMatrix) {
    
    //1. Erstelle sortierte Edgelist anhand der Similarity Matrix

    // edges implizit dargestellt als tuple< startNode, endNode, similarity>
    // wobei startNode aus der alten Partition und endNode aus der neuen Partition kommt
    std::vector<std::tuple<int, int, int>> edgeList;

    for (size_t i = 0; i < simMatrix.size(); i++) {
        for (size_t j = 0; j < simMatrix.size(); j++) {
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

    size_t matchCount = 0;
    for(const auto& edge : edgeList) {
        int oldPartition = std::get<0>(edge);
        int newPartition = std::get<1>(edge);

        if (!oldPartitionMatched[oldPartition] && !newPartitionMatched[newPartition]) {
            assignments.push_back(std::make_tuple(oldPartition, newPartition));
            oldPartitionMatched[oldPartition] = true;
            newPartitionMatched[newPartition] = true;
            matchCount++;
        }
        
        // quit early 
        if (matchCount >= simMatrix.size()) {
            break;
        }
    }

    return assignments;

}


std::vector<std::vector<int>> graphRFSMultilevel::shrinkMatrix(std::vector<std::vector<int>> simMatrix, int hierarchy  ) {

    int n = simMatrix.size();
    int k = int(n / hierarchy) ;


    std::vector<std::vector<int>> shrunkenMatrix(k, std::vector<int>(k, 0));
    // create shrunkenMatrix by adding the values of collapsing rows and collumns
    for(int i=0; i < n ; i++) {
        for(int j=0; j < n; j++) {
            int newRow = std::floor(i / hierarchy);
            int newCol = std::floor(j / hierarchy);
            shrunkenMatrix.at(newRow).at(newCol) += simMatrix.at(i).at(j);
        }
    }


    // Verify that the sum of elements is preserved
    int originalSum = 0;
    for (const auto& row : simMatrix) {
        originalSum += std::accumulate(row.begin(), row.end(), 0);
    }

    int shrunkenSum = 0;
    for (const auto& row : shrunkenMatrix) {
        shrunkenSum += std::accumulate(row.begin(), row.end(), 0);
    }

    assert(originalSum == shrunkenSum && "Sum of elements is not preserved during shrinking.");


    return shrunkenMatrix;
}



std::vector< std::vector<std::vector<int>> >  graphRFSMultilevel::createAllSimMatrices(std::vector<std::vector<int>> simMatrix, std::vector<int> hierarchyArray) {

    std::vector< std::vector<std::vector<int>> > all_matrices = {};
    all_matrices.push_back(simMatrix);

    // skip the last value of the array
    for(int i = 0; i < hierarchyArray.size() - 1; i++) {
        std::vector<std::vector<int>> matrix = shrinkMatrix(all_matrices.back(), hierarchyArray.at(i));
        all_matrices.push_back(matrix);
    }

    return all_matrices;
}


std::vector< std::vector<std::vector<int>> > graphRFSMultilevel::createSubMatrices(std::vector<std::vector<int>> simMatrix, std::vector<std::tuple<int, int>> matching, int hierarchy) {
    
    int n = simMatrix.size();
    int k = (int)(n / hierarchy);

    assert(simMatrix.size() == simMatrix[0].size() && "Matrix must be square.");
    assert(simMatrix.size() % hierarchy == 0 && "Matrix dimensions must be divisible by the hierarchy value.");

    std::vector< std::vector<std::vector<int>> > matrices = {};
    
    std::vector<std::vector<int>> sub_arrays;
    for (int i = 0; i < k; ++i) {
        std::vector<int> sub_array;
        for (int j = i * hierarchy; j < (i + 1) * hierarchy; ++j) {
            sub_array.push_back(j);
        }
        sub_arrays.push_back(sub_array);
    }

    for (const std::tuple<int, int>& match : matching) {
        int i = std::get<0>(match);
        int j = std::get<1>(match);

        std::vector<std::vector<int>> sub_matrix(hierarchy, std::vector<int>(hierarchy, 0));
        for (int row : sub_arrays[i]) {
            for (int col : sub_arrays[j]) {
                sub_matrix[row % hierarchy][col % hierarchy] = simMatrix[row][col];
            }
        }
        matrices.push_back(sub_matrix);
    }

    return matrices;
}




void graphRFSMultilevel::test() {
    // Create a sparse matrix of dimension 8x8 with some entries 0 and some entries 1
    std::vector<std::vector<int>> sparseMatrix(9, std::vector<int>(9, 0));
    // Set one third of the entries to 1
    int n = sparseMatrix.size();
    int totalEntries = n * n;
    int onesToSet = totalEntries / 3;

    int count = 0;
    for (int i = 0; i < n && count < onesToSet; ++i) {
        for (int j = 0; j < n && count < onesToSet; ++j) {
            if ((i + j) % 2 == 0) { // Example pattern to distribute 1s
                sparseMatrix[i][j] = 1;
                ++count;
            }
        }
    }

    // Print the sparse matrix
    for (const auto& row : sparseMatrix) {
        for (const auto& val : row) {
            std::cout << val << " ";
        }
        std::cout << std::endl;
    }


    std::vector<int> hierarchyArray = {3,3,2};
    std::vector< std::vector<std::vector<int>> > matrices = createAllSimMatrices(sparseMatrix, hierarchyArray);

    std::vector<std::vector<int>> shrunkenMatrix = matrices[1];

    
    std::cout << "first matrix: " << std::endl;
        // Print the sparse matrix
        for (const auto& row : shrunkenMatrix) {
            for (const auto& val : row) {
                std::cout << val << " ";
            }
            std::cout << std::endl;
        }
    
    
        shrunkenMatrix = matrices[2];

    
        std::cout << "second matrix: " << std::endl;
            // Print the sparse matrix
            for (const auto& row : shrunkenMatrix) {
                for (const auto& val : row) {
                    std::cout << val << " ";
                }
                std::cout << std::endl;
            }


    std::vector<std::tuple<int, int>> matching = {std::make_tuple(0, 0), std::make_tuple(0, 1), std::make_tuple(2, 1) };

    std::vector< std::vector<std::vector<int>> > sub_matrices = createSubMatrices(matrices[0], matching, 3);
    std::cout << "Sub matrices: " << std::endl;
    for (size_t idx = 0; idx < sub_matrices.size(); ++idx) {
        std::cout << "Sub matrix " << idx + 1 << ":" << std::endl;
        for (const auto& row : sub_matrices[idx]) {
            for (const auto& val : row) {
                std::cout << val << " ";
            }
            std::cout << std::endl;
        }
        std::cout << std::endl;
    }


    return;

}

void graphRFSMultilevel::repartition(std::string configFile){




    return;
}