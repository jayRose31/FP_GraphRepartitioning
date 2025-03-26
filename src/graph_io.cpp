#include "../include/graph_io.hh"

#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>

//TODO: extend to weighted case
void GraphIo::writeGraphToFileMetis(std::string filename, Graph& g) {
    //1. check if file exists
    // if not create it, else open and clear contents

    //TODO: Change io output to logging info
    std::ifstream infile(filename);
    if (infile.good()) {
        std::cout << "File exists: " << filename << std::endl;
        
        infile.close();
        
        // Clear the contents of the file
        std::ofstream outfile(filename, std::ios::trunc);
        if (!outfile) {
            std::cerr << "Error clearing file: " << filename << std::endl;
            return;
        }

        outfile.close();
    } else {
        std::cout << "File does not exist, creating file: " << filename << std::endl;
        std::ofstream outfile(filename);
        if (!outfile) {
            std::cerr << "Error creating file: " << filename << std::endl;
            return;
        }
        outfile.close();
    }






    // Continue with writing to the file
    std::ofstream outfile(filename, std::ios::app); // Open file in append mode
    if (!outfile) {
        std::cerr << "Error opening file: " << filename << std::endl;
        return;
    }

    // write Header:
    outfile << "% hier steht der Header \n";
    outfile << g.getNumberNodes() << " " << g.getNumberEdges() << "\n";

    std::vector<std::vector<long>> adjList = g.getGraph();

    // write Edges
    outfile << "% hier kommen die ganzen edges yeah \n";
    for(int i=0; i < g.getNumberNodes(); i++) {
        int neighborCount = adjList.at(i).size();
        
        for(int j=0; j < neighborCount; j++) {
            //! add shift
            outfile << adjList.at(i).at(j) + 1 <<" ";
        }
        outfile << "\n";
    }

    
    
    outfile.close();
}


void GraphIo::writePartitionToFile(std::string filename, Graph& g) {
    //1. check if file exists
    // if not create it, else open and clear contents
        //1. check if file exists
    // if not create it, else open and clear contents

    //TODO: Change io output to logging info
    std::ifstream infile(filename);
    if (infile.good()) {
        std::cout << "File exists: " << filename << std::endl;
        
        infile.close();
        
        // Clear the contents of the file
        std::ofstream outfile(filename, std::ios::trunc);
        if (!outfile) {
            std::cerr << "Error clearing file: " << filename << std::endl;
            return;
        }

        outfile.close();
    } else {
        std::cout << "File does not exist, creating file: " << filename << std::endl;
        std::ofstream outfile(filename);
        if (!outfile) {
            std::cerr << "Error creating file: " << filename << std::endl;
            return;
        }
        outfile.close();
    }






    // Continue with writing to the file
    std::ofstream outfile(filename, std::ios::app); // Open file in append mode
    if (!outfile) {
        std::cerr << "Error opening file: " << filename << std::endl;
        return;
    }

    // write Header:
    std::vector<long> partition = g.getPartition();

    for (size_t i = 0; i < partition.size(); i++) {
        outfile << partition.at(i) << "\n";
    }
    outfile.close();

}