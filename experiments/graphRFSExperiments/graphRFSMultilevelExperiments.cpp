#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <string> 
#include <chrono>
#include <filesystem>

#include "../../include/graph.hh"
#include "../../include/graphRFS.hh"
#include "../../include/graphRFSMultilevel.hh"
#include "../../include/graph_io.hh"
#include "../../include/fileUtils.hh"

int main(int argc, char* argv[]) {
    

    //------------------------------- init ------------------------------------------

    // Check if the correct number of arguments is provided
    if (argc != 4) {
        std::cerr << "Usage: " << argv[0] << " <configFile> <filename> <number_of_updates>" << std::endl;
        return 1;
    }

    std::filesystem::path executable_path = std::filesystem::path(argv[0]).parent_path();
    
    std::filesystem::path analyzer_tool_path = executable_path / "../experiments/graphRFSExperiments/analyzerTool_temporary.txt";
    std::filesystem::path graph_metis_path = executable_path / "../experiments/graphRFSExperiments/Graph_metis";
    std::filesystem::path graph_partition_path = executable_path / "../experiments/graphRFSExperiments/Graph_partition";
    std::filesystem::path out_graph_path = executable_path / "../experiments/graphRFSExperiments/out_graph";
    std::filesystem::path results_temp_path = executable_path / "../experiments/graphRFSExperiments/results_temporary.txt";


    // Read arguments from the command line
    std::string configFile = argv[1];  
    std::string graphFilename = argv[2];   
    int numberOfUpdates = std::stoi(argv[3]);
    

    // Assert that the third argument is a valid integer
    assert((numberOfUpdates = std::stoi(argv[3])) >= 0 && "Number of updates must be a non-negative integer");


    std::ifstream file(graphFilename); 
    if (!file) {
        std::cerr << "Fehler beim Öffnen der Datei: " << graphFilename << std::endl;
        return 1;
    }

    std::cout << "starte experiment" << std::endl;

    std::vector<long> edge;

    std::vector<long> numberNodesAndUpdates = fileUtils::readNumberNodesAndEdgesFromFile(file);



    // ------------------------------- build graph and partition --------------------------------
    graphRFSMultilevel g(numberNodesAndUpdates[0]);


    // Kanten einlesen
    for(int i = 0; i < numberNodesAndUpdates[1] *2/ 3; i++) {
        edge = fileUtils::readEdgeInformationFromFile(file);
        
        if(edge[0] == 1) {
            g.add_edge(edge[1], edge[2]);
            
        } else if(edge[0] == 0) {
            g.remove_edge(edge[1], edge[2]);
            
        }
    }

    
    g.partitionWithSharedMap(configFile);
    
    for(int i = 0; i < numberOfUpdates ; i++) {
        edge = fileUtils::readEdgeInformationFromFile(file);
        
        if(edge[0] == 1) {
            g.add_edge(edge[1], edge[2]);
            
        } else if(edge[0] == 0) {
            g.remove_edge(edge[1], edge[2]);
            
        }
    }

    // Zeit messen
    auto start = std::chrono::high_resolution_clock::now(); // Uhr starten

    g.repartition(configFile);
    
    auto stop = std::chrono::high_resolution_clock::now(); // Uhr stoppen
    
    // Dauer berechnen
    //auto duration_repartitioning = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
    std::chrono::duration<double> duration_repartitioning = stop - start;

    // fertig
    file.close();

    

    // ---------- Schreibe Sachen damit ich in python das Tool von Henning benutzen kann ------------
    // std::string filename = "./experiments/graphRFSExperiments/analyzerTool_temporary.txt";
    std::ofstream analyzer_tool(analyzer_tool_path); 
    if (!analyzer_tool) {
        std::cerr << "Fehler beim Öffnen der Datei: " << analyzer_tool_path << std::endl;
        return 1;
    }


    // std::string graph_metis_filename = "/home/jacob/Dokumente/AldaPraktikum/Code/experiments/graphRFSExperiments/Graph_metis" ;
    // std::string graph_partition_filename = "/home/jacob/Dokumente/AldaPraktikum/Code/experiments/graphRFSExperiments/Graph_partition";
    // std::string out_graph_filename = "/home/jacob/Dokumente/AldaPraktikum/Code/experiments/graphRFSExperiments/out_graph" ;
    
    GraphIo io;
    io.writeGraphToFileMetis(graph_metis_path.string(), g);
    io.writePartitionToFile(graph_partition_path.string(), g);

    analyzer_tool <<  graph_metis_path.string() << std::endl;
    analyzer_tool <<  graph_partition_path.string() << std::endl;
    analyzer_tool <<  out_graph_path.string() << std::endl;
    

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

    // ---------------------------------------- pass remaining data to python script--------------------------------------------



    // Datei öffnen zum schreiben der Ergebnisse
    // filename = "./experiments/graphRFSExperiments/results_temporary.txt";
    std::ofstream res_temp(results_temp_path); 
    if (!res_temp) {
        std::cerr << "Fehler beim Öffnen der Datei: " << results_temp_path << std::endl;
        return 1;
    }

    // Ergebnisse schreiben
    // 1. Zeit zum repartitionieren in sekunden
    double duration_in_seconds = duration_repartitioning.count() /* / 1000.0 */ ;
    res_temp << duration_in_seconds << std::endl;

    // 2. Baseline value communication cost
    res_temp << g.get_baseline_comm_cost() <<std::endl;

    // 3. Zeit die sharedMap allein gebraucht hat:
    res_temp << g.get_baseline_speed() << std::endl;

    //4. migration cost: what percent of nodes changed partition
    res_temp << g.get_migrationCost() << std::endl;


    
    res_temp.close();


    //--------------------------------- ----------------- done ------------------------------------------------------
    
    return 0;
}