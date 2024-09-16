#include "Kosaraju.hpp"
#include <iostream>
#include <algorithm>

Kosaraju::Kosaraju(int vertices) : n(vertices) {
    adj.resize(n);
    // std::cout << "Initialized Kosaraju with " << vertices << " vertices.\n";
}

void Kosaraju::resetGraph(int vertices) {
    n = vertices;
    adj.clear();
    adj.resize(n);
}

int Kosaraju::removeArc(int u, int v) {
    if (u < 1 || u > n || v < 1 || v > n) {
        std::cerr << "Error: Invalid vertices (" << u << ", " << v << "). Vertices must be between 1 and " << n << ".\n";
        return -1;
    }
    auto it = std::find(adj[u - 1].begin(), adj[u - 1].end(), v - 1);
    if (it != adj[u - 1].end()) {
        adj[u - 1].erase(it);
        return 0;
    }
    std::cerr << "Error: Edge from " << u << " to " << v << " does not exist.\n";
    return -1;
}

int Kosaraju::addArc(int u, int v) {
    if (u < 1 || u > n || v < 1 || v > n) {
        std::cerr << "Error: Invalid vertices (" << u << ", " << v << "). Vertices must be between 1 and " << n << ".\n";
        return -1;
    }
    if (std::find(adj[u - 1].begin(), adj[u - 1].end(), v - 1) != adj[u - 1].end()) {
        std::cerr << "Error: Edge from " << u << " to " << v << " already exists.\n";
        return -1;
    }
    adj[u - 1].push_back(v - 1); // Adjust for 0-based indexing
    return 0;
}

std::vector<std::deque<int>> Kosaraju::transposeGraph() {
    std::vector<std::deque<int>> transposed(n);
    for (int v = 0; v < n; ++v) {
        for (int i : adj[v]) {
            transposed[i].push_back(v);
        }
    }
    return transposed;
}

void Kosaraju::dfs(int v, std::vector<std::deque<int>> &adj, std::vector<bool> &visited, std::stack<int> &Stack) {
    visited[v] = true;
    for (int i : adj[v])
        if (!visited[i])
            dfs(i, adj, visited, Stack);
    Stack.push(v);
}

void Kosaraju::dfsOnTranspose(int v, std::vector<std::deque<int>> &adj, std::vector<bool> &visited, std::vector<int> &scc) {
    visited[v] = true;
    scc.push_back(v);
    for (int i : adj[v])
        if (!visited[i])
            dfsOnTranspose(i, adj, visited, scc);
}

std::vector<std::vector<int>> Kosaraju::findSCCs() {
    std::stack<int> Stack;
    std::vector<bool> visited(n, false);

    // Fill the stack with vertices based on finishing times
    for (int i = 0; i < n; ++i)
        if (!visited[i])
            dfs(i, adj, visited, Stack);

    // Transpose the graph
    std::vector<std::deque<int>> transposed = transposeGraph();

    // Clear visited array for the second DFS
    std::fill(visited.begin(), visited.end(), false);

    std::vector<std::vector<int>> allSCCs; // To collect all SCCs

    // Process all vertices in the order defined by the stack
    while (!Stack.empty()) {
        int v = Stack.top();
        Stack.pop();

        if (!visited[v]) {
            std::vector<int> scc;
            dfsOnTranspose(v, transposed, visited, scc);
            allSCCs.push_back(scc); // Collect SCC
        }
    }

    return allSCCs;
}
