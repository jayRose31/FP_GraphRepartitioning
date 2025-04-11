#ifndef GRAPHRFSMULTILEVEL_H
#define GRAPHRFSMULTILEVEL_H

#include "graph.hh"
#include <vector>

class graphRFSMultilevel : public Graph {
    private:

    std::vector<std::vector<int>> shrinkMatrix(std::vector<std::vector<int>> simMatrix, int hierarchy  );
    std::vector< std::vector<std::vector<int>>  > createAllSimMatrices(std::vector<std::vector<int>> simMatrix, std::vector<int> hierarchyArray);


    public:
        graphRFSMultilevel(long node_count) : Graph(node_count) {}
        void repartition(std::string configFile) override;

        void test();

};


#endif