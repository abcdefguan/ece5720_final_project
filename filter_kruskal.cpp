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
			ans += edges[i].weight;
			uf_kruskal.join(edges[i].from, edges[i].to);
		}
	}
	return ans;

}

//Alternative partition => It's 2x slower
/*int partition(vector<Edge> & edges, long long pivot, int start, int end){
	vector<unique_ptr<vector<Edge> > > v_less_equal;
	vector<unique_ptr<vector<Edge> > > v_greater;
	for (int i = 0; i < p; i++){
		v_less_equal.emplace_back(make_unique<vector<Edge> >());
		v_greater.emplace_back(make_unique<vector<Edge> >());
	}
	#pragma omp parallel for num_threads(p)
	for (int i = start; i < end; i++){
		int tid = omp_get_thread_num();
		if (edges[i].weight <= pivot){
			v_less_equal[tid]->push_back(edges[i]);
		}
		else{
			v_greater[tid]->push_back(edges[i]);
		}
	}
	int less_equal_slots[p + 1];
	int greater_slots[p + 1];
	less_equal_slots[0] = start;
	for (int i = 1; i <= p; i++){
		less_equal_slots[i] = less_equal_slots[i - 1] + v_less_equal[i - 1]->size();
	}
	greater_slots[0] = less_equal_slots[p];
	for (int i = 1; i <= p; i++){
		greater_slots[i] = greater_slots[i - 1] + v_greater[i - 1]->size();
	}
	#pragma omp parallel for schedule(static, 1) num_threads(p)
	for (int i = 0; i < p; i++){
		for (int j = 0; j < v_less_equal[i]->size(); j++){
			edges[less_equal_slots[i] + j] = (*(v_less_equal[i]))[j];
		}
		for (int j = 0; j < v_greater[i]->size(); j++){
			edges[greater_slots[i] + j] = (*(v_greater[i]))[j];
		}
	}
	return less_equal_slots[p];
}*/

//Partition algorithm => Slows down when parallelized (likely due to cache misses)
int partition(vector<Edge> & edges, long long pivot, int start, int end){
	//Strided partitioning algorithm
	unique_ptr<vector<int> > vi (new vector<int> (p, 0));
	#pragma omp parallel for schedule(static, 1) num_threads(p) 
	for (int i = 0; i < p; i++)
	{
		int left_ptr = start + i;
		int right_ptr = (end - start) - ((end - start) % p) + start;
		if (right_ptr >= end){
			right_ptr -= p;
		}
		right_ptr += i;
		if (right_ptr >= end){
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
			(*vi)[i] = left_ptr + p;
		}
		else{
			(*vi)[i] = left_ptr;
		}
	}
	int vmin = (*vi)[0];
	int vmax = (*vi)[0];
	for (int i = 0; i < p; i++){
		vmin = min(vmin, (*vi)[i]);
		vmax = max(vmax, (*vi)[i]);
	}
	vmin = max(start, vmin);
	vmax = min(end - 1, vmax);
	vmin = min(vmin, vmax);

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
			right_ptr--;
		}
	}
	if (edges[left_ptr].weight <= pivot){
		left_ptr++;
	}
	return left_ptr;
}

int filter(vector<Edge> & edges, UnionFind & uf_kruskal, int start, int end){
	unique_ptr<vector<vector<Edge> > > filter_results(new vector<vector<Edge> > ());
	
	for (int i = 0; i < p; i++){
		filter_results->emplace_back();
	}
	#pragma omp parallel for num_threads(p)
	for (int i = start; i < end; i++){
		int tid = omp_get_thread_num();
		if (!uf_kruskal.thread_safe_query(edges[i].from, edges[i].to)){
			(*filter_results)[tid].push_back(edges[i]);
		}
	}
	int start_pt[p + 1];
	start_pt[0] = start;
	for (int i = 1; i <= p; i++){
		start_pt[i] = start_pt[i - 1] + (*filter_results)[i - 1].size();
	}
	//Combine the filter results
	#pragma omp parallel for schedule(static, 1) num_threads(p)
	for (int i = 0; i < p; i++){
		for (int j = 0; j < (*filter_results)[i].size(); j++){
			edges[start_pt[i] + j] = (*filter_results)[i][j];
		}
	}
	//This is the new endpoint
	return start_pt[p];
}

//Note that start is inclusive and end is exclusive
long long filter_kruskal(vector<Edge> & edges, UnionFind & uf_kruskal,
	int start, int end, int depth){

	//cout << "depth: " << depth << "size: " << (end - start) << endl;

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
	long long pivot = edges[(rand() % (end - start)) + start].weight;
	//split is first element > pivot
	//Force partition with 1 thread only
	int prev_p = p;
	p = 1;
	int split = partition(edges, pivot, start, end);
	p = prev_p;
	//Partition is failing, use normal kruskal (has to do with distribution of edge weights)
	if (split == end){
		/*if (end - start > 1000){
			cout << "Warning: Large partition failure of " << end - start << endl;
			cout << "pivot was " << pivot << endl;
		}*/
		return kruskal(edges, uf_kruskal, start, end);
	}
	ans += filter_kruskal(edges, uf_kruskal, start, split, depth + 1);
	//int prev_p = p;
	//p = 1;
	int filter_split = filter(edges, uf_kruskal, split, end);
	//p = prev_p;
	ans += filter_kruskal(edges, uf_kruskal, split, filter_split, depth + 1);
	return ans;
}



int main(int argc, char ** argv){

	if (argc < 4){
		cout << "Usage: " << argv[0] << " n e p" << endl;
		cout << "n: number of nodes in graph" << endl;
		cout << "e: number of edges per node" << endl;
		cout << "p: number of threads" << endl;
		return 1;
	}

	//Set number of threads
	int n = atoi(argv[1]);
	int edges_per_node = atoi(argv[2]);
	p = atoi(argv[3]);
	omp_set_num_threads(p);

	//Set cout precision
	cout.precision(2);
	//Set a random seed
	srand(1337);
	unique_ptr<UnionFind> uf (new UnionFind(n));
	unique_ptr<vector<Edge> > edges (new vector<Edge> ());
	edges->reserve(n * edges_per_node);

	bool is_connected = false;
	while (!is_connected){
		for (int node_num = 0; node_num < n; node_num++){
			for (int edge_num = 0; edge_num < edges_per_node; edge_num++){
				//Edge from node_num to edge_num
				long long weight = (rand() % 100000000) + 1;
				//cout << weight << endl;
				int target = rand() % n;
				edges->emplace_back(node_num, target, weight);
				//cout << node_num << " " << target << " " << weight << endl;
				uf->join(node_num, target);
			}
		}
		is_connected = true;
		for (int node_num = 0; node_num < n; node_num++){
			if (!uf->query(node_num, 0)){
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

	unique_ptr<UnionFind> uf_kruskal(new UnionFind (n));

	//Run filter kruskal algorithm
	ans = filter_kruskal(*edges.get(), *uf_kruskal.get(), 0, edges->size(), 0);


	chrono::high_resolution_clock::time_point end_time = 
	chrono::high_resolution_clock::now();
	cout << "n: " << n << " e: " << edges_per_node << " p: " << p << endl;
	cout << "Time Elapsed: " << scientific <<
	chrono::duration_cast<chrono::nanoseconds>(end_time - start_time).count() / 1e9
	<< " s" << endl;
	cout << "MST Weight: " << ans << endl;
}