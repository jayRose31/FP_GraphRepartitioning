#ifndef GRAPH_H
#define GRAPH_H

#include <vector>

class Graph{
    private:
        // nodes
        // edges
        // partition
        // Gewichte?

        
        std::vector<std::vector<long>> adjacency_list;

        //TODO: Ich muss die Partitionierungsinformation codieren!
        //TODO: Hierfür brauche ich einen zusätzliche datentyp
        

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


};


#endif