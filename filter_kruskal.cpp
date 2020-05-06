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
int p; //Total number of threads

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
long long kruskal(vector<Edge> & edges, UnionFind & uf_kruskal,
	int start, int end){
	long long ans = 0;
	//Sort edges
	sort(edges.begin() + start, edges.begin() + end, compareEdges);
	//Create union find
	for (int i = start; i < end; i++){
		if (!uf_kruskal.query(edges[i].from, edges[i].to)){
			cout << "Taken: " << edges[i].from << " " << edges[i].to << " " << edges[i].weight << endl;
			ans += edges[i].weight;
			uf_kruskal.join(edges[i].from, edges[i].to);
		}
	}
	return ans;

}

int partition(vector<Edge> & edges, long long pivot, int start, int end){
	//Strided partitioning algorithm
	vector<int> vi (p, 0);
	#pragma omp parallel for schedule(static, 1) num_threads(p) 
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
	//Verify correct partitioning
	/*for (int i = start; i < end; i++){
		if (i < left_ptr && edges[i].weight > pivot){
			cout << "Error with index: " << i << endl;
			cout << "Left Pointer is: " << left_ptr << endl;
		}
		else if (i >= left_ptr && edges[i].weight <= pivot){
			cout << "Error with index: " << i << endl;
			cout << "Left Pointer is: " << left_ptr << endl;
		}
	}*/
	return left_ptr;
}

int filter(vector<Edge> & edges, UnionFind & uf_kruskal, int start, int end){
	cout << "Filtering..." << endl;
	cout << "UF kruskal: " << endl;
	for (int i = 0; i < 5; i++){
		cout << uf_kruskal.parent(i) << " ";
	}
	vector<vector<Edge> > filter_results;
	cout << endl;
	for (int i = 0; i < p; i++){
		filter_results.emplace_back();
	}
	cout << "UF kruskal: " << endl;
	for (int i = 0; i < 5; i++){
		cout << uf_kruskal.parent(i) << " ";
	}
	cout << endl;
	#pragma omp parallel for num_threads(p)
	for (int i = start; i < end; i++){
		int tid = omp_get_thread_num();
		cout << "UF kruskal: " << endl;
		for (int i = 0; i < 5; i++){
			cout << uf_kruskal.parent(i) << " ";
		}
		cout << endl;
		cout << edges[i].from << " " << edges[i].to << " " << edges[i].weight << endl;
		cout << uf_kruskal.parent(edges[i].from) << " " << uf_kruskal.parent(edges[i].to) << endl;
		if (!uf_kruskal.thread_safe_query(edges[i].from, edges[i].to)){
			filter_results[tid].push_back(edges[i]);
		}
	}
	//Check filter results
	cout << "Filter results: " << endl;
	for (int i = 0; i < p; i++){
		cout << "[" << i << "]: ";
		for (int j = 0; j < filter_results[i].size(); j++){
			cout << filter_results[i][j].weight << " ";
		}
		cout << endl;
	}
	int start_pt[p + 1];
	start_pt[0] = start;
	for (int i = 1; i <= p; i++){
		start_pt[i] = start_pt[i - 1] + filter_results[i - 1].size();
	}
	//Combine the filter results
	#pragma omp parallel for schedule(static, 1) num_threads(p)
	for (int i = 0; i < p; i++){
		for (int j = 0; j < filter_results[i].size(); j++){
			edges[start_pt[i] + j] = filter_results[i][j];
		}
	}
	//This is the new endpoint
	return start_pt[p];
}

//Note that start is inclusive and end is exclusive
long long filter_kruskal(vector<Edge> & edges, UnionFind & uf_kruskal,
	int start, int end){
	cout << "Filter Kruskal called with " << start << " " << end << endl;
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
	cout << "Pivot was: " << pivot << endl;
	//split is first element > pivot
	int split = partition(edges, pivot, start, end);
	cout << "Partition yielded: " << endl;
	for (int i = start; i < end; i++){
		cout << edges[i].weight << " ";
	}
	cout << endl;
	cout << "split: " << split << endl;
	ans += filter_kruskal(edges, uf_kruskal, start, split);
	int filter_split = filter(edges, uf_kruskal, split, end);
	cout << "filter_split: " << filter_split << endl;
	cout << "Filtered result was: " << endl;
	for (int i = split; i < filter_split; i++){
		cout << edges[i].weight << " ";
	}
	cout << endl;
	ans += filter_kruskal(edges, uf_kruskal, split, filter_split);
	return ans;
}



int main(){
	array<int,1/*5*/> n_vals = {5/*, 50000, 100000, 500000, 1000000*/};
	int num_tc = 1/*5*/;
	int edges_per_node = 2;

	//Set number of threads
	p = 4;
	omp_set_num_threads(p);

	//Set cout precision
	cout.precision(2);
	for (int a = 0; a < n_vals.size(); a++){
		for (int j = 0; j < num_tc; j++){
			//Set a random seed
			srand(1337 + j);
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
						cout << node_num << " " << target << " " << weight << endl;
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

			UnionFind uf_kruskal(n);

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