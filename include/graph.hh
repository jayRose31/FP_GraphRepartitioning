#ifndef GRAPH_H
#define GRAPH_H

#include <vector>


struct CSR{
    std::vector<int> adj_ptrs;
    std::vector<int> adj_weights;
    std::vector<int> adj;
};


/*
 Ich muss hier beachten das nodes in den vorliegenden Formaten
 von 1 bis n gehen aber meine Datenstruktur natürlicherweise von
 0 bis n-1.
 Ich will nicht, dass der Nutzer sich Gedanken machen muss immer 
 n+1 im Konstruktor anzugeben und dann die nullte Stelle einfach ignorieren
 Deshalb habe ich mir folgende Mechanismen ausgedacht:
 Intern (in der Graph Klasse) benutze ich die ganz natürliche Indexierung
 meiner Datenstruktur von 0 bis n-1. Aber bei den IO kLASSEN muss ich ganz genau
 aufpassen, von 1...n umzuwandeln auf 0...n-1
*/
class Graph{
    private:
        // nodes
        // edges
        // partition
        // Gewichte?
        int edge_count;
        
        std::vector<std::vector<long>> adjacency_list;

        //TODO: Ich muss die Partitionierungsinformation codieren!
        //TODO: Hierfür brauche ich einen zusätzliche datentyp

        //? einfach vektor der an jeder Stelle den Index des Clusters speichert- Zbsp partition[0] = 1 
        //? bedeutet, dass der erste Knoten im Cluster 1 ist.
        //? std::vector<long> partition;
        //? Oder gleich int [n] partition; weil Henning das selbe nutzt?
        

    public:
        //Konstruktor
        Graph(long node_count);

        /*
        brauche eine Operation "update" oder "extend"
        um neue Edges hinzuzunehmen und die Partition zu aktualisieren
        Die arbeitet nur auf den Attributen dieser Klasse, deshalb 
        kann sie void sein.
        Ich trenne das erstelmal auf in:
        1. Den Graph um eine Kante zu erweitern
        2. Die Partition neu berechnen (kann man aufrufen nachdem man eine 
        Kante hinzugenommen hat) 
        */
       void add_edge(long start_node, long destination_node);
       void repartition();

       void remove_edge(long start_node, long destination_node);
       void printGraph();

       int getNumberNodes();
       int getNumberEdges();
       std::vector<std::vector<long>> getGraph();

       //TODO: convert to CSR Format function
       CSR convertToCSR();


};


#endif