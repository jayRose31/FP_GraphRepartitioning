cmake_minimum_required(VERSION 3.10)
project(GraphPartitioning)

set(CMAKE_CXX_STANDARD 17)  # Use C++17 (adjust as needed)
set(CMAKE_CXX_FLAGS "-Wall -g")  # Match Makefile flags

# Source files shared by both executables
set(SOURCES src/graph.cpp src/graph_io.cpp)

add_subdirectory(third_party/SharedMap)

# Define the first executable (graph_test)
add_executable(graph_test tests/graph_test.cpp ${SOURCES})

# Define the second executable (graph_repartitioning)
add_executable(graph_repartitioning src/graph_repartitioning.cpp ${SOURCES})

target_link_libraries(graph_test sharedmap)
target_link_libraries(graph_repartitioning sharedmap)