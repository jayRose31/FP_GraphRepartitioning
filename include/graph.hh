#ifndef GRAPH_H
#define GRAPH_H

#include <vector>
#include <string>


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
    protected:
        int edge_count;
        
        std::vector<std::vector<long>> adjacency_list;
        
        std::vector<long> partition;

    public:
        //Konstruktor
        Graph(long node_count);

       virtual void add_edge(long start_node, long destination_node);
       virtual void remove_edge(long start_node, long destination_node);
       
       // Hier keine Implementierung, nur in den Kinderklassen
       virtual void repartition(std::string configFile);


       void printGraph();


       std::vector<long> getPartition();
       void setPartition(std::vector<long> partition);
       void setPartition(int* partition);

       int getNumberNodes();
       int getNumberEdges();
       std::vector<std::vector<long>> getGraph();

       CSR convertToCSR();

       // this will set the current partition of the graph
       // using the shared map algorithm
       int partitionWithSharedMap(std::string configFile);


};


#endif