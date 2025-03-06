/*
This class is supposed to read and write a graph to a file.
This will be especially important for converting my Graph to a file format (metis)
that can be read by the sharedMap algorithm from Henning

Metis has the following format:
1. line: |V| |E| (information on edge weights)
all other lines list up the neighbors of a node.
So line 1 lists all neighbors of node 1,
line 2 lists all neighbors node 2 and so on and so forth.

The neighbors are simply listed by their node id.
So if for example node 1 has neighbors 2, 3 and 4, the line would look like this:
2 3 4

The Graph is undirected and edges will be listed twice.

*/

#include "graph.hh"
#include <sstream>


class GraphIo {
    private:

    public:
        //empty constructor
        GraphIo() {}

        // Write Graph to File in METIS Format
        // receive a Graph, set 
        void writeGraphToFile(std::string filename, Graph g); 

        // Read Graph from File in METIS Format
        // Return the Graph you read 
        Graph readGraphFromFile(std::string filename);

        /*
        TODO: Ich sollte natürlich irgendwie auch noch die Partition mit verarbeiten die
        TODO: mir sharedMap gibt! Das heißt ich brauche eine Methode um einen GRaphen mit
        TODO: seiner Paritionierung auszulesen!
        */


};