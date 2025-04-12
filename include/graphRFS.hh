#ifndef GRAPHRFS_H
#define GRAPHRFS_H

#include "graph.hh"
#include <tuple>
#include <string>


/*
RFS := Repartition From Scratch.
Die Strategie ist also:
1. Berechne neue Partition
2. Finde migration minimal mapping 
3. Permutiere die neu berechnete Partition
*/

class graphRFS : public Graph {
    private:
        // save the value determined by sharedMap when computing initial repartition as baseline value
        int comm_cost_sharedMap;

        //TODO: same for speed
        
        // die Hilfsmethoden muss ich nicht nach außen sichtbar machen
        std::vector<std::vector<int>> createSimilarityMatrix(const std::vector<long>& old_partition, const std::vector<long>& new_partition, int k);
        std::vector<std::tuple<int, int>> heuristicAssignment(const std::vector<std::vector<int>>& simMatrix);

    public:

        graphRFS(long node_count) : Graph(node_count) { comm_cost_sharedMap = 0; }
        void repartition(std::string configFile) override;

        int get_baseline_comm_cost() {return comm_cost_sharedMap;}

};

#endif