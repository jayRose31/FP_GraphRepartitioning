#ifndef GRAPHRFSMULTILEVEL_H
#define GRAPHRFSMULTILEVEL_H

#include "graph.hh"
#include <vector>
#include <tuple>

class graphRFSMultilevel : public Graph {
    private:
    
    std::vector<std::vector<int>> createSimilarityMatrix(const std::vector<long>& old_partition, const std::vector<long>& new_partition, int k);
    std::vector<std::tuple<int, int>> heuristicAssignment(const std::vector<std::vector<int>>& simMatrix);

    std::vector<std::vector<int>> shrinkMatrix(std::vector<std::vector<int>> simMatrix, int hierarchy  );
    std::vector< std::vector<std::vector<int>> > createAllSimMatrices(std::vector<std::vector<int>> simMatrix, std::vector<int> hierarchyArray);
    
    std::vector< std::vector<std::vector<int>> > createSubMatrices(std::vector<std::vector<int>> simMatrix, std::vector<std::tuple<int, int>> matching, int hierarchy);
    // func match
    // return type is matching  shiftIndexMatching();


    public:
        graphRFSMultilevel(long node_count) : Graph(node_count) {}
        void repartition(std::string configFile) override;

        void test();

};


#endif