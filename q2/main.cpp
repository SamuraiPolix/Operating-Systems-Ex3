#include "kosaraju_list.hpp"
#include "kosaraju_deque.hpp"
#include "kosaraju_adjmatrix.hpp"
#include <iostream>
#include <string>

int main() {
    int n, m;
    std::cout << "Enter number of vertices: ";
    std::cin >> n;
    std::cout << "Enter number of arcs: ";
    std::cin >> m;

    std::string implementation;
    std::cout << "Choose implementation (list, deque, adjmatrix): ";
    std::cin >> implementation;

    if (implementation == "list") {
        KosarajuList kosaraju(n);

        std::cout << "Enter arcs (fromVertice toVertice):\n";
        for (int i = 0; i < m; ++i) {
            int u, v;
            std::cin >> u >> v;
            kosaraju.addArc(u, v);
        }

        std::cout << "Starting Kosaraju's algorithm to find SCCs.\n";
        kosaraju.findSCCs();
        std::cout << "Finished finding SCCs.\n";
    } else if (implementation == "deque") {
        KosarajuDeque kosaraju(n);

        std::cout << "Enter arcs (fromVertice toVertice):\n";
        for (int i = 0; i < m; ++i) {
            int u, v;
            std::cin >> u >> v;
            kosaraju.addArc(u, v);
        }

        std::cout << "Starting Kosaraju's algorithm to find SCCs.\n";
        kosaraju.findSCCs();
        std::cout << "Finished finding SCCs.\n";
    } else if (implementation == "adjmatrix") {
        KosarajuAdjMatrix kosaraju(n);

        std::cout << "Enter arcs (fromVertice toVertice):\n";
        for (int i = 0; i < m; ++i) {
            int u, v;
            std::cin >> u >> v;
            kosaraju.addArc(u, v);
        }

        std::cout << "Starting Kosaraju's algorithm to find SCCs.\n";
        kosaraju.findSCCs();
        std::cout << "Finished finding SCCs.\n";
    } else {
        std::cout << "Invalid choice of implementation. Please choose either 'list', 'deque', or 'adjmatrix'.\n";
    }

    return 0;
}
