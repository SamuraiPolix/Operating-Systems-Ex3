#include "Kosaraju.hpp"
#include <iostream>
#include <sstream>
#include <string>

using std::cout;

void printSCCs(const std::vector<std::vector<int>>& sccs) {
    cout << "Strongly Connected Components (SCCs):\n";
    for (const auto& scc : sccs) {
        for (int vertex : scc) {
            cout << vertex + 1 << " "; // Output 1-based vertex indices
        }
        cout << std::endl;
    }
}

// ASSUMING INPUT IS CORRECT
int main() {
    Kosaraju kosaraju(0); // Initialize with 0 vertices initially

    std::string command;
    // We are using getline to read the entire line, as the command may have spaces - e.g., Newgraph 3 2
    // (Every command should end with LF according to assignment)
    while (std::getline(std::cin, command)) {       // pulls one line (until \n) to command
        std::istringstream iss(command);        // create a stream from the command to act as if it was cin
        std::string cmd;                // create a string to store the cmd command
        iss >> cmd;             // read the first word from the command and store it in cmd

        if (cmd == "Newgraph") {

            int n, m;
            iss >> n >> m;      // read the next two integers from the command and store them in n and m
            kosaraju.resetGraph(n); // Reset graph with n vertices

            // Read m pairs of edges
            for (int i = 0; i < m; ++i) {
                std::getline(std::cin, command);
                std::istringstream issEdge(command);
                int u, v;
                issEdge >> u >> v;
                kosaraju.addArc(u, v);
            }

            cout << "Graph created with " << n << " vertices and " << m << " edges.\n";
        } else if (cmd == "Kosaraju") {
            cout << "Calculating SCCs using Kosaraju's algorithm...\n";
            std::vector<std::vector<int>> sccs = kosaraju.findSCCs();
            printSCCs(sccs);
        } else if (cmd == "Newedge") {
            int i, j;
            iss >> i >> j;
            kosaraju.addArc(i, j);
            cout << "Added edge from " << i << " to " << j << ".\n";
        } else if (cmd == "Removeedge") {
            int i, j;
            iss >> i >> j;
            kosaraju.removeArc(i, j);
            cout << "Removed edge from " << i << " to " << j << ".\n";
        } else {
            cout << "Invalid command: " << cmd << ". Skipping...\n";
        }
    }

    return 0;
}
