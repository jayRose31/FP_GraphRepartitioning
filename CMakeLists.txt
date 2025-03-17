cmake_minimum_required(VERSION 3.10)
project(GraphPartitioning)

set(CMAKE_CXX_STANDARD 17)  # Use C++17 (adjust as needed)
set(CMAKE_CXX_FLAGS "-Wall -g")  # Match Makefile flags

find_package(nlohmann_json 3.2.0 REQUIRED)

set(SOURCES src/graph.cpp src/graph_io.cpp util/fileUtils.cpp)

add_subdirectory(third_party/SharedMap)


add_executable(graph_test tests/graph_test.cpp ${SOURCES})
add_executable(graph_repartitioning src/graph_repartitioning.cpp ${SOURCES})
add_executable(sharedMapTest tests/sharedMapTest.cpp)
add_executable(sharedMapTest1 tests/function_tests/sharedMapTest1.cpp ${SOURCES})

target_link_libraries(graph_test sharedmap)
target_link_libraries(graph_repartitioning sharedmap nlohmann_json::nlohmann_json)
target_link_libraries(sharedMapTest sharedmap)
target_link_libraries(sharedMapTest1 sharedmap nlohmann_json::nlohmann_json)




