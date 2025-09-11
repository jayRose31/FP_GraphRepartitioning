cmake_minimum_required(VERSION 3.10)
project(GraphPartitioning)

# Compiler Flags
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS_RELEASE "-O3  -finline-functions -fwhole-program -ftree-vectorize -flto -funroll-loops -falign-loops -march=native -Wall -Wextra -pedantic ")
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-O3 -g3 -DNDEBUG -fno-inline -fno-omit-frame-pointer -fwhole-program -ftree-vectorize -flto -funroll-loops -falign-loops -march=native -Wall -Wextra -pedantic")
set(CMAKE_CXX_FLAGS_MINSIZEREL "-O3 -DNDEBUG -finline-functions -fwhole-program -ftree-vectorize -flto -funroll-loops -falign-loops -march=native -Wall -Wextra -pedantic")
set(CMAKE_CXX_FLAGS_DEBUG "-g -O0 -Wall -Wextra -pedantic")
set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE) 

# Zuerst versuchen, nlohmann-json systemweit zu finden
# Versuch: systemweite Installation finden
find_package(nlohmann_json 3.2.0 QUIET)

# Falls nicht gefunden, lokalen Header-Pfad nutzen
if(NOT nlohmann_json_FOUND)
    message(STATUS "nlohmann_json nicht systemweit gefunden, benutze lokalen Pfad...")
    set(NLOHMANN_JSON_INCLUDE_DIR "$ENV{HOME}/json/single_include")
    if(EXISTS "${NLOHMANN_JSON_INCLUDE_DIR}/nlohmann/json.hpp")
        add_library(nlohmann_json INTERFACE)
        target_include_directories(nlohmann_json INTERFACE "${NLOHMANN_JSON_INCLUDE_DIR}")
    else()
        message(FATAL_ERROR "Konnte nlohmann_json nicht finden. Bitte im Home-Verzeichnis unter ~/json/single_include bereitstellen.")
    endif()
endif()


set(SOURCES
    src/graph.cpp
    src/graph_io.cpp
    util/fileUtils.cpp
    src/graphRFS.cpp
    src/graphRFSMultilevel.cpp
    src/graphLocalSearch.cpp
)

add_subdirectory(third_party/SharedMap)
add_subdirectory(third_party/ProcessMappingAnalyzer)

add_executable(graphRFSTest tests/function_tests/graphRFSTest.cpp ${SOURCES})
add_executable(graphRFSExp experiments/graphRFSExperiments/graphRFSExperiments.cpp ${SOURCES})
add_executable(graphRFSMultilevelExp experiments/graphRFSExperiments/graphRFSMultilevelExperiments.cpp ${SOURCES})
add_executable(graphRFSMultilevelTest tests/function_tests/graphRFSMultilevelTest.cpp ${SOURCES})
add_executable(graphLSTest tests/function_tests/graphLocalSearchTest.cpp ${SOURCES})
add_executable(graphLSExp experiments/graphLSExperiments/graphLSExperiments.cpp ${SOURCES})

# add to remote server
add_executable(graphRFS_evolving  evolving_experiments/graphRFSExperiments/graphRFSExperiments.cpp ${SOURCES})
add_executable(graphRFSMultilevel_evolving  evolving_experiments/graphRFSExperiments/graphRFSMultilevelExperiments.cpp ${SOURCES})
add_executable(graphLS_evolving  evolving_experiments/graphLSExperiments/graphLSExperiments.cpp ${SOURCES})

if(TARGET nlohmann_json::nlohmann_json)
    # Paket gefunden → offizielles Target nutzen
    set(NLOHMANN_TARGET nlohmann_json::nlohmann_json)
else()
    # Lokale Header-only Variante nutzen
    set(NLOHMANN_TARGET nlohmann_json)
endif()

target_link_libraries(graphRFSTest sharedmap ${NLOHMANN_TARGET})
target_link_libraries(graphRFSExp sharedmap ${NLOHMANN_TARGET})
target_link_libraries(graphRFSMultilevelExp sharedmap ${NLOHMANN_TARGET})
target_link_libraries(graphRFSMultilevelTest sharedmap ${NLOHMANN_TARGET})
target_link_libraries(graphLSTest sharedmap ${NLOHMANN_TARGET})
target_link_libraries(graphLSExp sharedmap ${NLOHMANN_TARGET})

# add to remote server
target_link_libraries(graphRFS_evolving sharedmap ${NLOHMANN_TARGET})
target_link_libraries(graphRFSMultilevel_evolving sharedmap ${NLOHMANN_TARGET})
target_link_libraries(graphLS_evolving sharedmap ${NLOHMANN_TARGET})

