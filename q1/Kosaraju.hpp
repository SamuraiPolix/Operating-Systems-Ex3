#ifndef KOSARAJU_HPP
#define KOSARAJU_HPP

#include <vector>
#include <stack>

class Kosaraju {
public:
    Kosaraju(int vertices);
    void addArc(int u, int v);
    void findSCCs();

private:
    int n; // Number of vertices
    std::vector<std::vector<int>> adj; // Adjacency list
    std::vector<std::vector<int>> transposeGraph();
    void dfs(int v, std::vector<std::vector<int>> &adj, std::vector<bool> &visited, std::stack<int> &Stack);
    void dfsOnTranspose(int v, std::vector<std::vector<int>> &adj, std::vector<bool> &visited, std::vector<int> &scc);
};

#endif // KOSARAJU_HPP
