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
    std::string filename = "./res/dynGraphs/haggle.seq";
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
    for(int i = 0; i < numberNodesAndUpdates[1] ; i++) {
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



    Graph g_test(5);
    g_test.add_edge(0, 1);
    g_test.add_edge(0, 2);
    g_test.add_edge(4, 3);
    g_test.add_edge(4, 2);
    g_test.add_edge(1, 4);

    g_test.printGraph();

    g_test.partitionWithSharedMap("./res/sharedMapConfigs/sharedMap_config2.json");

   // g.partitionWithSharedMap("./res/sharedMapConfigs/sharedMap_config1.json");

    
    return 0;
}