#include "../include/fileUtils.hh"

std::vector<long> fileUtils::readEdgeInformationFromFile(std::ifstream& file) {
    // enthält 3 Zahlen: erste Zahl ist 0 oder 1, zweite Zahl ist Startknoten, dritte Zahl ist Endknoten
    std::vector<long> edgeInformation(3);

    std::string line;
    if (std::getline(file, line)) {
        std::istringstream iss(line);
        iss >> edgeInformation[0] >> edgeInformation[1] >> edgeInformation[2];

        // Format vom Graphen ändern (von 1 bis n, zu 0 bis n-1)
        edgeInformation[1] -= 1;
        edgeInformation[2] -= 1;
    } else {
        // std::cerr << "Fehler oder Ende der Datei erreicht." << std::endl;
        edgeInformation[0] = -1;
    }
    
    return edgeInformation;
}

std::vector<long> fileUtils::readNumberNodesAndEdgesFromFile(std::ifstream& file) {
    std::vector<long> numberNodesAndEdges(2);
    char dummy;
    std::string line;
    if (std::getline(file, line)) {
        std::istringstream iss(line);
        iss >> dummy >> numberNodesAndEdges[0] >> numberNodesAndEdges[1];
    } else {
        std::cerr << "Fehler oder Ende der Datei erreicht." << std::endl;
    }

    return numberNodesAndEdges;
}

int fileUtils::getNumberPartitions(const std::string& filename){
    std::ifstream file(filename); // Datei im Textmodus öffnen
    if (!file) {
        std::cerr << "Fehler beim Öffnen der Datei: " << filename << std::endl;
        return false;
    }

    json j;
    file >> j;
    std::vector<int> hierarchy = j["hierarchy"].get<std::vector<int>>();
    file.close();
    
    int numPartitions = 1;
    for (int num : hierarchy) {
        numPartitions *= num;
    }

    return numPartitions;
}

std::vector<int> fileUtils::readHierarchyFromFile(const std::string& filename) {
    std::ifstream file(filename); // Datei im Textmodus öffnen
    if (!file) {
        std::cerr << "Fehler beim Öffnen der Datei: " << filename << std::endl;
        return {};
    }

    json j;
    file >> j;
    file.close();

    if (j.contains("hierarchy")) {
        return j["hierarchy"].get<std::vector<int>>();
    } else {
        std::cerr << "Hierarchie nicht in der Datei gefunden!" << std::endl;
        return {};
    }
}

std::vector<int> fileUtils::readDistancesFromFile(const std::string& filename) {

    std::ifstream file(filename); // Datei im Textmodus öffnen
    if (!file) {
        std::cerr << "Fehler beim Öffnen der Datei: " << filename << std::endl;
        return {};
    }

    json j;
    file >> j;
    file.close();

    if (j.contains("distance")) {
        return j["distance"].get<std::vector<int>>();
    } else {
        std::cerr << "Hierarchie nicht in der Datei gefunden!" << std::endl;
        return {};
    }

}


float fileUtils::readEpsilonFromFile(const std::string& filename) {

    std::ifstream file(filename); // Datei im Textmodus öffnen
    if (!file) {
        std::cerr << "Fehler beim Öffnen der Datei: " << filename << std::endl;
        return false;
    }

    json j;
    file >> j;
    float imbalance = j["imbalance"].get<float>();
    file.close();
   

    return imbalance;
}


bool fileUtils::readConfigFileSharedMap(const std::string& filename, std::vector<int>& hierarchy, std::vector<int>& distance, int& l, float& imbalance, int& n_threads, int& seed, shared_map_strategy_type_t& strategy, shared_map_algorithm_type_t& parallel_alg, shared_map_algorithm_type_t& serial_alg, bool& verbose_error, bool& verbose_statistics) {
    std::ifstream file(filename); // Datei im Textmodus öffnen
    if (!file) {
        std::cerr << "Fehler beim Öffnen der Datei: " << filename << std::endl;
        return false;
    }

    json j;
    file >> j;

    hierarchy = j["hierarchy"].get<std::vector<int>>();
    distance = j["distance"].get<std::vector<int>>();
    l = j["l"].get<int>();
    imbalance = j["imbalance"].get<float>();
    n_threads = j["n_threads"].get<int>();
    seed = j["seed"].get<int>();        
    verbose_error = j["verbose_error"].get<bool>();
    verbose_statistics = j["verbose_statistics"].get<bool>();

    file.close();

    // die muss ich anders behandeln:
    // zuerst als String extrahieren und dann zuordnen
    std::string strategy_string = j["strategy"].get<std::string>();
    if(strategy_string == "NAIVE") {
        strategy = NAIVE;
    } else if(strategy_string == "LAYER") {
        strategy = LAYER;
    } else if(strategy_string == "QUEUE") {
        strategy = QUEUE;
    } else if(strategy_string == "NB_LAYER") {
        strategy = NB_LAYER;
    } else {
        std::cerr << "Strategy not found!" << std::endl;
        return false;
    }


    std::string parallel_alg_string = j["parallel_alg"].get<std::string>();
    if(parallel_alg_string == "MTKAHYPAR_DEFAULT") {
        parallel_alg = MTKAHYPAR_DEFAULT;
    } else if(parallel_alg_string == "MTKAHYPAR_QUALITY") {
        parallel_alg = MTKAHYPAR_QUALITY;
    } else if(parallel_alg_string == "MTKAHYPAR_HIGHEST_QUALITY") {
        parallel_alg = MTKAHYPAR_HIGHEST_QUALITY;
    } else {
        std::cerr << "Parallel Algorithm not found!" << std::endl;
        return false;
    }

    
    std::string serial_alg_string = j["serial_alg"].get<std::string>();
    if(serial_alg_string == "KAFFPA_ECO") {
        serial_alg = KAFFPA_ECO;
    } else if(serial_alg_string == "KAFFPA_FAST") {
        serial_alg = KAFFPA_FAST;
    } else if(serial_alg_string == "KAFFPA_STRONG") {
        serial_alg = KAFFPA_STRONG;
    } else {
        std::cerr << "Serial Algorithm not found!" << std::endl;
        return false;
    }
    

    return true;
}