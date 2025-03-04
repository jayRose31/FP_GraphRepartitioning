#include <iostream>
#include "../include/graph.hh"

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

int main() {
    testGraph();
    return 0;
}