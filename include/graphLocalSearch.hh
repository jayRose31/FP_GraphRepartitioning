#ifndef GRAPHLOCALSEARCH
#define GRAPHLOCALSEARCH

#include "graph.hh"

#include <unordered_set>
#include <unordered_map>
#include <string>

class graphLocalSearch : public Graph {

    private:
        bool initialized;

        float imbalance;
        long numberPartitions;
        std::vector<int> distances;
        std::vector<int> hierarchy;
        std::unordered_map<long, long> nodesPerPartition;

        double numberMigratedNodes;

        std::unordered_set<long> dirty_vertices;


        void mark_dirty(long vertex);

        // this will only compute gains from the number of cut edges
        //! maybe change to long long
        std::unordered_map<long, int> compute_gains_simplified(long from_node);

        // this will compute gains with respect to the hierarchy
        std::unordered_map<long, int> compute_gains(long from_node);

        void determineBalance();
        
        bool canMove(long from_partition, long to_partition);
        void move_vertex(long node, long to_partition);

        void determine_location(const long partition_id, std::vector<long> &loc );
        long determine_distance(const long partiton_u, const long partition_v, std::vector<long> &u_loc, std::vector<long> &v_loc);

        // i make them private so the user can only set them
        // via the initialise function
        void set_imbalance_from_file(std::string configFile);
        void set_number_partitions_from_file(std::string configFile);
        void set_distance_from_file(std::string configFile);
        void set_hierarchy_from_file(std::string configFile);

    public:

        graphLocalSearch(long node_count) : Graph(node_count) {
            initialized = false;
            numberMigratedNodes = 0.0;
            imbalance = 0;
            numberPartitions = 0;
        }

        double get_migrationCost() {return (double)(numberMigratedNodes / this->adjacency_list.size());}
        
        void initialise(std::string configFile);
        void determine_initial_partition(std::string configFile);

        void repartition(std::string configFile) override {this->repartition();};
        void repartition();

        void add_edge(long start_node, long destination_node) override;
        void remove_edge(long start_node, long destination_node) override;

        void print_dirty_vertices();

};



#endif