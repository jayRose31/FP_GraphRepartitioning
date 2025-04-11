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


void graphRFSMultilevel::test() {
    // Create a sparse matrix of dimension 8x8 with some entries 0 and some entries 1
    std::vector<std::vector<int>> sparseMatrix(8, std::vector<int>(8, 0));
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



    std::vector<std::vector<int>> shrunkenMatrix = shrinkMatrix(sparseMatrix, 2);

    std::cout << std::endl;
        // Print the sparse matrix
        for (const auto& row : shrunkenMatrix) {
            for (const auto& val : row) {
                std::cout << val << " ";
            }
            std::cout << std::endl;
        }
    

    return;

}

void graphRFSMultilevel::repartition(std::string configFile){

    return;
}