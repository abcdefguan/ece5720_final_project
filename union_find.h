#include <vector>

using namespace std;

class UnionFind {
private:
	vector<int> connected;

public:
	int n;

	UnionFind(int _n);

	int parent(int node);

	void join(int node1, int node2);

	bool query(int node1, int node2);

	~UnionFind();

};