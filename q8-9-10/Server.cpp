#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>
#include <sstream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <cstring>
#include <arpa/inet.h>
#include <sys/select.h>
#include <thread>
#include <mutex>
#include <condition_variable>
#include "Kosaraju.hpp"
#include "Proactor.hpp"

#define PORT 9036         // Port we're listening on
#define MAXCONNECTIONS 10

// Mutex and condition variable for synchronization
std::mutex graph_mutex;                 // POSIX mutex to protect the graph

std::condition_variable graph_cv;       // POSIX cond var to wake up monitor thread
bool significant_change = false;        // POSIX cond var flag

// Global instances
Kosaraju graph(0);
std::unordered_map<int, int> client_expected_edges;

void handle_client_command(int client_socket, const std::string& command);

void* client_thread_func(int client_fd) {
    char buf[256];
    int nbytes;

    graph_mutex.lock();
    while (true) {
        // non blocking using select
        fd_set readfds;
        FD_ZERO(&readfds);
        FD_SET(client_fd, &readfds);
        struct timeval tv;
        tv.tv_sec = 0;
        tv.tv_usec = 0;
        int retval = select(client_fd + 1, &readfds, nullptr, nullptr, &tv);
        if (retval == -1) {
            perror("select");
            break;
        } else if (retval == 0) {
            continue;
        }
        // Receive data from client
        nbytes = recv(client_fd, buf, sizeof(buf), 0);
        if (nbytes <= 0) {
            if (nbytes == 0) {
                std::cout << "Client " << client_fd << " disconnected.\n";
            } else {
                perror("recv");
            }
            close(client_fd);
            break;
        }

        buf[nbytes] = '\0';
        std::string command(buf);
        handle_client_command(client_fd, command);
    }

    return nullptr;
}

void handle_client_command(int client_socket, const std::string& command) {
    std::istringstream iss(command);
    std::string cmd;
    iss >> cmd;

    if (cmd == "Newgraph") {
        int vertices, edges;
        if (iss >> vertices >> edges) {

            graph.resetGraph(vertices);
            std::cout << "Graph created with " << vertices << " vertices. Waiting for " << edges << " edges.\n";
            std::string response = "Graph created. Send " + std::to_string(edges) + " edges (u v).\n";
            send(client_socket, response.c_str(), response.size(), 0);
            client_expected_edges[client_socket] = edges;
        } else {
            std::cout << "Error: Invalid graph command format\n";
        }
    } else if (client_expected_edges.find(client_socket) != client_expected_edges.end() && client_expected_edges[client_socket] > 0) {
        std::istringstream edge_stream(command);
        int u, v;
        while (edge_stream >> u >> v) {
            if (u > 0 && v > 0 && u <= graph.getNumVertices() && v <= graph.getNumVertices()) {
                graph.addArc(u, v);
                client_expected_edges[client_socket]--;
                std::cout << "Added edge from " << u << " to " << v << ". " << client_expected_edges[client_socket] << " edges remaining.\n";
                std::string response = "Edge added. " + std::to_string(client_expected_edges[client_socket]) + " edges remaining.\n";
                send(client_socket, response.c_str(), response.size(), 0);
                significant_change = true;
                graph_cv.notify_one(); // Notify the monitoring thread of a significant change
            } else {
                std::cout << "Error: Vertex index out of bounds\n";
            }
        }
        if (client_expected_edges[client_socket] == 0) {
            client_expected_edges.erase(client_socket);
            std::cout << "All edges received for client " << client_socket << ".\n";
        }
    } else if (cmd == "Newedge") {
        int u, v;
        if (iss >> u >> v) {
            if (u > 0 && v > 0 && u <= graph.getNumVertices() && v <= graph.getNumVertices()) {
                graph.addArc(u, v);
                std::cout << "Added edge from " << u << " to " << v << ".\n";
                significant_change = true;
                graph_cv.notify_one(); // Notify the monitoring thread of a significant change
            } else {
                std::cout << "Error: Vertex index out of bounds\n";
            }
        } else {
            std::cout << "Error: Invalid edge command format\n";
        }
    } else if (cmd == "Removeedge") {
        int u, v;
        if (iss >> u >> v) {
            if (u > 0 && v > 0 && u <= graph.getNumVertices() && v <= graph.getNumVertices()) {
                graph.removeArc(u, v);
                std::cout << "Removed edge from " << u << " to " << v << ".\n";
                significant_change = true;
                graph_cv.notify_one(); // Notify the monitoring thread of a significant change
            } else {
                std::cout << "Error: Vertex index out of bounds\n";
            }
        } else {
            std::cout << "Error: Invalid edge command format\n";
        }
    } else if (cmd == "Kosaraju") {
        std::vector<std::vector<int>> sccs = graph.findSCCs();
        std::cout << "Calculating SCCs using Kosaraju's algorithm.\n";
        std::cout << "Strongly Connected Components (SCCs):\n";
        for (const auto& scc : sccs) {
            for (int vertex : scc) {
                std::cout << vertex + 1 << " ";
            }
            std::cout << "\n";
        }
        significant_change = true;
        graph_cv.notify_one(); // Notify the monitoring thread after Kosaraju calculation
    } else {
        std::cout << "Unknown command.\n";
    }
    if ((client_expected_edges.find(client_socket) != client_expected_edges.end() && client_expected_edges[client_socket] == 0) || client_expected_edges.find(client_socket) == client_expected_edges.end()) {
        graph_mutex.unlock();
    }
}

void monitor_graph() {
    // using unique_lock instead of lock_guard to be able to wait on the condition variable with the lock
    std::unique_lock<std::mutex> lock(graph_mutex);
    
    while (true) {
        // Wait for a significant change in the graph (signaled by the significant_change flag from the client threads)
        graph_cv.wait(lock, []{ return significant_change; });

        // Check the current SCC status using Kasaraju
        auto sccs = graph.findSCCs();
        int num_vertices = graph.getNumVertices();
        bool majority_in_one_scc = false;
        
        for (const auto& scc : sccs) {
            if ((int)scc.size() >= num_vertices / 2) {
                majority_in_one_scc = true;
                break;
            }
        }

        if (majority_in_one_scc) {
            std::cout << "At least 50% of the graph belongs to the same SCC.\n";
        } else {
            std::cout << "At least 50% of the graph does NOT belong to the same SCC.\n";
        }
        
        // Reset significant change flag
        significant_change = false;
    }
}


int main() {
    // Create a socket
    int server = socket(AF_INET, SOCK_STREAM, 0);
    if (server == -1) {
        std::cerr << "socket: Could not create socket.\n";
        return 1;
    }

    // Set server address for binding
    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    // Allow reuse of address
    int yes = 1;
    if (setsockopt(server, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
        std::cerr << "setsockopt: Could not set socket options.\n";
        return 2;
    }

    // Bind to port
    if (bind(server, (sockaddr *) &serverAddr, sizeof(serverAddr)) == -1) {
        std::cerr << "bind: Could not bind to port " << PORT << ".\n";
        return 3;
    }

    // Listen on port for incoming connections
    if (listen(server, MAXCONNECTIONS) == -1) {
        std::cerr << "listen: Could not listen on port " << PORT << ".\n";
        // print error
        std::cout << "Error: " << strerror(errno) << "\n";
        return 4;
    }

    Proactor::startProactor(server, client_thread_func);

    std::thread monitor_thread(monitor_graph); // Start the monitoring thread

    // Clean up (not reached in the current code)
    monitor_thread.join(); // Wait for the monitoring thread to finish

    return 0;
}
