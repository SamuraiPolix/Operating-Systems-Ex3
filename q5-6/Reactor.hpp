#ifndef REACTOR_HPP
#define REACTOR_HPP

#include <unordered_map>
#include <vector>
#include <functional>
#include <poll.h>

// REACTOR IMPLEMENTATION USING POLL

/*
 * A template reactor library. The reactor will receive a fd and a function to call it when the fd is hot.
 * Assuming a valid input (if you add fd to the reactor it does not exist, f you remove fd from the reactor, it exists)
*/

typedef void* (*reactorFunc)(int fd);

class Reactor {
public:
    Reactor();
    ~Reactor();

    // starts a new reactor and returns a pointer to it
    static void* startReactor();

    // adds fd to Reactor (for reading) ; returns 0 on success.
    static int addFdToReactor(void* reactor, int fd, reactorFunc func);

    // removes fd from reactor
    static int removeFdFromReactor(void* reactor, int fd);

    // stops reactor
    static int stopReactor(void* reactor);

    void run();

private:
    std::unordered_map<int, reactorFunc> handlers;
    std::vector<struct pollfd> pollfds;
    bool isActive;
};



#endif // REACTOR_HPP