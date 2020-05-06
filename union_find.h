#include <vector>

using namespace std;

class UnionFind {
private:
	vector<int> connected;

	int num_cc;

public:
	int n;

	UnionFind(int _n);

	int parent(int node);

	void join(int node1, int node2);

	bool query(int node1, int node2);

	int thread_safe_parent(int node);

	bool thread_safe_query(int node1, int node2);

	int get_num_cc();

	~UnionFind();

};