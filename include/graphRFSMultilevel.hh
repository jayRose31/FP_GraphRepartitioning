#ifndef GRAPHRFSMULTILEVEL_H
#define GRAPHRFSMULTILEVEL_H

#include "graph.hh"
#include <vector>
#include <tuple>

class graphRFSMultilevel : public Graph {
    private:
    // save the value determined by sharedMap when computing initial repartition as baseline value
    int comm_cost_sharedMap;

    //TODO: same for speed
    
    std::vector<std::vector<int>> createSimilarityMatrix(const std::vector<long>& old_partition, const std::vector<long>& new_partition, int k);
    std::vector<std::tuple<int, int>> heuristicAssignment(const std::vector<std::vector<int>>& simMatrix);
    std::vector<std::tuple<int, int>> optimalMatching(const std::vector<std::vector<int>>& simMatrix);

    std::vector<std::vector<int>> shrinkMatrix(std::vector<std::vector<int>> simMatrix, int hierarchy  );
    std::vector< std::vector<std::vector<int>> > createAllSimMatrices(std::vector<std::vector<int>> simMatrix, std::vector<int> hierarchyArray);
    
    std::vector< std::vector<std::vector<int>> > createSubMatrices(std::vector<std::vector<int>> simMatrix, std::vector<std::tuple<int, int>> matching, int hierarchy);
    std::vector<std::tuple<int, int>> shiftMatchingIndex(std::tuple<int, int> matching_originalMatrix , std::vector<std::tuple<int, int>> matching_subMatrix, int hierarchy) ;

    std::vector<std::tuple<int, int>> matchTopToBottom(std::vector< std::vector<std::vector<int>> >  matrixHierarchy, std::vector<int> hierarchyArray) ;

    public:
        graphRFSMultilevel(long node_count) : Graph(node_count) {comm_cost_sharedMap = 0;}
        void repartition(std::string configFile) override;

        int get_baseline_comm_cost() {return comm_cost_sharedMap;}

        void test();

};


#endif