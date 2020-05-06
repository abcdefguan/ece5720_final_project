CXX = g++
CXXFLAGS = -g -Wall -O2 -fopenmp

all: kruskal prim boruvka filter_kruskal parallel_boruvka

parallel_boruvka: graph.o union_find.o parallel_boruvka.o
	$(CXX) $(CXXFLAGS) -o parallel_boruvka graph.o union_find.o parallel_boruvka.o

filter_kruskal: graph.o union_find.o filter_kruskal.o
	$(CXX) $(CXXFLAGS) -o filter_kruskal graph.o union_find.o filter_kruskal.o

prim: graph.o union_find.o prim.o
	$(CXX) $(CXXFLAGS) -o prim graph.o union_find.o prim.o

kruskal: union_find.o kruskal.o
	$(CXX) $(CXXFLAGS) -o kruskal union_find.o kruskal.o

boruvka: graph.o union_find.o boruvka.o
	$(CXX) $(CXXFLAGS) -o boruvka graph.o union_find.o boruvka.o

parallel_boruvka.o: parallel_boruvka.cpp union_find.h graph.h
	$(CXX) $(CXXFLAGS) -c parallel_boruvka.cpp

filter_kruskal.o: filter_kruskal.cpp union_find.h graph.h
	$(CXX) $(CXXFLAGS) -c filter_kruskal.cpp

boruvka.o: boruvka.cpp union_find.h graph.h
	$(CXX) $(CXXFLAGS) -c boruvka.cpp

prim.o: prim.cpp union_find.h graph.h
	$(CXX) $(CXXFLAGS) -c prim.cpp

kruskal.o: kruskal.cpp union_find.h
	$(CXX) $(CXXFLAGS) -c kruskal.cpp

graph.o: graph.cpp
	$(CXX) $(CXXFLAGS) -c graph.cpp

union_find.o: union_find.cpp
	$(CXX) $(CXXFLAGS) -c union_find.cpp