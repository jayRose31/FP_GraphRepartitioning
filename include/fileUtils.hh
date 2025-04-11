#ifndef FILEUTILS_HH
#define FILEUTILS_HH

#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <string> 
#include "../third_party/SharedMap/include/libsharedmap.h"
#include <nlohmann/json.hpp>

/*

Hier will ich die ganzen Funktionen bündeln die ich für das Einlesen und Schreiben von Dateien brauche.
Zbsp kommen hier die ganzen Funktionen aus graph_repartitioning rein um einen Graph
aus einer Datei zu lesen oder die config datei zu lesen.

Diese Klasse sollte man benutzen können ohne sie zu initialisieren. Also static sein?

*/

using json = nlohmann::json;    

class fileUtils {
public:
    
    //! Wichtig hier: Node format umwandeln von 1...n
    //! zu 0...n-1
    static std::vector<long> readEdgeInformationFromFile(std::ifstream& file) ;

    static std::vector<long> readNumberNodesAndEdgesFromFile(std::ifstream& file);

    static bool readConfigFileSharedMap(const std::string& filename, std::vector<int>& hierarchy, std::vector<int>& distance, int& l, float& imbalance, int& n_threads, int& seed, shared_map_strategy_type_t& strategy, shared_map_algorithm_type_t& parallel_alg, shared_map_algorithm_type_t& serial_alg, bool& verbose_error, bool& verbose_statistics) ;

    static int getNumberPartitions(const std::string& filename) ;

    static std::vector<int> readHierarchyFromFile(const std::string& filename);

};

#endif