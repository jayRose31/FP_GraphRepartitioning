#ifndef GRAPHLOCALSEARCH
#define GRAPHLOCALSEARCH

#include "graph.hh"

#include <unordered_set>
#include <string>


class graphLocalSearch : public Graph {

    private:
        std::unordered_set<long> dirty_vertices;
        void mark_dirty(long vertex);

        // this will only compute gains from the number of cut edges
        std::unordered_map<long, int> compute_gains_simplified(long from_node);

    public:

        graphLocalSearch(long node_count) : Graph(node_count) {}

        void repartition(std::string configFile) override {this->repartition();};
        void repartition();
        // brauche hier eine eigene funktion insert edge
        // als wrapper mit extra funktionalität

        void add_edge(long start_node, long destination_node) override;
        void remove_edge(long start_node, long destination_node) override;

        void print_dirty_vertices();


};



#endif