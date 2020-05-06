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
#include "union_find.h"

using namespace std;

class Edge {
public:
	int from;
	int to;
	long long weight;

	Edge (int _from, int _to, long long _weight){
		from = _from;
		to = _to;
		weight = _weight;
	}
};

bool compareEdges(Edge e1, Edge e2){
	return e1.weight < e2.weight;
}

int main(int argc, char ** argv){

	//Set cout precision
	cout.precision(2);

	if (argc < 3){
		cout << "Usage: " << argv[0] << " n e" << endl;
		cout << "n: number of nodes in graph" << endl;
		cout << "e: number of edges per node" << endl;
		return 1;
	}
	//Set a random seed
	srand(1337);

	int n = atoi(argv[1]);
	int edges_per_node = atoi(argv[2]);

	UnionFind uf (n);
	vector<Edge> edges;
	edges.reserve(n * edges_per_node);

	bool is_connected = false;
	while (!is_connected){
		for (int node_num = 0; node_num < n; node_num++){
			for (int edge_num = 0; edge_num < edges_per_node; edge_num++){
				//Edge from node_num to edge_num
				long long weight = (rand() % 100000) + 1;
				int target = rand() % n;
				edges.emplace_back(node_num, target, weight);
				//cout << node_num << " " << target << " " << weight << endl;
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
	//Sort edges
	sort(edges.begin(), edges.end(), compareEdges);
	//Create union find
	UnionFind uf_kruskal(n);
	for (int i = 0; i < edges.size(); i++){
		if (!uf_kruskal.query(edges[i].from, edges[i].to)){
			ans += edges[i].weight;
			//cout << "Taken: " << edges[i].from << " " << edges[i].to << " " << edges[i].weight << endl;
			uf_kruskal.join(edges[i].from, edges[i].to);
		}
		//Joined all the nodes so can be done early
		if (uf_kruskal.get_num_cc() == 1){
			break;
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