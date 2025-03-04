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


        // wie stelle ich edges dar?
        // wie stell ich den Graph am besten dar?
        /*
        Wir brauchen eine darstellung die sich leicht aktualisieren lässt.
        Also neue Kanten sollte man schnell einfügen können.
        Deshalb bietet sich eine Darstellung mit zwei arrays 
        (adjacency array) NICHT an.

        */


        /*
        Ich brauche hier noch eine Funktion um die Datei einzulesen.
        Als hilsfunktion. Ich denke der Returntyp muss noch geändert werden
        Vllt brauchen wir auch noch eine Variable für die Datei.
        */
        

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