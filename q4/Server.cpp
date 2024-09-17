#include "Kosaraju.hpp"
#include <sys/wait.h>
#include <iostream>
#include <sstream>
#include <string>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <string.h>
#include <unordered_map>

/*
 * We will merge the program of phase 3 and the chat of beej.
 * Our goal is to combine them, that is, we will create a server that will hold a structure
 * Graph data (common to all users) and each logged in user can create a new graph, add an arc, download an arc
 * or order a kosaraju calculation on the current graph.
 * It can be assumed that the input is valid.
 * The communication protocol between the server and the clients will be textual (as in step 3) on port 9034 (the beej port)
 * Each user gives commands (as we gave in step 3) or orders the calculation of the algorithm.
 * A user giving commands that change the graph has to wait for the change or calculation to finish.
 *** Note that in this section we still don't need to protect the graph calculation with a mutex because we are still working in one trad
 * Therefore, there will be no change and calculation for the graph at the same time.
 * We also won't defend creating a graph, even though there could have been conflicts
 * At this point, let's assume the input is valid
*/

#define PORT 9035
#define MAXCONNECTIONS 10

using std::cout, std::cin, std::endl, std::string;

void handle_client_command(int client_socket, const string& command, Kosaraju& graph, std::unordered_map<int, int>& client_expected_edges);



void printSCCs(const std::vector<std::vector<int>>& sccs) {
    cout << "Strongly Connected Components (SCCs):\n";
    for (const auto& scc : sccs) {
        for (int vertex : scc) {
            cout << vertex + 1 << " "; // Output 1-based vertex indices
        }
        cout << endl;
    }
}

// ASSUMING INPUT IS CORRECT
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
        cout << "Error: " << strerror(errno) << "\n";
        return 4;
    }

    // Store client address and socket
    sockaddr_in clientAddr;
    socklen_t addrlen;
    int client;

    fd_set master;    // Master file descriptor list
    fd_set read_fds;  // Temp file descriptor list for select()
    int fdmax;        // Maximum file descriptor number

    char buf[256];    // Buffer for client data
    int nbytes;

    // Track expected edges for each client
    std::unordered_map<int, int> client_expected_edges; 

    // Clear master and temp sets
    FD_ZERO(&master);
    FD_ZERO(&read_fds);

    // Global graph for all clients
    Kosaraju graph(0);

    // Add the server to the master set
    FD_SET(server, &master);

    // Keep track of the biggest file descriptor
    fdmax = server; // So far, it's this one

    int i;
    while (1) {
        // Copy master file descriptor list
        read_fds = master;
        // select() blocks until an activity on one of the file descriptors
        if (select(fdmax+1, &read_fds, NULL, NULL, NULL) == -1) {
            perror("select");
            exit(4);
        }

        // Run through the existing connections looking for data to read
        for (i = 0; i <= fdmax; i++) {
            if (FD_ISSET(i, &read_fds)) { // We got one!!
                if (i == server) {
                    // Handle new connections
                    addrlen = sizeof clientAddr;
                    client = accept(server, (struct sockaddr *)&clientAddr, &addrlen);

                    if (client == -1) {
                        perror("accept");
                    } else {
                        FD_SET(client, &master); // Add to master set
                        if (client > fdmax) {    // Keep track of the max so far
                            fdmax = client;
                        }
                        std::cout << "new connection from " << inet_ntoa(clientAddr.sin_addr) << endl;
                    }
                } else {        // descriptor is not the server
                    // Handle data from a client
                    if ((nbytes = recv(i, buf, sizeof buf - 1, 0)) <= 0) {  // error
                        if (nbytes == 0) {
                            // Connection closed
                            std::cout << "Connection closed by " << inet_ntoa(clientAddr.sin_addr) << endl;
                        } else {
                            perror("recv");
                        }
                        close(i); // Close the connection on our side
                        FD_CLR(i, &master); // Remove from master set to not select on it
                        client_expected_edges.erase(i); // Remove client from the expected edges map
                    } else {        // recv succeeded
                        // We got some data from a client
                        buf[nbytes] = '\0';     // null terminate the string
                        string command(buf);
                        handle_client_command(i, command, graph, client_expected_edges);
                    }
                }
            }
        }
    }
    close(server);

    delete &graph;

    return 0;
}

void handle_client_command(int client_socket, const string& command, Kosaraju& graph, std::unordered_map<int, int>& client_expected_edges) {
    // Like we did in q3
    std::istringstream iss(command);
    string cmd;
    iss >> cmd;

    if (cmd == "Newgraph") {
        int vertices, edges;
        if (iss >> vertices >> edges) {
            graph.resetGraph(vertices);
            client_expected_edges[client_socket] = edges;
            std::cout << "Graph created with " << vertices << " vertices. Waiting for " << edges << " edges.\n";
            // Notify client to send edges
            string response = "Graph created. Send " + std::to_string(edges) + " edges (u v).\n";
            send(client_socket, response.c_str(), response.size(), 0);
        } else {
            std::cout << "Error: Invalid graph command format\n";
        }
    } else if (client_expected_edges.find(client_socket) != client_expected_edges.end() && client_expected_edges[client_socket] > 0) {
        // We are expecting edges from this client
        std::istringstream edge_stream(command);
        int u, v;
        while (edge_stream >> u >> v) {
            if (u > 0 && v > 0 && u <= graph.getNumVertices() && v <= graph.getNumVertices()) {
                graph.addArc(u, v);
                client_expected_edges[client_socket]--;     // got one edge
                std::cout << "Added edge from " << u << " to " << v << ". " << client_expected_edges[client_socket] << " edges remaining.\n";
                // Notify client about remaining edges
                string response = "Edge added. " + std::to_string(client_expected_edges[client_socket]) + " edges remaining.\n";
                send(client_socket, response.c_str(), response.size(), 0);
            } else {
                std::cout << "Error: Vertex index out of bounds\n";
            }
        }
        if (client_expected_edges[client_socket] == 0) {
            client_expected_edges.erase(client_socket);     // done receiving edges from this client
            std::cout << "All edges received for client " << client_socket << ".\n";
        }
    } else if (cmd == "Newedge") {
        // client_expected_edges[client_socket] = 1;       // its 0 before, otherwise it would have been caught in the previous if
        int u, v;
        if (iss >> u >> v) {
            if (u > 0 && v > 0 && u <= graph.getNumVertices() && v <= graph.getNumVertices()) {
                graph.addArc(u, v);
                std::cout << "Added edge from " << u << " to " << v << ".\n";
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
                std::cout << vertex + 1 << " "; // Adjust for 1-based indexing
            }
            std::cout << "\n";
        }
    } else {
        std::cout << "Unknown command.\n";
    }
}
