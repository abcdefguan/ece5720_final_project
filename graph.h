#include <vector>
#include <unordered_map>

using namespace std;

class Graph {
public:
	vector<unordered_map<int, long long> > adjlist;
	int n;

	Graph(int _n);
	void join(int node1, int node2, long long weight);
	~Graph();
};