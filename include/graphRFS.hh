#ifndef GRAPHRFS_H
#define GRAPHRFS_H

#include "graph.hh"


/*
RFS := Repartition From Scratch.
Die Strategie ist also:
1. Berechne neue Partition
2. Finde migration minimal mapping 
3. Permutiere die neu berechnete Partition
*/

class graphRFS : public Graph {
    public:
        graphRFS(long node_count) : Graph(node_count) {}
        void repartition() override;
};

#endif