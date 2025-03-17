#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <string> 
#include "../include/graph.hh"
#include "../include/graph_io.hh"
#include "../include/fileUtils.hh"

#include "../third_party/SharedMap/include/libsharedmap.h"


int main(int argc, char* argv[]) {
    
    // Datei öffnen
    std::string filename = "./res/proper_loans.seq";
    std::ifstream file(filename); 
    if (!file) {
        std::cerr << "Fehler beim Öffnen der Datei: " << filename << std::endl;
        return 1;
    }

    std::vector<long> edge;

    std::vector<long> numberNodesAndUpdates = fileUtils::readNumberNodesAndEdgesFromFile(file);
    std::cout << "Number of Nodes: " << numberNodesAndUpdates[0] << std::endl;
    std::cout << "Number of Updates: " << numberNodesAndUpdates[1] << std::endl;

    // Erstelle den Graphen
    Graph g(numberNodesAndUpdates[0]);

    // Kanten einlesen
    int linesRead = 0;
    for(int i = 0; i < 5000  /* 3330022 */ ; i++) {
        edge = fileUtils::readEdgeInformationFromFile(file);
        
        if(edge[0] == 1) {
            g.add_edge(edge[1], edge[2]);
            linesRead++;
        } else if(edge[0] == 0) {
            g.remove_edge(edge[1], edge[2]);
            linesRead++;
        }
    }

    std::cout << "Lines read: " << linesRead << std::endl;

    // fertig
    file.close();

    /*
    ! Es fällt auf, dass die Zahl in Number of Updates nicht mit der Anzahl der Zeilen in der Datei übereinstimmt.
    ! Die Anzahl der Updates SOLLTE hier 3394979 sein aber es gibt tatsächlich nur 3330022 Zeilen.:(
    */

  
  //TODO: Das Partitioningtool von Henning einbinden
  //Steps:

    /*
    Ich benutze jetzt erstmal einen ganz kleinen Graphen um zu testen ob das alles funktioniert.

    */

    Graph g_test(5);
    g_test.add_edge(0, 1);
    g_test.add_edge(0, 2);
    g_test.add_edge(4, 3);
    g_test.add_edge(4, 2);
    g_test.add_edge(1, 4);

    g_test.printGraph();

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
   
    bool configSucces = fileUtils::readConfigFileSharedMap("./res/sharedMap_config.json", hierarchy, distance, l, imbalance, n_threads, seed, strategy, parallel_alg, serial_alg, verbose_error, verbose_statistics);

    // optional. just nice debugging rn
    if(configSucces && false) {std::cout << "Hierarchy contents: ";
        for (const auto& level : hierarchy) {
            std::cout << level << " ";
        }
        std::cout << std::endl;
        std::cout << "Distance contents: ";
        for (const auto& dist : distance) {
            std::cout << dist << " ";
        }
        std::cout << std::endl;

        std::cout << "L: " << l << std::endl;
        std::cout << "Imbalance: " << imbalance << std::endl;
        std::cout << "Number of Threads: " << n_threads << std::endl;
        std::cout << "Seed: " << seed << std::endl;
        std::cout << "Strategy: " << strategy << std::endl;
        std::cout << "Parallel Algorithm: " << parallel_alg << std::endl;
        std::cout << "Serial Algorithm: " << serial_alg << std::endl;
        std::cout << "Verbose Error: " << std::boolalpha << verbose_error << std::endl;
        std::cout << "Verbose Statistics: " << std::boolalpha << verbose_statistics << std::endl;
    }



    /*
    Das hier klappt ohne probleme aber gibt riesen output aus.
    
    // assert the input (optional)
    bool assert_passed =  shared_map_hierarchical_multisection_assert_input(n, v_weights, csr.adj_ptrs.data(), csr.adj_weights.data() , csr.adj.data()  , hierarchy.data(), distance.data(), l, imbalance, n_threads, seed, strategy, parallel_alg, serial_alg, verbose_error);
    if (!assert_passed) {
        std::cout << "Error while asserting the input!" << std::endl;
        exit(EXIT_FAILURE);
        }
    */

    int comm_cost;
    int partition[n];


    // do the actual hierarchical multisection
    shared_map_hierarchical_multisection(n, v_weights, csr.adj_ptrs.data() ,  csr.adj_weights.data() , csr.adj.data(), hierarchy.data(), distance.data(), l, imbalance, n_threads, seed, strategy, parallel_alg, serial_alg, comm_cost, partition, verbose_statistics);

    std::cout << "Communication Cost J(C, D, PI): " << comm_cost << std::endl;
    std::cout << "Partition: ";
    for (int i = 0; i < 20/*n*/; ++i) { std::cout << partition[i] << " "; }
    std::cout << std::endl;

    g.setPartition(partition);




    return 0;
}