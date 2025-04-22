#include <iostream>
#include <vector>
#include <string>
#include <tuple>

#include "../include/graphRFS.hh"
#include "../include/graph_io.hh"
#include "../include/fileUtils.hh"
#include "../third_party/SharedMap/include/libsharedmap.h"

std::vector<std::vector<int>> graphRFS::createSimilarityMatrix(const std::vector<long>& old_partition, const std::vector<long>& new_partition, int k) {
    
    std::vector<std::vector<int>> simMatrix(k, std::vector<int>(k, 0));
    
    for (size_t i=0; i < old_partition.size(); i++) {
        simMatrix.at(old_partition.at(i)).at(new_partition.at(i))++;
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


void graphRFS::determineMigrationCost(const std::vector<std::vector<int>>& simMatrix, const std::vector<std::tuple<int,int>>& matching) {

    
    int sum_unmoved_vertices = 0;

    for(auto match : matching) {
        sum_unmoved_vertices += simMatrix.at(std::get<0>(match)).at(std::get<1>(match));
    }

    double sum_simMatrix = 0.0;
    for (const auto& row : simMatrix) {
        sum_simMatrix += std::accumulate(row.begin(), row.end(), 0);
    }
    
    this->countMigratedNodes = sum_simMatrix- sum_unmoved_vertices;
    this->migrationCost = this->countMigratedNodes / sum_simMatrix;

    //this->countMigratedNodes = this->getNumberNodes() - sum_unmoved_vertices;
}


const int INF = std::numeric_limits<int>::max();

std::vector<std::tuple<int, int>> graphRFS::optimalMatching(const std::vector<std::vector<int>>& A) {
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



void graphRFS::repartition(std::string configFile) {
    

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



    
    // 3. Löse assignment problem
    std::vector<std::tuple<int, int>> matching = this->optimalMatching(simMatrix);
    // std::vector<std::tuple<int, int>> matching = this->heuristicAssignment(simMatrix);


        // Write the similarity matrix and matching to the same file
        std::ofstream outputFile("./experiments/singleLevel_similarity_matrix_and_matching.txt");
        if (outputFile.is_open()) {
            // Write the similarity matrix
            outputFile << "Similarity Matrix:\n";
            for (const auto& row : simMatrix) {
            for (const auto& value : row) {
                outputFile << value << " ";
            }
            outputFile << "\n";
            }

            // Write the matching
            outputFile << "\nMatching:\n";
            for (const auto& match : matching) {
            outputFile << std::get<0>(match) << " " << std::get<1>(match) << "\n";
            }

            outputFile.close();
        } else {
            std::cerr << "Unable to open file to write similarity matrix and matching.\n";
        }



    // 4. Permutiere die Partition
    std::vector<int> implicit_matching = std::vector<int>(matching.size());
    for(const auto& match : matching) {
        implicit_matching[std::get<1>(match)] = std::get<0>(match);
    }
    
    std::vector<long> partition_permuted;
    for (const auto& value : new_partition) {
        partition_permuted.push_back(implicit_matching.at(value));
    }


    // 5. Setze die neue Partition
        
    this->setPartition(partition_permuted);    

    determineMigrationCost(simMatrix, matching);


    return;


}