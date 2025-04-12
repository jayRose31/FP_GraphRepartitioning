#ifndef GRAPHRFSMULTILEVEL_H
#define GRAPHRFSMULTILEVEL_H

#include "graph.hh"
#include <vector>
#include <tuple>

class graphRFSMultilevel : public Graph {
    private:
    
    std::vector<std::vector<int>> createSimilarityMatrix(const std::vector<long>& old_partition, const std::vector<long>& new_partition, int k);
    std::vector<std::tuple<int, int>> heuristicAssignment(const std::vector<std::vector<int>>& simMatrix);
    
    //TODO: implement hungarian algorithm to get optimal matching
    //? maybe this will improve my results...
    std::vector<std::tuple<int, int>> optimalMatching(const std::vector<std::vector<int>>& simMatrix);

    std::vector<std::vector<int>> shrinkMatrix(std::vector<std::vector<int>> simMatrix, int hierarchy  );
    std::vector< std::vector<std::vector<int>> > createAllSimMatrices(std::vector<std::vector<int>> simMatrix, std::vector<int> hierarchyArray);
    
    std::vector< std::vector<std::vector<int>> > createSubMatrices(std::vector<std::vector<int>> simMatrix, std::vector<std::tuple<int, int>> matching, int hierarchy);
    std::vector<std::tuple<int, int>> shiftMatchingIndex(std::tuple<int, int> matching_originalMatrix , std::vector<std::tuple<int, int>> matching_subMatrix, int hierarchy) ;

    std::vector<std::tuple<int, int>> matchTopToBottom(std::vector< std::vector<std::vector<int>> >  matrixHierarchy, std::vector<int> hierarchyArray) ;

    public:
        graphRFSMultilevel(long node_count) : Graph(node_count) {}
        void repartition(std::string configFile) override;

        void test();

};


#endif