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
	unique_ptr<Graph> g (new Graph (n));
	unique_ptr<Graph> g2;

	bool is_connected = false;
	while (!is_connected){
		for (int node_num = 0; node_num < n; node_num++){
			for (int edge_num = 0; edge_num < edges_per_node; edge_num++){
				//Edge from node_num to edge_num
				long long weight = (rand() % 100000) + 1;
				int target = rand() % n;
				g->join(node_num, target, weight);
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

	long long ans = 0;
	chrono::high_resolution_clock::time_point start_time =
	chrono::high_resolution_clock::now();

	while (n > 1){
		UnionFind uf_boruvka (n);
		set<pair<int, int> > taken_edges;
		//Determine minimum weight edge for each tree
		for (int i = 0; i < n; i++){
			long long minWeight = 1000000000;
			int nearestNode = -1;
			for (pair<int, long long> entry : g->adjlist[i]){
				int target = entry.first;
				long long weight = entry.second;
				if (weight < minWeight){
					minWeight = weight;
					nearestNode = target;
				}
			}
			//Determine new connected component
			uf_boruvka.join(i, nearestNode);
			//Enumerate edges to prevent repeats
			pair<int, int> p = 
			make_pair(min(i, nearestNode), max(i, nearestNode));
			if (taken_edges.count(p) == 0){
				taken_edges.insert(p);
				ans += minWeight;
			}
		}
		//Renumber connected components
		//Map of node number to cc number
		unordered_map<int, int> ccmap;
		int cccnt = 0;
		for (int i = 0; i < n; i++){
			int parent = uf_boruvka.parent(i);
			//If parent not seen yet
			if (ccmap.count(parent) == 0){
				ccmap[parent] = cccnt;
				cccnt += 1;
			}
		}
		//Collapse graphs together into new graph
		g2 = make_unique<Graph>(cccnt);
		for (int i = 0; i < n; i++){
			int newSrc = ccmap[uf_boruvka.parent(i)];
			for (pair<int, long long> entry : g->adjlist[i]){
				int newTarget = ccmap[uf_boruvka.parent(entry.first)];
				long long weight = entry.second;
				g2->join(newSrc, newTarget, weight);
			}
		}
		//Replace graphs, update n
		g = move(g2);
		n = cccnt;
	}
	chrono::high_resolution_clock::time_point end_time = 
	chrono::high_resolution_clock::now();
	cout << "n: " << n << " e: " << edges_per_node << endl;
	cout << "Time Elapsed: " << scientific <<
	chrono::duration_cast<chrono::nanoseconds>(end_time - start_time).count() / 1e9
	<< " s" << endl;
	cout << "MST Weight: " << ans << endl;
}