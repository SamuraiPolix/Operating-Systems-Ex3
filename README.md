# Synchronization and Kosaraju server
***Ex3 in Operating Systems @ Ariel University***



## Questions 1-3

Implemented a graph in 3 ways, used profiling to determine the best implementation - Deque

Using Kosaraju's algorithms, we can find the Strongly Connected Components (SCC) in the graph.

## Questions 4-10

Implemented a server that allows multiple clients to connect to it at the same time.

The server receives graphs and changes in them, from multiple users in TCP communication or from stdin, makes the changes simultaneously, runs the algorithm, and returns outputs.

Each client is assigned a pthread to deal with it's data and everything is being done while protecting the graph using mutex.
