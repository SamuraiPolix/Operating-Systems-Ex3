#include "kosaraju_deque.hpp"
#include <iostream>
#include <algorithm>

KosarajuDeque::KosarajuDeque(int vertices) : n(vertices) {
    adj.resize(n);
    std::cout << "Initialized KosarajuDeque with " << vertices << " vertices.\n";
}

void KosarajuDeque::addArc(int u, int v) {
    if (u < 1 || u > n || v < 1 || v > n) {
        std::cerr << "Error: Invalid vertices (" << u << ", " << v << "). Vertices must be between 1 and " << n << ".\n";
        return;
    }
    adj[u - 1].push_back(v - 1); // Adjust for 0-based indexing
}

std::vector<std::deque<int>> KosarajuDeque::transposeGraph() {
    std::vector<std::deque<int>> transposed(n);
    for (int v = 0; v < n; ++v) {
        for (int i : adj[v]) {
            transposed[i].push_back(v);
        }
    }
    return transposed;
}

void KosarajuDeque::dfs(int v, std::vector<std::deque<int>> &adj, std::vector<bool> &visited, std::stack<int> &Stack) {
    visited[v] = true;
    for (int i : adj[v])
        if (!visited[i])
            dfs(i, adj, visited, Stack);
    Stack.push(v);
}

void KosarajuDeque::dfsOnTranspose(int v, std::vector<std::deque<int>> &adj, std::vector<bool> &visited, std::vector<int> &scc) {
    visited[v] = true;
    scc.push_back(v);
    for (int i : adj[v])
        if (!visited[i])
            dfsOnTranspose(i, adj, visited, scc);
}

void KosarajuDeque::findSCCs() {
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

    // Print all SCCs
    std::cout << "Strongly Connected Components (SCCs):\n";
    for (const auto& scc : allSCCs) {
        std::cout << "SCC: ";
        for (int vertex : scc)
            std::cout << vertex + 1 << " "; // Output 1-based vertex indices
        std::cout << std::endl;
    }
}
