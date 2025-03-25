#ifndef GRAPHRFS_H
#define GRAPHRFS_H

#include "graph.hh"
#include <tuple>


/*
RFS := Repartition From Scratch.
Die Strategie ist also:
1. Berechne neue Partition
2. Finde migration minimal mapping 
3. Permutiere die neu berechnete Partition
*/

class graphRFS : public Graph {
    public:
        graphRFS(long node_count) : Graph(node_count) {}
        void repartition() override;

        std::vector<std::vector<int>> createSimilarityMatrix(const std::vector<long>& old_partition, const std::vector<long>& new_partition, int k);
        std::vector<std::tuple<int, int>> heuristicAssignment(const std::vector<std::vector<int>>& simMatrix);
};

#endif