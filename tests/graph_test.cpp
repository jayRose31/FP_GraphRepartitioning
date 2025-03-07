#include <iostream>
#include "../include/graph.hh"
#include "../include/graph_io.hh"

void testGraph() {
    // Create a graph with 5 nodes
    Graph g(5);
    std::cout << "Initial graph:" << std::endl;
    g.printGraph();

    // Add edges
    g.add_edge(0, 1);
    g.add_edge(0, 2);
    g.add_edge(1, 3);
    g.add_edge(3, 4);
    std::cout << "Graph after adding edges:" << std::endl;
    g.printGraph();

    // Remove an edge
    g.remove_edge(0, 1);
    std::cout << "Graph after removing an edge:" << std::endl;
    g.printGraph();
}

void testWriteGraphToFile(std::string filename) {
    Graph g(5);
    g.add_edge(0, 1);
    g.add_edge(0, 2);
    g.add_edge(1, 3);
    g.add_edge(3, 4);
    g.add_edge(2, 4);

    GraphIo g_io = GraphIo();

    g_io.writeGraphToFileMetis(filename, g);

}

int main() {
    std::string filename = "testWriteGraph";
    testWriteGraphToFile(filename);
    std::string command = "cat " + filename;
    system(command.c_str());
    return 0;
}