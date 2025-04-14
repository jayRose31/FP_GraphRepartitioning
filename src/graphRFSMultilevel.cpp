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

std::vector<std::tuple<int, int>> graphRFSMultilevel::shiftMatchingIndex(std::tuple<int, int> matching_originalMatrix , std::vector<std::tuple<int, int>> matching_subMatrix, int hierarchy) {

    std::vector<std::tuple<int, int>> matchingShifted = {};

    for(std::tuple<int, int>& match : matching_subMatrix) {
        int i = std::get<0>(match) + std::get<0>(matching_originalMatrix) * hierarchy ;
        int j = std::get<1>(match) + std::get<1>(matching_originalMatrix) * hierarchy ;
        matchingShifted.push_back(std::make_tuple(i, j));
    }

    return matchingShifted;
}


std::vector<std::tuple<int, int>>  graphRFSMultilevel::matchTopToBottom(std::vector< std::vector<std::vector<int>> >  matrixHierarchy, std::vector<int> hierarchyArray) {

    //std::vector<std::tuple<int, int>> match0 = heuristicAssignment(matrixHierarchy.back());
    std::vector<std::tuple<int, int>> match0 = optimalMatching(matrixHierarchy.back());
    matrixHierarchy.pop_back();
    hierarchyArray.pop_back();

    std::vector<std::vector<std::tuple<int, int>>> subMatrixMatching_All = {};
    while (!matrixHierarchy.empty()) {
        
        std::vector< std::vector<std::vector<int>> > subMatrices = createSubMatrices(matrixHierarchy.back(), match0, hierarchyArray.back());
        
        std::vector<std::tuple<int, int>> subMatrixMatching;
        std::vector<std::tuple<int, int>> MatchingShifted;
        
        for (int i = 0; i < subMatrices.size(); i++) {
            // subMatrixMatching = heuristicAssignment(subMatrices.at(i));
            subMatrixMatching = optimalMatching(subMatrices.at(i));
            MatchingShifted = shiftMatchingIndex(match0.at(i), subMatrixMatching, hierarchyArray.back());
            subMatrixMatching_All.push_back(MatchingShifted);
        }

        match0.clear();
        for (const auto& subMatrixMatching : subMatrixMatching_All) {
            match0.insert(match0.end(), subMatrixMatching.begin(), subMatrixMatching.end());
        }
        subMatrixMatching_All.clear();
        matrixHierarchy.pop_back();
        hierarchyArray.pop_back();
    }

    return match0;
}

const int INF = std::numeric_limits<int>::max();

// A is our similarity matrix
std::vector<std::tuple<int, int>> graphRFSMultilevel::optimalMatching(const std::vector<std::vector<int>>& A) {
    int n = A.size();
    std::vector<std::vector<int>> cost = A;

    // Negate the cost matrix for max-weight matching
    int max_value = -INF;
    for (const auto& row : cost)
        for (int val : row)
            max_value = std::max(max_value, val);

    for (int i = 0; i < n; ++i)
        for (int j = 0; j < n; ++j)
            cost[i][j] = max_value - cost[i][j];

    std::vector<int> u(n + 1), v(n + 1), p(n + 1), way(n + 1);

    for (int i = 1; i <= n; ++i) {
        p[0] = i;
        std::vector<int> minv(n + 1, INF);
        std::vector<bool> used(n + 1, false);
        int j0 = 0;

        do {
            used[j0] = true;
            int i0 = p[j0], delta = INF, j1 = -1;
            for (int j = 1; j <= n; ++j) {
                if (!used[j]) {
                    int cur = cost[i0 - 1][j - 1] - u[i0] - v[j];
                    if (cur < minv[j]) {
                        minv[j] = cur;
                        way[j] = j0;
                    }
                    if (minv[j] < delta) {
                        delta = minv[j];
                        j1 = j;
                    }
                }
            }
            for (int j = 0; j <= n; ++j) {
                if (used[j]) {
                    u[p[j]] += delta;
                    v[j] -= delta;
                } else {
                    minv[j] -= delta;
                }
            }
            j0 = j1;
        } while (p[j0] != 0);

        do {
            int j1 = way[j0];
            p[j0] = p[j1];
            j0 = j1;
        } while (j0);
    }

    std::vector<std::tuple<int, int>> result(n);
    for (int j = 1; j <= n; ++j)
        result[p[j] - 1] = std::make_tuple(p[j] - 1, j - 1);

    return result;    
}





