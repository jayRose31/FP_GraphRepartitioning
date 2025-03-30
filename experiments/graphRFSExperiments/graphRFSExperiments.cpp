#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <string> 
#include "../../include/graph.hh"
#include "../../include/graphRFS.hh"
#include "../../include/graph_io.hh"
#include "../../include/fileUtils.hh"

int main(int argc, char* argv[]) {
    
    //TODO: read input from cli
    //1. read graph from file
    //2. sharedMapConfig
    //3. Number Updates till repartitioning

    std::string configFile ="./res/sharedMapConfigs/sharedMap_config1.json";


    // Datei öffnen
    std::string filename = "./res/dynGraphs/haggle.seq";
    std::ifstream file(filename); 
    if (!file) {
        std::cerr << "Fehler beim Öffnen der Datei: " << filename << std::endl;
        return 1;
    }

    std::cout << "starte experiment" << std::endl;

    std::vector<long> edge;

    std::vector<long> numberNodesAndUpdates = fileUtils::readNumberNodesAndEdgesFromFile(file);

    // Erstelle den Graphen
    graphRFS g(numberNodesAndUpdates[0]);

    // Kanten einlesen
    for(int i = 0; i < numberNodesAndUpdates[1] *2/ 3 ; i++) {
        edge = fileUtils::readEdgeInformationFromFile(file);
        
        if(edge[0] == 1) {
            g.add_edge(edge[1], edge[2]);
            
        } else if(edge[0] == 0) {
            g.remove_edge(edge[1], edge[2]);
            
        }
    }

    
    g.partitionWithSharedMap(configFile);
    
    for(int i = 0; i < 1000 ; i++) {
        edge = fileUtils::readEdgeInformationFromFile(file);
        
        if(edge[0] == 1) {
            g.add_edge(edge[1], edge[2]);
            
        } else if(edge[0] == 0) {
            g.remove_edge(edge[1], edge[2]);
            
        }
    }

    // Zeit messen
    g.repartition();


    // fertig
    file.close();

    
    // ---------- Schreibe Sachen damit ich in python das Tool von Henning benutzen kann ------------
    filename = "./experiments/graphRFSExperiments/analyzerTool_temporary.txt";
    std::ofstream analyzer_tool(filename); 
    if (!analyzer_tool) {
        std::cerr << "Fehler beim Öffnen der Datei: " << filename << std::endl;
        return 1;
    }


    std::string graph_metis_filename = "/home/jacob/Dokumente/AldaPraktikum/Code/experiments/graphRFSExperiments/Graph_metis" ;
    std::string graph_partition_filename = "/home/jacob/Dokumente/AldaPraktikum/Code/experiments/graphRFSExperiments/Graph_partition";
    std::string out_graph_filename = "/home/jacob/Dokumente/AldaPraktikum/Code/experiments/graphRFSExperiments/out_graph" ;
    
    GraphIo io;
    io.writeGraphToFileMetis(graph_metis_filename, g);
    io.writePartitionToFile(graph_partition_filename, g);

    analyzer_tool <<  graph_metis_filename << std::endl;
    analyzer_tool <<  graph_partition_filename << std::endl;
    analyzer_tool <<  out_graph_filename << std::endl;
    

    // Die anderen Paramter holen 
    // Alles egal außer: hierarchy, distance und epsilon
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
        std::cerr << "Error reading config file. Exiting." << std::endl;
        return 1;
    }


    for (size_t i = 0; i < hierarchy.size(); ++i) {
        analyzer_tool << hierarchy[i];
        if (i < hierarchy.size() - 1) {
            analyzer_tool << ":";
        }
    }
    analyzer_tool << std::endl;


    for (size_t i = 0; i < distance.size(); ++i) {
        analyzer_tool << distance[i];
        if (i < distance.size() - 1) {
            analyzer_tool << ":";
        }
    }
    analyzer_tool << std::endl; 

    analyzer_tool << imbalance << std::endl;

    analyzer_tool.close();

    // ---------------------------------------- fertig ----------------------------------------------------



    // Datei öffnen zum schreiben der Ergebnisse
    filename = "./experiments/graphRFSExperiments/results_temporary.txt";
    std::ofstream res_temp(filename); 
    if (!res_temp) {
        std::cerr << "Fehler beim Öffnen der Datei: " << filename << std::endl;
        return 1;
    }

    // Ergebnisse schreiben
    // 1. Zeit zum repartitionieren




    
    res_temp.close();


    
    return 0;
}