cmake_minimum_required(VERSION 3.10)
project(GraphPartitioning)

set(CMAKE_CXX_STANDARD 17)  # Use C++17 (adjust as needed)
set(CMAKE_CXX_FLAGS "-Wall -g")  # Match Makefile flags

find_package(nlohmann_json 3.2.0 REQUIRED)

set(SOURCES src/graph.cpp src/graph_io.cpp)

add_subdirectory(third_party/SharedMap)


add_executable(graph_test tests/graph_test.cpp ${SOURCES})
add_executable(graph_repartitioning src/graph_repartitioning.cpp ${SOURCES})
add_executable(sharedMapTest tests/sharedMapTest.cpp)



target_link_libraries(graph_test sharedmap)
target_link_libraries(graph_repartitioning sharedmap nlohmann_json::nlohmann_json)
target_link_libraries(sharedMapTest sharedmap)

target_include_directories(sharedMapTest PRIVATE ${CMAKE_SOURCE_DIR}/third_party/SharedMap/include)
target_include_directories(graph_repartitioning PRIVATE ${CMAKE_SOURCE_DIR}/third_party/SharedMap/include)


