#include "union_find.h"
#include <iostream>

UnionFind :: UnionFind(int _n) {
	n = _n;
	for (int i = 0; i < n; i++){
		connected.push_back(i);
	}
}

int UnionFind :: parent(int node){
	int predecessor = connected[node];
	int ans;
	if (predecessor != node){
		ans = parent(predecessor);
		connected[node] = ans;
	}
	else{
		ans = node;
	}
	return ans;
}

void UnionFind :: join(int node1, int node2){
	connected[parent(node2)] = parent(node1);
}

bool UnionFind :: query(int node1, int node2){
	return parent(node1) == parent(node2);
}

UnionFind::~UnionFind() {
	connected.clear();
}