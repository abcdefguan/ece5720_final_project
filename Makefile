CXX = g++
CXXFLAGS = -g -Wall -O2

all: kruskal prim

prim: graph.o union_find.o prim.o
	$(CXX) $(CXXFLAGS) -o prim graph.o union_find.o prim.o

kruskal: union_find.o kruskal.o
	$(CXX) $(CXXFLAGS) -o kruskal union_find.o kruskal.o

prim.o: prim.cpp union_find.h graph.h
	$(CXX) $(CXXFLAGS) -c prim.cpp union_find.h graph.h

kruskal.o: kruskal.cpp union_find.h
	$(CXX) $(CXXFLAGS) -c kruskal.cpp

graph.o: graph.cpp
	$(CXX) $(CXXFLAGS) -c graph.cpp

union_find.o: union_find.cpp
	$(CXX) $(CXXFLAGS) -c union_find.cpp