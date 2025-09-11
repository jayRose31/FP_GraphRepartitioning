#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <string> 
#include <chrono>
#include <filesystem>

#include "../../include/graph.hh"
#include "../../include/graphLocalSearch.hh"
#include "../../include/graph_io.hh"
#include "../../include/fileUtils.hh"

int main(int argc, char* argv[]) {
    
    std::cerr << "came to base 1" << std::endl;

    //------------------------------- init ------------------------------------------

    // Check if the correct number of arguments is provided
    if (argc != 4) {
        std::cerr << "Usage: " << argv[0] << " <configFile> <filename> <bool: new graph?>" << std::endl;
        return 1;
    }

    // Get the directory of the executable
    std::filesystem::path executable_path = std::filesystem::path(argv[0]).parent_path();
    
    
    std::filesystem::path graph_metis_path = executable_path / "../evolving_experiments/graphLSExperiments/Graph_metis";
    std::filesystem::path migration_results_path = executable_path / "../evolving_experiments/graphLSExperiments/migration_results.txt";
    std::filesystem::path time_results_path = executable_path / "../evolving_experiments/graphLSExperiments/time_results.txt";


    // Read arguments from the command line
    std::string configFile_temp = argv[1];  
    std::string graphFilename_temp = argv[2];   
    int new_graph = std::stoi(argv[3]);

    std::filesystem::path configFile = executable_path / ".." / configFile_temp;
    std::filesystem::path graphFilename = executable_path / ".." / graphFilename_temp;



    std::ifstream file(graphFilename); 
    if (!file) {
        std::cerr << "Fehler beim Öffnen der Datei: " << graphFilename << std::endl;
        return 1;
    }

    std::cout << "starte experiment" << std::endl;

    std::vector<long> edge;

    std::vector<long> numberNodesAndUpdates = fileUtils::readNumberNodesAndEdgesFromFile(file);

    std::cerr << "came to base 2" << std::endl;


    // ------------------------------- build graph and partition --------------------------------
    graphLocalSearch g(numberNodesAndUpdates[0]);
    GraphIo io;


    // Kanten einlesen
    for(int i = 0; i < numberNodesAndUpdates[1] *2.0/ 3 ; i++) {
        edge = fileUtils::readEdgeInformationFromFile(file);
        
        if(edge[0] == 1) {
            g.add_edge(edge[1], edge[2]);
            
        } else if(edge[0] == 0) {
            g.remove_edge(edge[1], edge[2]);
            
        }
    }


    // compute initial partition
    g.determine_initial_partition(configFile);

    std::vector<double> residuals = { 1.0/2, 1.0/4, 1.0/8, 1.0/16, 1.0/32, 1.0/64 };
    std::vector<double> rep_times = {};
    std::vector<double> mig_costs = {};

    std::filesystem::path graph_partition_path = executable_path / "../evolving_experiments/graphLSExperiments/Graph_partition";
    std::vector<std::string> partition_paths = {
        graph_partition_path.string() + "1", graph_partition_path.string() + "2",
        graph_partition_path.string() + "3", graph_partition_path.string() + "4",
        graph_partition_path.string() + "5", graph_partition_path.string() + "6"
    };

    // do the actual experiments:
    for (int j = 0; j < residuals.size(); j++) {
    
        for(int i = 0; i < numberNodesAndUpdates[1] * 1.0/3 * residuals[j] ; i++) {
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
        
        rep_times.push_back(duration_repartitioning.count());
        mig_costs.push_back(g.get_migrationCost());

        io.writePartitionToFile(partition_paths[j], g);


    }



    // fertig
    file.close();





    //TODO: Hier kann ich arbeit einsparen! Muss nicht immer den Graphen schreiben!
    if(new_graph == 1) {
        io.writeGraphToFileMetis(graph_metis_path, g);
    }


    // ------------------ write migration cost and time ---------------------------


    std::ofstream migration_res(migration_results_path); 
    if (!migration_res) {
        std::cerr << "Fehler beim Öffnen der Datei: " << migration_results_path << std::endl;
        return 1;
    }

    std::ofstream time_res(time_results_path); 
    if (!time_res) {
        std::cerr << "Fehler beim Öffnen der Datei: " << time_results_path << std::endl;
        return 1;
    }



    for (const auto& time : rep_times) {
        time_res << time << std::endl;
    }

    for(const auto& migr_cost : mig_costs ) {
        migration_res << migr_cost << std::endl;
    } 

    
    migration_res.close();
    time_res.close();


    //--------------------------------- ----------------- done ------------------------------------------------------
    
    return 0;
}