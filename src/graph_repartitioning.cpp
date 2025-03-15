#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <string> 
#include "../include/graph.hh"
#include "../include/graph_io.hh"

#include <libsharedmap.h>

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
    for(int i = 0; i < 3330022 ; i++) {
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
    //1. Graph in CSR Format umwandeln
    //2. Die anderen Paramter setzen (vielleicht kann ich die durch file einlesen)


    int hierarchy[] = {2, 2};
    int distance[]  = {1, 10};
    int l           = 2;

    // imbalance 3%, 1 thread, seed 0
    float imbalance = 0.03;
    int   n_threads = 1;
    int   seed      = 0;

    shared_map_strategy_type_t  strategy     = NB_LAYER;                    // distribution strategy Non-Blocking Layer
    shared_map_algorithm_type_t parallel_alg = MTKAHYPAR_HIGHEST_QUALITY;   // parallel algorithm Mt-KaHyPar Highest Quality
    shared_map_algorithm_type_t serial_alg   = KAFFPA_STRONG;               // serial algorithm KaFFPa Strong

    // whether to print statistics
    bool verbose_error      = true;
    bool verbose_statistics = true;
   


    return 0;
}