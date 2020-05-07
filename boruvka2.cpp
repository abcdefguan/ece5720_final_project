#include <fstream>
#include <iostream>
#include <vector>
#include <string>
#include <cstdlib>
#include <array>
#include <ctime>
#include <chrono>
#include <unordered_set>
#include <unordered_map>
#include <memory>
#include <algorithm>
#include <set>
#include <queue>
#include "graph.h"
#include "union_find.h"

using namespace std;


int main(int argc, char ** argv){

	if (argc < 3){
		cout << "Usage: " << argv[0] << " n e" << endl;
		cout << "n: number of nodes in graph" << endl;
		cout << "e: number of edges per node" << endl;
		return 1;
	}

	//Set cout precision
	cout.precision(2);

	//Set a random seed
	srand(1337);
	
	int n = atoi(argv[1]);
	int edges_per_node = atoi(argv[2]);

	UnionFind uf (n);
	Graph g (n);

	bool is_connected = false;
	while (!is_connected){
		for (int node_num = 0; node_num < n; node_num++){
			for (int edge_num = 0; edge_num < edges_per_node; edge_num++){
				//Edge from node_num to edge_num
				long long weight = (rand() % 100000000) + 1;
				int target = rand() % n;
				g.join(node_num, target, weight);
				uf.join(node_num, target);
			}
		}
		is_connected = true;
		for (int node_num = 0; node_num < n; node_num++){
			if (!uf.query(node_num, 0)){
				is_connected = false;
			}
		}
		//Verifies that graph is connected
		if (!is_connected){
			cout << "Warning: Graph is not connected" << endl;
		}
	}
	//cout << "Graph ready" << endl;

	long long ans = 0;
	chrono::high_resolution_clock::time_point start_time =
	chrono::high_resolution_clock::now();

	UnionFind uf_boruvka (n);

	vector<long long> minWeight (n, 0);
	vector<int> nearestNode (n, 0);

	while (uf_boruvka.get_num_cc() > 1){
		//cout << "cc: " << uf_boruvka.get_num_cc() << endl;
		set<pair<int, int> > taken_edges;
		//Determine minimum weight edge for each tree
		for (int i = 0; i < n; i++){
			minWeight[i] = 1000000000;
			nearestNode[i] = -1;
		}
		for (int i = 0; i < n; i++){
			int parent = uf_boruvka.parent(i);
			for (auto it = g.adjlist[i].begin(); it != g.adjlist[i].end(); ){
				int target = uf_boruvka.parent((*it).first);
				long long weight = (*it).second;
				
				if (target != parent && weight < minWeight[parent]){
					minWeight[parent] = weight;
					nearestNode[parent] = target;
				}
				if (target == parent){
					it = g.adjlist[i].erase(it);
				}
				else{
					it++;
				}
			}
		}
		for (int i = 0; i < n; i++){
			//Ignore if not own parent
			if (uf_boruvka.parent(i) != i){
				continue;
			}
			//Determine new connected component
			uf_boruvka.join(i, nearestNode[i]);
			//Enumerate edges to prevent repeats
			pair<int, int> p = 
			make_pair(min(i, nearestNode[i]), max(i, nearestNode[i]));
			if (taken_edges.count(p) == 0){
				taken_edges.insert(p);
				ans += minWeight[i];
			}
		}
	}

	chrono::high_resolution_clock::time_point end_time = 
	chrono::high_resolution_clock::now();
	cout << "n: " << n << " e: " << edges_per_node << endl;
	cout << "Time Elapsed: " << scientific <<
	chrono::duration_cast<chrono::nanoseconds>(end_time - start_time).count() / 1e9
	<< " s" << endl;
	cout << "MST Weight: " << ans << endl;
}