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
        edgeInformation[0] = -1;
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

    std::vector<long> numberNodesAndUpdates = readNumberNodesAndEdgesFromFile(file);
    std::cout << "Number of Nodes: " << numberNodesAndUpdates[0] << std::endl;
    std::cout << "Number of Updates: " << numberNodesAndUpdates[1] << std::endl;

    // Erstelle den Graphen
    //? Ich glaube ich brauche +1, weil wir bei 1 anfangen zu zählen
    Graph g(numberNodesAndUpdates[0] + 1);

    int linesRead = 0;
    for(int i = 0; i < numberNodesAndUpdates[1] ; i++) {
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

    //TODO: Das Partitioningtool von Henning einbinden



    // Umleiten des Standardausgabestreams in eine Datei
    std::ofstream outFile("graph_output.txt");
    if (!outFile) {
        std::cerr << "Fehler beim Öffnen der Ausgabedatei." << std::endl;
        return 1;
    }

    // Speichern des aktuellen Puffer
    std::streambuf* coutBuf = std::cout.rdbuf();
    // Umleiten von std::cout zu outFile
    std::cout.rdbuf(outFile.rdbuf());

    // Graphen ausgeben
    g.printGraph();

    // Wiederherstellen des Standardausgabestreams
    std::cout.rdbuf(coutBuf);

    outFile.close();
    file.close();
    return 0;
}