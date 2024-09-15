#ifndef KOSARAJU_LIST_HPP
#define KOSARAJU_LIST_HPP

#include <vector>
#include <list>
#include <stack>

class KosarajuList {
public:
    KosarajuList(int vertices);
    void addArc(int u, int v);
    void findSCCs();
private:
    int n;
    std::vector<std::list<int>> adj;
    std::vector<std::list<int>> transposeGraph();
    void dfs(int v, std::vector<std::list<int>> &adj, std::vector<bool> &visited, std::stack<int> &Stack);
    void dfsOnTranspose(int v, std::vector<std::list<int>> &adj, std::vector<bool> &visited, std::vector<int> &scc);
};

#endif
