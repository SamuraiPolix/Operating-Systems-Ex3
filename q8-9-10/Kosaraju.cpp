#include "Kosaraju.hpp"
#include <algorithm>

Kosaraju::Kosaraju(int vertices) : n(vertices) {
    adj.resize(n);
}

void Kosaraju::resetGraph(int vertices) {
    n = vertices;
    adj.clear();
    adj.resize(n);
}

void Kosaraju::addArc(int u, int v) {
    adj[u - 1].push_back(v - 1); // Adjust for 0-based indexing
}

void Kosaraju::removeArc(int u, int v) {
    auto it = std::find(adj[u - 1].begin(), adj[u - 1].end(), v - 1);
    if (it != adj[u - 1].end()) {
        adj[u - 1].erase(it);
    }
}

int Kosaraju::getNumVertices() const{
    return n;
}

std::vector<std::vector<int>> Kosaraju::findSCCs() {
    std::stack<int> Stack;
    std::vector<bool> visited(n, false);

    // Fill the stack with vertices based on finishing times
    for (int i = 0; i < n; ++i)
        if (!visited[i])
            dfs(i, visited, Stack);

    // Transpose the graph
    std::vector<std::vector<int>> transposed = transposeGraph();

    // Clear visited array for the second DFS
    std::fill(visited.begin(), visited.end(), false);

    std::vector<std::vector<int>> allSCCs; // To collect all SCCs

    // Process all vertices in the order defined by the stack
    while (!Stack.empty()) {
        int v = Stack.top();
        Stack.pop();

        if (!visited[v]) {
            std::vector<int> scc;
            dfs(v, visited, scc, transposed);
            allSCCs.push_back(scc); // Collect SCC
        }
    }

    return allSCCs; // Return SCCs instead of printing
}

void Kosaraju::dfs(int v, std::vector<bool>& visited, std::stack<int>& Stack) {
    visited[v] = true;
    for (int i : adj[v])
        if (!visited[i])
            dfs(i, visited, Stack);
    Stack.push(v);
}

void Kosaraju::dfs(int v, std::vector<bool>& visited, std::vector<int>& scc, const std::vector<std::vector<int>>& adj) {
    visited[v] = true;
    scc.push_back(v);
    for (int i : adj[v]) {
        if (!visited[i]) {
            dfs(i, visited, scc, adj);
        }
    }
}

std::vector<std::vector<int>> Kosaraju::transposeGraph() {
    std::vector<std::vector<int>> transposed(n);
    for (int v = 0; v < n; ++v) {
        for (int i : adj[v]) {
            transposed[i].push_back(v);
        }
    }
    return transposed;
}
