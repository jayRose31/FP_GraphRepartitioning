#include <iostream>
#include <vector>
#include <string>

#include "../include/graphRFS.hh"
#include "../include/fileUtils.hh"
#include "../third_party/SharedMap/include/libsharedmap.h"

void graphRFS::repartition() {
    
    //TODO eventuell auslagern in die Klasse
    std::string configFile = "./res/sharedMapConfigs/sharedMap_config1.json";
    
    // 1. Berechne neue Partition mit shared map
    
    std::vector<long> old_partition = this->partition;
    this->partitionWithSharedMap(configFile);
    std::vector<long> new_partition = this->partition;

    // 2. erstelle similarity matrix
    int k = fileUtils::getNumberPartitions(configFile);
    std::cout << "Number of Partitions: " << k << std::endl;
    
    // 3. Permutiere die neu berechnete Partition
    
    
    // 4. Setze die Partition
    
    
    // 5. Fertig
    
}