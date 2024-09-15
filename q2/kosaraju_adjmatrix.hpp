#ifndef KOSARAJU_ADJMATRIX_HPP
#define KOSARAJU_ADJMATRIX_HPP

#include <vector>
#include <stack>

class KosarajuAdjMatrix {
public:
    KosarajuAdjMatrix(int vertices);
    void addArc(int u, int v);
    void findSCCs();
private:
    int n;
    std::vector<std::vector<bool>> adj;
    std::vector<std::vector<bool>> transposeGraph();
    void dfs(int v, std::vector<std::vector<bool>> &adj, std::vector<bool> &visited, std::stack<int> &Stack);
    void dfsOnTranspose(int v, std::vector<std::vector<bool>> &adj, std::vector<bool> &visited, std::vector<int> &scc);
};

#endif
