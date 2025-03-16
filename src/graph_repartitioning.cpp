#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <string> 
#include "../include/graph.hh"
#include "../include/graph_io.hh"

#include <libsharedmap.h>
#include <nlohmann/json.hpp>

using json = nlohmann::json;    

//! Wichtig hier: Node format umwandeln von 1...n
//! zu 0...n-1
std::vector<long> readEdgeInformationFromFile(std::ifstream& file) {
    // enthält 3 Zahlen: erste Zahl ist 0 oder 1, zweite Zahl ist Startknoten, dritte Zahl ist Endknoten
    std::vector<long> edgeInformation(3);

    std::string line;
    if (std::getline(file, line)) {
        std::istringstream iss(line);
        iss >> edgeInformation[0] >> edgeInformation[1] >> edgeInformation[2];

        edgeInformation[1] -= 1;
        edgeInformation[2] -= 1;
    } else {
        std::cerr << "Fehler oder Ende der Datei erreicht." << std::endl;
        edgeInformation[0] = -1;
    }
    
    return edgeInformation;
}

// die Funktion benutze ich um die erste zeile aus der Datei zu lesen
// die erste Zeile enthält die Anzahl der Knoten und Kanten
std::vector<long> readNumberNodesAndEdgesFromFile(std::ifstream& file) {
    std::vector<long> numberNodesAndEdges(2);
    char dummy;
    std::string line;
    if (std::getline(file, line)) {
        std::istringstream iss(line);
        iss >> dummy >> numberNodesAndEdges[0] >> numberNodesAndEdges[1];
    } else {
        std::cerr << "Fehler oder Ende der Datei erreicht." << std::endl;
    }

    return numberNodesAndEdges;
}

bool readConfigFileSharedMap(const std::string& filename, std::vector<int>& hierarchy, std::vector<int>& distance, int& l, float& imbalance, int& n_threads, int& seed, shared_map_strategy_type_t& strategy, shared_map_algorithm_type_t& parallel_alg, shared_map_algorithm_type_t& serial_alg, bool& verbose_error, bool& verbose_statistics) {
    std::ifstream file(filename); // Datei im Textmodus öffnen
    if (!file) {
        std::cerr << "Fehler beim Öffnen der Datei: " << filename << std::endl;
        return false;
    }

    json j;
    file >> j;

    hierarchy = j["hierarchy"].get<std::vector<int>>();
    distance = j["distance"].get<std::vector<int>>();
    l = j["l"].get<int>();
    imbalance = j["imbalance"].get<float>();
    n_threads = j["n_threads"].get<int>();
    seed = j["seed"].get<int>();        
    verbose_error = j["verbose_error"].get<bool>();
    verbose_statistics = j["verbose_statistics"].get<bool>();


    // die muss ich anders behandeln:
    // zuerst als String extrahieren und dann zuordnen
    std::string strategy_string = j["strategy"].get<std::string>();
    if(strategy_string == "NAIVE") {
        strategy = NAIVE;
    } else if(strategy_string == "LAYER") {
        strategy = LAYER;
    } else if(strategy_string == "QUEUE") {
        strategy = QUEUE;
    } else if(strategy_string == "NB_LAYER") {
        strategy = NB_LAYER;
    } else {
        std::cerr << "Strategy not found!" << std::endl;
        return false;
    }


    std::string parallel_alg_string = j["parallel_alg"].get<std::string>();
    if(parallel_alg_string == "MTKAHYPAR_DEFAULT") {
        parallel_alg = MTKAHYPAR_DEFAULT;
    } else if(parallel_alg_string == "MTKAHYPAR_QUALITY") {
        parallel_alg = MTKAHYPAR_QUALITY;
    } else if(parallel_alg_string == "MTKAHYPAR_HIGHEST_QUALITY") {
        parallel_alg = MTKAHYPAR_HIGHEST_QUALITY;
    } else {
        std::cerr << "Parallel Algorithm not found!" << std::endl;
        return false;
    }

    
    std::string serial_alg_string = j["serial_alg"].get<std::string>();
    if(serial_alg_string == "KAFFPA_ECO") {
        serial_alg = KAFFPA_ECO;
    } else if(serial_alg_string == "KAFFPA_FAST") {
        serial_alg = KAFFPA_FAST;
    } else if(serial_alg_string == "KAFFPA_STRONG") {
        serial_alg = KAFFPA_STRONG;
    } else {
        std::cerr << "Serial Algorithm not found!" << std::endl;
        return false;
    }
    

    return true;
}


