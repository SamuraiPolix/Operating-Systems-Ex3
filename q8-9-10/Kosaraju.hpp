#ifndef KOSARAJU_HPP
#define KOSARAJU_HPP

#include <vector>
#include <stack>
#include <iostream>

class Kosaraju {
private:
    int n; // Number of vertices
    std::vector<std::vector<int>> adj; // Adjacency list

    void dfs(int v, std::vector<bool> &visited, std::stack<int> &Stack);
    void dfs(int v, std::vector<bool> &visited, std::vector<int> &scc, const std::vector<std::vector<int>> &adj);
    std::vector<std::vector<int>> transposeGraph();

public:
    Kosaraju(int vertices);
    void resetGraph(int vertices);
    void addArc(int u, int v);
    void removeArc(int u, int v);
    int getNumVertices() const;
    std::vector<std::vector<int>> findSCCs(); // Modify return type to vector of SCCs
};

#endif // KOSARAJU_HPP
