#ifndef KOSARAJU_DEQUE_HPP
#define KOSARAJU_DEQUE_HPP

#include <vector>
#include <deque>
#include <stack>

class KosarajuDeque {
public:
    KosarajuDeque(int vertices);
    void addArc(int u, int v);
    void findSCCs();
private:
    int n;
    std::vector<std::deque<int>> adj;
    std::vector<std::deque<int>> transposeGraph();
    void dfs(int v, std::vector<std::deque<int>> &adj, std::vector<bool> &visited, std::stack<int> &Stack);
    void dfsOnTranspose(int v, std::vector<std::deque<int>> &adj, std::vector<bool> &visited, std::vector<int> &scc);
};

#endif
