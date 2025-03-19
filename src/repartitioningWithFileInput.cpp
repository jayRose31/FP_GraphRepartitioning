#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <string> 
#include "../include/graph.hh"
#include "../include/graph_io.hh"
#include "../include/fileUtils.hh"

#include "../third_party/SharedMap/include/libsharedmap.h"

/*
 Hier soll der Nutzer das Programm aufrufen zusammen mit 
 einem Graphen und einer Konfigurationsdatei.
 also: ./repartitioningWithFileInput  <graph_file> <config_file>
*/
int main(int argc, char* argv[]) {
    
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <graph_file> <config_file>" << std::endl;
        return 1;
    }

    std::string filename = argv[1];
    std::string configFile = argv[2];


    std::ifstream file(filename); 
    if (!file) {
        std::cerr << "Fehler beim Öffnen der GraphenDatei: " << filename << std::endl;
        return 1;
    }

    std::vector<long> edge;

    std::vector<long> numberNodesAndUpdates = fileUtils::readNumberNodesAndEdgesFromFile(file);
    std::cout << "Number of Nodes: " << numberNodesAndUpdates[0] << std::endl;
    std::cout << "Number of Updates: " << numberNodesAndUpdates[1] << std::endl;

    // Erstelle den Graphen
    Graph g(numberNodesAndUpdates[0]);

    // Kanten einlesen
    for(int i = 0; i < numberNodesAndUpdates[1]; i++) {
        edge = fileUtils::readEdgeInformationFromFile(file);
        
        if (edge[0] == -1) {
            // Fehler oder Dateiende erreicht
            continue;
        } else if(edge[0] == 1) {
            g.add_edge(edge[1], edge[2]);
        } else if(edge[0] == 0) {
            g.remove_edge(edge[1], edge[2]);
        }
        
    }


    // fertig
    file.close();



    //1. Graph in CSR Format umwandeln

    CSR csr = g.convertToCSR();
    int n = g.getNumberNodes();
    
    std::vector<int> v_weights_vector(n, 1);
    int* v_weights = v_weights_vector.data();


    //2. Die anderen Paramter setzen 
    std::vector<int> hierarchy;
    std::vector<int> distance;
    int l;
    float imbalance;
    int   n_threads;
    int   seed;
    shared_map_strategy_type_t  strategy;                    
    shared_map_algorithm_type_t parallel_alg ;
    shared_map_algorithm_type_t serial_alg;
    bool verbose_error;
    bool verbose_statistics;
   
    bool configSucces = fileUtils::readConfigFileSharedMap(configFile, hierarchy, distance, l, imbalance, n_threads, seed, strategy, parallel_alg, serial_alg, verbose_error, verbose_statistics);

    if(!configSucces) {
        std::cerr << "Fehler beim Einlesen der Konfigurationsdatei: " << configFile << std::endl;
        return 1;
    }


    int comm_cost;
    int partition[n];
    

    // do the actual hierarchical multisection
    shared_map_hierarchical_multisection(n, v_weights, csr.adj_ptrs.data() ,  csr.adj_weights.data() , csr.adj.data(), hierarchy.data(), distance.data(), l, imbalance, n_threads, seed, strategy, parallel_alg, serial_alg, comm_cost, partition, verbose_statistics);

    std::cout << "Communication Cost J(C, D, PI): " << comm_cost << std::endl;

    g.setPartition(partition);




    return 0;
}