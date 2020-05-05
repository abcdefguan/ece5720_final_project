#include "graph.h"

Graph:: Graph(int _n){
	n = _n;
	for (int i = 0; i < n; i++){
		adjlist.emplace_back();
	}
}

void Graph :: join(int node1, int node2, long long weight){
	if (adjlist[node1][node2] == 0){
		adjlist[node1][node2] = weight;
	}
	else{
		adjlist[node1][node2] = min(weight, adjlist[node1][node2]);
	}
	if (adjlist[node2][node1] == 0){
		adjlist[node2][node1] = weight;
	}
	else{
		adjlist[node2][node1] = min(weight, adjlist[node2][node1]);
	}
}

Graph:: ~Graph() {
	for (int i = 0; i < adjlist.size(); i++){
		adjlist[i].clear();
	}
	adjlist.clear();
}