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
        double speed_sharedMap;
        int countMigratedNodes; // gibt die anzahl an nodes an, die ihre partition gewechslet haben
        double migrationCost; // gibt gleich prozentuel an wieviel prozent der nodes migriert wurden beim repartitionieren

        
        // die Hilfsmethoden muss ich nicht nach außen sichtbar machen
        std::vector<std::vector<int>> createSimilarityMatrix(const std::vector<long>& old_partition, const std::vector<long>& new_partition, int k);
        std::vector<std::tuple<int, int>> heuristicAssignment(const std::vector<std::vector<int>>& simMatrix);

        
        // this takes in the simMatrix of the leaf-level and the final assignment (of leafs / old partitions to new ones)
        void determineMigrationCost(const std::vector<std::vector<int>>& simMatrix, const std::vector<std::tuple<int,int>>& matching);

    public:

        graphRFS(long node_count) : Graph(node_count) { 
            comm_cost_sharedMap = 0; 
            speed_sharedMap = 0;
            countMigratedNodes = 0;
            migrationCost = 0;
        }
        void repartition(std::string configFile) override;

        int get_baseline_comm_cost() {return comm_cost_sharedMap;}
        double get_baseline_speed() {return speed_sharedMap;}
        double get_migrationCost() {return migrationCost;}

};

#endif