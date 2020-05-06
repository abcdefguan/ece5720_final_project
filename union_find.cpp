#include "union_find.h"
#include <iostream>

UnionFind :: UnionFind(int _n) {
	n = _n;
	num_cc = n;
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

int UnionFind :: thread_safe_parent(int node){
	int predecessor = connected[node];
	if (predecessor != node){
		return parent(predecessor);
	}
	return node;
}

void UnionFind :: join(int node1, int node2){
	int p1 = parent(node1);
	int p2 = parent(node2);
	if (p1 != p2){
		num_cc -= 1;
		connected[p2] = p1;
	}
}

bool UnionFind :: query(int node1, int node2){
	return parent(node1) == parent(node2);
}

bool UnionFind :: thread_safe_query(int node1, int node2){
	return thread_safe_parent(node1) == thread_safe_parent(node2);
}

int UnionFind :: get_num_cc(){
	return num_cc;
}

UnionFind::~UnionFind() {
	connected.clear();
}