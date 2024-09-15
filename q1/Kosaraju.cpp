#include "Kosaraju.hpp"
#include <iostream>
#include <algorithm>

// Q1: Kosaraju's Algorithm to find Strongly Connected Components for a directed graph
/*
Input from the standard input 2 numbers. +LF
The first number describes the number of vertices in the graph.
The second number describes the number of arcs in the graph.
The first number (the vertices) is called n.
The second number (the arcs) is called m.
The vertices of the graph are numbered as size_ts from 1 to n.
After reading the first pair, m more pairs are called, each pair representing an arc
The pair (1,2) will represent the arc from 1 to 2.
 */

using std::vector, std::cout, std::endl, std::stack, std::fill;

Kosaraju::Kosaraju(int vertices) : n(vertices) {
    adj.resize(n);
    // Transpose while adding arcs
    // adjT.resize(n);
    #ifdef DEBUG
        cout << "Initialized Kosaraju with " << vertices << " vertices.\n";
    #endif
}

void Kosaraju::addArc(int u, int v) {
    adj[u - 1].push_back(v - 1); // Adjust for 0-based indexing
    // Transpose while adding arcs
    // adjT[v - 1].push_back(u - 1); // Adjust for 0-based indexing
}

vector<vector<int>> Kosaraju::transposeGraph() {
    vector<vector<int>> transposed(n);
    for(int v = 0; v < n; ++v) {
        for(int i : adj[v]) {
            transposed[i].push_back(v);
        }
    }
    return transposed;
}

void Kosaraju::dfs(int v, vector<vector<int>> &adj, vector<bool> &visited, stack<int> &Stack) {
    visited[v] = true;
    for(int i : adj[v])
        if(!visited[i])
            dfs(i, adj, visited, Stack);
    Stack.push(v);
}

void Kosaraju::dfsOnTranspose(int v, vector<vector<int>> &adj, vector<bool> &visited, vector<int> &scc) {
    visited[v] = true;
    scc.push_back(v);
    for(int i : adj[v])
        if(!visited[i])
            dfsOnTranspose(i, adj, visited, scc);
}

void Kosaraju::findSCCs() {
    stack<int> stk;
    vector<bool> visited(n, false);

    // First DFS - Fill the stack with vertices based on finishing times
    for(int i = 0; i < n; ++i)
        if(!visited[i])
            dfs(i, adj, visited, stk);

    // Transpose the graph 
    vector<vector<int>> transposed = transposeGraph();

    // Clear visited array for the second DFS
    fill(visited.begin(), visited.end(), false);

    vector<vector<int>> allSCCs; // To collect all SCCs

    // Second DFS (on the transpose of the graph) - Process all vertices in the order defined by the stack
    while(!stk.empty()) {
        int v = stk.top();
        stk.pop();
        
        if(!visited[v]) {
            vector<int> scc;
            dfsOnTranspose(v, transposed, visited, scc);
            allSCCs.push_back(scc); // Collect SCC
        }
    }

    // Print all SCCs
    #ifdef DEBUG
        cout << "Strongly Connected Components (SCCs):\n";
    #endif
    for(const auto& scc : allSCCs) {
        #ifdef DEBUG
            cout << "SCC: ";
        #endif
        for(int vertex : scc)
            cout << vertex + 1 << " "; // Output 1-based vertex indices
        cout << endl;
    }
}
