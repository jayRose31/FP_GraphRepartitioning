#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <string> 
#include "../include/graph.hh"

void readOneLine(std::ifstream& file) {
    std::string line;
    if (std::getline(file, line)) { // Liest genau eine Zeile
        std::cout << line << std::endl; // Zeile ausgeben
    } else {
        std::cerr << "Fehler oder Ende der Datei erreicht." << std::endl;
    }
}

std::vector<long> readEdgeInformationFromFile(std::ifstream& file) {
    // enthält 3 Zahlen: erste Zahl ist 0 oder 1, zweite Zahl ist Startknoten, dritte Zahl ist Endknoten
    std::vector<long> edgeInformation(3);

    std::string line;
    if (std::getline(file, line)) {
        std::istringstream iss(line);
        iss >> edgeInformation[0] >> edgeInformation[1] >> edgeInformation[2];
    } else {
        std::cerr << "Fehler oder Ende der Datei erreicht." << std::endl;
    }
    
    return edgeInformation;
}

// die Funktion benutze ich um die erste zeile aus der Datei zu lesen
// die erste Zeile enthält die Anzahl der Knoten und Kanten
//! Check mit Christian / der Doku welcher Wert was ist
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




//TODO: Die genau definition der .seq datein abchecken bevor ich weiter mache.
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


    std::vector<long> numberNodesAndEdges = readNumberNodesAndEdgesFromFile(file);
    std::cout << "Number of Nodes: " << numberNodesAndEdges[0] << std::endl;
    std::cout << "Number of Edges: " << numberNodesAndEdges[1] << std::endl;
    for(int i = 0; i < 3; i++) {
        edge = readEdgeInformationFromFile(file);

        for(long value : edge) {
            std::cout << value << " ";
        }
        //leerzeile
        std::cout << "\n" << std::endl;
    }

    file.close();
    return 0;


}