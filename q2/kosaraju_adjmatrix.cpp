#include "kosaraju_adjmatrix.hpp"
#include <iostream>
#include <algorithm>

KosarajuAdjMatrix::KosarajuAdjMatrix(int vertices) : n(vertices) {
    adj.resize(n, std::vector<bool>(n, false));
    std::cout << "Initialized KosarajuAdjMatrix with " << vertices << " vertices.\n";
}

void KosarajuAdjMatrix::addArc(int u, int v) {
    if (u < 1 || u > n || v < 1 || v > n) {
        std::cerr << "Error: Invalid vertices (" << u << ", " << v << "). Vertices must be between 1 and " << n << ".\n";
        return;
    }
    adj[u - 1][v - 1] = true; // Adjust for 0-based indexing
}

std::vector<std::vector<bool>> KosarajuAdjMatrix::transposeGraph() {
    std::vector<std::vector<bool>> transposed(n, std::vector<bool>(n, false));
    for (int v = 0; v < n; ++v) {
        for (int i = 0; i < n; ++i) {
            if (adj[v][i]) {
                transposed[i][v] = true;
            }
        }
    }
    return transposed;
}

void KosarajuAdjMatrix::dfs(int v, std::vector<std::vector<bool>> &adj, std::vector<bool> &visited, std::stack<int> &Stack) {
    visited[v] = true;
    for (int i = 0; i < n; ++i) {
        if (adj[v][i] && !visited[i])
            dfs(i, adj, visited, Stack);
    }
    Stack.push(v);
}

void KosarajuAdjMatrix::dfsOnTranspose(int v, std::vector<std::vector<bool>> &adj, std::vector<bool> &visited, std::vector<int> &scc) {
    visited[v] = true;
    scc.push_back(v);
    for (int i = 0; i < n; ++i) {
        if (adj[v][i] && !visited[i])
            dfsOnTranspose(i, adj, visited, scc);
    }
}

void KosarajuAdjMatrix::findSCCs() {
    std::stack<int> Stack;
    std::vector<bool> visited(n, false);

    // Fill the stack with vertices based on finishing times
    for (int i = 0; i < n; ++i)
        if (!visited[i])
            dfs(i, adj, visited, Stack);

    // Transpose the graph
    std::vector<std::vector<bool>> transposed = transposeGraph();

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

    // Print all SCCs
    std::cout << "Strongly Connected Components (SCCs):\n";
    for (const auto& scc : allSCCs) {
        std::cout << "SCC: ";
        for (int vertex : scc)
            std::cout << vertex + 1 << " "; // Output 1-based vertex indices
        std::cout << std::endl;
    }
}
