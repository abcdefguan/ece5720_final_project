#include <fstream>
#include <iostream>
#include <vector>
#include <string>
#include <cstdlib>
#include <array>
#include <ctime>
#include <chrono>
#include <unordered_set>
#include <memory>
#include <algorithm>
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
	unordered_set<int> considered_nodes;
	priority_queue<pair<long long, int> > considered_edges;

	considered_nodes.reserve(n * 10);

	bool is_connected = false;
	while (!is_connected){
		for (int node_num = 0; node_num < n; node_num++){
			for (int edge_num = 0; edge_num < edges_per_node; edge_num++){
				//Edge from node_num to edge_num
				long long weight = (rand() % 100000) + 1;
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

	long long ans = 0;
	chrono::high_resolution_clock::time_point start_time =
	chrono::high_resolution_clock::now();
	//Add node 0
	//add_node(0, g, &considered_nodes, &considered_edges);
	int node = 0;
	considered_nodes.insert(node);
	for (pair<int, long long> entry : g.adjlist[node]){
		int target = entry.first;
		long long weight = entry.second;
		//Target not yet considered
		if (considered_nodes.count(target) == 0){
			//Weight is negated as workaround to redefining comparator
			considered_edges.emplace(-weight, target);
		}
	}
	//While we've not yet considered every node
	while (considered_nodes.size() < n){
		if (considered_edges.size() == 0){
			cout << "Unexpected empty edge set: graph is unconnected" << endl;
			return 1;
		}
		pair<long long, int> edge = considered_edges.top();
		considered_edges.pop();
		int target = edge.second;
		long long weight = - edge.first;
		//Target not yet considered
		if (considered_nodes.count(target) == 0){
			node = target;
			//add_node(target, g, &considered_nodes, &considered_edges);
			considered_nodes.insert(node);
			for (pair<int, long long> entry : g.adjlist[node]){
				int target = entry.first;
				long long weight = entry.second;
				//Target not yet considered
				if (considered_nodes.count(target) == 0){
					//Weight is negated as workaround to redefining comparator
					considered_edges.emplace(-weight, target);
				}
			}
			ans += weight;
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