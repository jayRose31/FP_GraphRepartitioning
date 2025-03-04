CXX = g++
CXXFLAGS = -Wall -g

TARGETS = graph_test graph_repartitioning

SRCS = src/graph.cpp

all: $(TARGETS)

graph_test: tests/graph_test.cpp $(SRCS)
	$(CXX) $(CXXFLAGS) -o $@ $^

graph_repartitioning: src/graph_repartitioning.cpp $(SRCS)
	$(CXX) $(CXXFLAGS) -o $@ $^

clean:
	rm -f $(TARGETS)