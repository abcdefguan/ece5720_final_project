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
#include <omp.h>
#include "union_find.h"

using namespace std;

const int filter_kruskal_threshold = 10;

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

//pair(weight, number joined)
long long kruskal(vector<Edge> const & edges, UnionFind & uf_kruskal,
	int start, int end){
	long long ans = 0;
	//Sort edges
	sort(edges.begin() + start, edges.begin() + end, compareEdges);
	//Create union find
	for (int i = 0; i < edges.size(); i++){
		if (!uf_kruskal.query(edges[i].from, edges[i].to)){
			ans += edges[i].weight;
			uf_kruskal.join(edges[i].from, edges[i].to);
		}
	}
	return ans;

}

//Note that start is inclusive and end is exclusive
long long filter_kruskal(vector<Edge> & edges, UnionFind & uf_kruskal,
	int start, int end){
	//We've already found all of our edges
	if (uf_kruskal.get_num_cc() == 1){
		return 0;
	}
	long long ans = 0;
	int n = end - start;
	//Use standard kruskal for small graphs
	if (n < filter_kruskal_threshold){
		return kruskal(edges, uf_kruskal, start, end);
	}
	long long pivot = edges[(rand() % end) + start].weight;
	//split is first element > pivot
	int split = partition(edges, pivot, start, end);
	ans += filter_kruskal(edges, uf_kruskal, start, split);
	int filter_split = filter(edges, uf_kruskal, split, end);
	ans += filter_kruskal(edges, uf_kruskal, split, filter_split);
	return ans;
}

int partition(vector<Edge> & edges, long long pivot, int start, int end){
	//Strided partitioning algorithm
	int p = omp_get_num_threads();
	vector<int> vi (p, 0);
	#pragma omp parallel for schedule(static, 1) 
	for (int i = 0; i < p; i++)
	{
		int left_ptr = start;
		int right_ptr = (end - start) - (end - start) % p + start;
		if (right_ptr == end){
			right_ptr -= p;
		}
		//Sequential partitioning algorithm
		while (left_ptr < right_ptr){
			if (edges[left_ptr].weight <= pivot){
				left_ptr += p;
			}
			else{
				//Swap with right_ptr
				Edge tmp = edges[right_ptr];
				edges[right_ptr] = edges[left_ptr];
				edges[left_ptr] = tmp;
				right_ptr -= p;
			}
		}
		if (edges[left_ptr].weight <= pivot){
			vi[i] = left_ptr + p;
		}
		else{
			vi[i] = left_ptr;
		}
	}
	int vmin = vi[0];
	int vmax = vi[0];
	for (int i = 0; i < p; i++){
		vmin = min(vmin, vi[i]);
		vmax = max(vmax, vi[i]);
	}
	//Sequential partitioning algorithm on vmin to vmax
	int left_ptr = vmin;
	int right_ptr = vmax;
	while (left_ptr < right_ptr){
		if (edges[left_ptr].weight <= pivot){
			left_ptr++;
		}
		else {
			//Swap with right_ptr
			Edge tmp = edges[right_ptr];
			edges[right_ptr] = edges[left_ptr];
			edges[left_ptr] = tmp;
			right_ptr++;
		}
	}
	if (edges[left_ptr].weight <= pivot){
		left_ptr++;
	}
	return left_ptr;
}

int filter(vector<Edge> & edges, UnionFind & uf_kruskal, int start, int end){
	vector<vector<Edge> > filter_results;
	int p = omp_get_num_threads();
	for (int i = 0; i < p; i++){
		filter_results.emplace_back();
	}
	#pragma omp parallel for
	for (int i = start; i < end; i++){
		int tid = omp_get_thread_num();
		if (!uf_kruskal.thread_safe_query(edges[i].from, edges[i].to)){
			filter_results[tid].push_back(edges[i]);
		}
	}
	int start_pt[p + 1];
	start_pt[0] = start;
	for (int i = 1; i <= p; i++){
		start_pt[i] = start_pt[i - 1] + filter_results[i - 1].size();
	}
	//Combine the filter results
	#pragma omp parallel for schedule(static, 1)
	for (int i = 0; i < p; i++){
		for (int j = 0; j < filter_results[i].size(); j++){
			edges[start_pt[i] + j] = filter_results[i][j];
		}
	}
	//This is the new endpoint
	return start_pt[p];
}



int main(){
	array<int,1/*5*/> n_vals = {10000/*, 50000, 100000, 500000, 1000000*/};
	int num_tc = 1/*5*/;
	int edges_per_node = 10;
	
	//Set a random seed
	srand(1337);

	//Set cout precision
	cout.precision(2);

	for (int a = 0; a < n_vals.size(); a++){
		for (int j = 0; j < num_tc; j++){
			int n = n_vals[a];
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

			//Run filter kruskal algorithm
			ans = filter_kruskal(edges, uf_kruskal, 0, edges.size());

			
			chrono::high_resolution_clock::time_point end_time = 
			chrono::high_resolution_clock::now();
			cout << "n: " << n << " tc: " << j << endl;
			cout << "Time Elapsed: " << scientific <<
			chrono::duration_cast<chrono::nanoseconds>(end_time - start_time).count() / 1e9
			<< " s" << endl;
			cout << "MST Weight: " << ans << endl;
		}
	}
}