int main(int argc, char* argv[]) {

    /*
    Grobe Struktur:
    Erstelle den Graph

    in einer loop:
    1. Lese eine Zeile der File
    2. Füge entsprechendes update auf dem Graphen durch (add / delete edge)

    */

    std::string filename = "./res/proper_loans.seq";
    std::ifstream file(filename); // Datei im Textmodus öffnen
    if (!file) {
        std::cerr << "Fehler beim Öffnen der Datei: " << filename << std::endl;
        return 1;
    }

    std::vector<long> edge;

    std::vector<long> numberNodesAndUpdates = readNumberNodesAndEdgesFromFile(file);
    std::cout << "Number of Nodes: " << numberNodesAndUpdates[0] << std::endl;
    std::cout << "Number of Updates: " << numberNodesAndUpdates[1] << std::endl;

    // Erstelle den Graphen
    Graph g(numberNodesAndUpdates[0]);

    int linesRead = 0;
    for(int i = 0; i <  50 /*3330022 */ ; i++) {
        edge = readEdgeInformationFromFile(file);
        
        if(edge[0] == 1) {
            g.add_edge(edge[1], edge[2]);
            linesRead++;
        } else if(edge[0] == 0) {
            g.remove_edge(edge[1], edge[2]);
            linesRead++;
        }
    }

    std::cout << "Lines read: " << linesRead << std::endl;

    /*
    ! Es fällt auf, dass die Zahl in Number of Updates nicht mit der Anzahl der Zeilen in der Datei übereinstimmt.
    ! Die Anzahl der Updates SOLLTE hier 3394979 sein aber es gibt tatsächlich nur 3330022 Zeilen.:(
    */
   
   /*
   Fürs erste schreibe ich meinen Graph in ne Metis file
   und switche dann in sein projekt und lass diese Datei einlesen.
   Einfach nur so zum testen wie das alles funktioniert!
   
   GraphIo g_io = GraphIo();
   
   g_io.writeGraphToFileMetis("bigGraph", g);
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

    CSR csr = g_test.convertToCSR();
    int n = g_test.getNumberNodes();
    
    std::vector<int> v_weights_vector(n, 1);
    int* v_weights = v_weights_vector.data();

    // Print the contents of the CSR representation
    std::cout << "CSR Representation:" << std::endl;

    std::cout << "Adjacency Pointers (adj_ptrs): ";
    for (size_t i = 0; i < csr.adj_ptrs.size(); ++i) {
        std::cout << csr.adj_ptrs[i] << " ";
    }
    std::cout << std::endl;

    std::cout << "Adjacency List (adj): ";
    for (size_t i = 0; i < csr.adj.size(); ++i) {
        std::cout << csr.adj[i] << " ";
    }
    std::cout << std::endl;

    std::cout << "Adjacency Weights (adj_weights): ";
    for (size_t i = 0; i < csr.adj_weights.size(); ++i) {
        std::cout << csr.adj_weights[i] << " ";
    }
    std::cout << std::endl;


    //2. Die anderen Paramter setzen 
    //! convert to int* later
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
   
    bool configSucces = readConfigFileSharedMap("./res/sharedMap_config.json", hierarchy, distance, l, imbalance, n_threads, seed, strategy, parallel_alg, serial_alg, verbose_error, verbose_statistics);

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




    // assert the input (optional)
    bool assert_passed =  shared_map_hierarchical_multisection_assert_input(n, v_weights, csr.adj_ptrs.data(), csr.adj_weights.data() , csr.adj.data()  , hierarchy.data(), distance.data(), l, imbalance, n_threads, seed, strategy, parallel_alg, serial_alg, verbose_error);
    if (!assert_passed) {
        std::cout << "Error while asserting the input!" << std::endl;
        exit(EXIT_FAILURE);
    }


    return 0;
}