void graphRFSMultilevel::test() {
    // Create a sparse matrix of dimension 8x8 with some entries 0 and some entries 1
    
    std::vector<std::vector<int>> sparseMatrix(8, std::vector<int>(8, 0));
    
    sparseMatrix = {
        {1, 0, 1, 0, 0, 1, 2, 2},
        {0, 1, 0, 1, 0, 0, 1, 0},
        {1, 0, 1, 0, 1, 0, 0, 1},
        {0, 1, 0, 1, 0, 1, 0, 0},
        {0, 0, 1, 0, 1, 0, 1, 0},
        {1, 0, 0, 1, 0, 1, 0, 1},
        {0, 1, 0, 0, 1, 0, 1, 0},
        {0, 5, 1, 0, 0, 1, 0, 1}
        };
        
        /*
        std::vector<std::vector<int>> sparseMatrix(9, std::vector<int>(9, 0));
        for (int j = 0; j < 9; ++j) {
            for (int i = 0; i < 9; ++i) {
                sparseMatrix[j][i] = i + j * 9 + 1;
            }
        }
        
        */
    /*
    */
    sparseMatrix = {
        {9, 0, 1, 0, 0, 1, 2, 2},
        {0, 1, 10, 1, 0, 0, 1, 0},
        {1, 0, 1, 0, 1, 10, 0, 1},
        {0, 1, 0, 1, 0, 1, 10, 0},
        {0, 0, 1, 0, 1, 0, 1, 10},
        {1, 0, 0, 1, 10, 1, 0, 1},
        {0, 1, 0, 10, 1, 0, 1, 0},
        {0, 5, 1, 0, 0, 1, 0, 1}
    };
    
    
   

    // Print the sparse matrix
    for (const auto& row : sparseMatrix) {
        for (const auto& val : row) {
            std::cout << val << " ";
        }
        std::cout << std::endl;
    }


    std::vector<int> hierarchyArray = {2,2,2};
    std::vector< std::vector<std::vector<int>> > matrices = createAllSimMatrices(sparseMatrix, hierarchyArray);

    std::vector<std::tuple<int, int>> matching = matchTopToBottom(matrices, hierarchyArray);
    // Print the matching
    std::cout << "Matching:" << std::endl;
    for (const auto& match : matching) {
        std::cout << "(" << std::get<0>(match) << ", " << std::get<1>(match) << ")" << std::endl;
    }



    return;

}

void graphRFSMultilevel::repartition(std::string configFile){

       // 1. Berechne neue Partition mit shared map
    
       std::vector<long> old_partition = this->partition;
        
       auto start = std::chrono::high_resolution_clock::now();
       this->comm_cost_sharedMap = this->partitionWithSharedMap(configFile);
       auto end = std::chrono::high_resolution_clock::now();
       std::chrono::duration<double> elapsed = end - start;
    
        this->speed_sharedMap = elapsed.count();

       std::vector<long> new_partition = this->partition;
    
   
       // 2. erstelle similarity matrix
       int k = fileUtils::getNumberPartitions(configFile);
       std::vector<std::vector<int>> simMatrix =  this->createSimilarityMatrix(old_partition, new_partition, k);
   
       // 3. mache hierarchisches mapping.
       std::vector<int> hierarchyArray = fileUtils::readHierarchyFromFile(configFile);

       std::vector< std::vector<std::vector<int>> > matrices = createAllSimMatrices(simMatrix, hierarchyArray);
   
       std::vector<std::tuple<int, int>> matching = matchTopToBottom(matrices, hierarchyArray);


       // 4. Setze die neue Partition

       std::vector<int> implicit_matching = std::vector<int>(matching.size());
        for(const auto& match : matching) {
            implicit_matching[std::get<1>(match)] = std::get<0>(match);
        }
        
        std::vector<long> partition_permuted;
        for (const auto& value : new_partition) {
            partition_permuted.push_back(implicit_matching.at(value));
        }

        this->setPartition(partition_permuted);
        
    


    return;


    return;
}