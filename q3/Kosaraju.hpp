#ifndef KOSARAJU_HPP
#define KOSARAJU_HPP

#include <vector>
#include <deque>
#include <stack>

// DEQUE

class Kosaraju {
private:
    int n;
    std::vector<std::deque<int>> adj;
    std::vector<std::deque<int>> transposeGraph();

public:
    Kosaraju(int vertices);
    int addArc(int u, int v);
    std::vector<std::vector<int>> findSCCs(); // Modify return type to vector of SCCs
    void resetGraph(int vertices);
    int removeArc(int u, int v);

private:
    void dfs(int v, std::vector<std::deque<int>> &adj, std::vector<bool> &visited, std::stack<int> &Stack);
    void dfsOnTranspose(int v, std::vector<std::deque<int>> &adj, std::vector<bool> &visited, std::vector<int> &scc);
};

#endif
