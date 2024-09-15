#include "Kosaraju.hpp"
#include <iostream>

using namespace std;

int main() {
        int n, m;
        #ifdef DEBUG
            cout << "Choose number of vertices: ";
        #endif
        cin >> n;
        #ifdef DEBUG
            cout << "Choose the number of edges: ";
        #endif
        cin >> m;

    Kosaraju kosaraju(n);

    // Reading the arcs
    for(int i = 0; i < m; ++i) {
        int u, v;
        #ifdef DEBUG
            cout << "Choose fromVertice: ";
        #endif
        cin >> u;
        #ifdef DEBUG
            cout << "Choose toVertice: ";
        #endif
        cin >> v;
        kosaraju.addArc(u, v);
    }

    #ifdef DEBUG
        std::cout << "Starting Kosaraju's algorithm to find SCCs.\n";
    #endif
    // Finding and printing SCCs
    kosaraju.findSCCs();

    #ifdef DEBUG
        std::cout << "Finished finding SCCs.\n";
    #endif
    return 0;
}
