cmake_minimum_required(VERSION 3.10)
project(GraphPartitioning)

# Compiler Flags
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS_RELEASE "-Ofast -DNDEBUG -finline-functions -fwhole-program -ftree-vectorize -flto -funroll-loops -falign-loops -march=native -Wall -Wextra -pedantic -fno-exceptions")
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-O3 -g3 -DNDEBUG -fno-inline -fno-omit-frame-pointer -fwhole-program -ftree-vectorize -flto -funroll-loops -falign-loops -march=native -Wall -Wextra -pedantic")
set(CMAKE_CXX_FLAGS_MINSIZEREL "-O3 -DNDEBUG -finline-functions -fwhole-program -ftree-vectorize -flto -funroll-loops -falign-loops -march=native -Wall -Wextra -pedantic")
set(CMAKE_CXX_FLAGS_DEBUG "-g -O0 -Wall -Wextra -pedantic")
set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE) 

find_package(nlohmann_json 3.2.0 REQUIRED)

set(SOURCES src/graph.cpp src/graph_io.cpp util/fileUtils.cpp src/graphRFS.cpp src/graphRFSMultilevel.cpp)

add_subdirectory(third_party/SharedMap)
add_subdirectory(third_party/ProcessMappingAnalyzer)


#add_executable(graph_test tests/graph_test.cpp ${SOURCES})
#add_executable(graph_repartitioning src/graph_repartitioning.cpp ${SOURCES})
#add_executable(sharedMapTest tests/sharedMapTest.cpp)
#add_executable(sharedMapTest1 tests/function_tests/sharedMapTest1.cpp ${SOURCES})
#add_executable(repartitioningWithFileInput src/repartitioningWithFileInput.cpp ${SOURCES})
add_executable(graphRFSTest tests/function_tests/graphRFSTest.cpp ${SOURCES})
add_executable(graphRFSExp experiments/graphRFSExperiments/graphRFSExperiments.cpp ${SOURCES})
add_executable(graphRFSMultilevelExp experiments/graphRFSExperiments/graphRFSMultilevelExperiments.cpp ${SOURCES} )
add_executable(graphRFSMultilevelTest tests/function_tests/graphRFSMultilevelTest.cpp ${SOURCES} )

#target_link_libraries(graph_test sharedmap)
#target_link_libraries(graph_repartitioning sharedmap nlohmann_json::nlohmann_json)
#target_link_libraries(sharedMapTest sharedmap)
#target_link_libraries(sharedMapTest1 sharedmap nlohmann_json::nlohmann_json)
#target_link_libraries(repartitioningWithFileInput sharedmap nlohmann_json::nlohmann_json)
target_link_libraries(graphRFSTest sharedmap nlohmann_json::nlohmann_json)
target_link_libraries(graphRFSExp sharedmap nlohmann_json::nlohmann_json)
target_link_libraries(graphRFSMultilevelExp sharedmap nlohmann_json::nlohmann_json)
target_link_libraries(graphRFSMultilevelTest sharedmap nlohmann_json::nlohmann_json)


