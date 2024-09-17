#include "Reactor.hpp"
#include <iostream>
#include <stdexcept>
#include <cstring>
#include <cerrno>
#include <algorithm>

Reactor::Reactor() : isActive(false) {
    std::cout << "Reactor has been created." << std::endl;
}

Reactor::~Reactor() {
    if (isActive) {
        stopReactor(this);
    }
}

void* Reactor::startReactor() {
    Reactor* reactor = new Reactor();
    reactor->isActive = true;
    std::cout << "Reactor has been started." << std::endl;
    return reactor;
}

int Reactor::addFdToReactor(void* reactorPtr, int fd, reactorFunc func) {
    Reactor* reactor = static_cast<Reactor*>(reactorPtr);
    if (!reactor) {
        std::cerr << "Error: Reactor pointer is null." << std::endl;
        return -1;
    }

    // Check if the file descriptor is already present
    for (const auto& pfd : reactor->pollfds) {      // iterate over pollfds
        if (pfd.fd == fd) {
            std::cerr << "Error: File descriptor already exists." << std::endl;
            return -1;
        }
    }

    // Add the new file descriptor to the pollfds
    pollfd pfd = {fd, POLLIN, 0};
    reactor->pollfds.push_back(pfd);
    reactor->handlers[fd] = func;

    return 0;
}

int Reactor::removeFdFromReactor(void* reactorPtr, int fd) {
    Reactor* reactor = static_cast<Reactor*>(reactorPtr);
    if (!reactor) {
        std::cerr << "Error: Reactor pointer is null." << std::endl;
        return -1;
    }

    // Check if the file descriptor exists
    auto it = reactor->handlers.find(fd);       // if exists in handlers, must exist in pollfds
    if (it == reactor->handlers.end()) {
        std::cerr << "Error: File descriptor not found." << std::endl;
        return -1;
    }

    // Remove file descriptor and its handler
    reactor->handlers.erase(it);        // 'it' was found above
    // remove from pollfds
    for (auto it = reactor->pollfds.begin(); it != reactor->pollfds.end(); ) {
        if (it->fd == fd) {
            it = reactor->pollfds.erase(it);
            break;      // assuming fd is unique
        } else {
            ++it;
        }
    }

    return 0;
}

int Reactor::stopReactor(void* reactorPtr) {
    Reactor* reactor = static_cast<Reactor*>(reactorPtr);
    if (!reactor) {
        std::cerr << "Error: Reactor pointer is null." << std::endl;
        return -1;
    }

    reactor->isActive = false;
    delete reactor;
    std::cout << "Reactor has been stopped." << std::endl;
    return 0;
}

void Reactor::run() {
    if (!isActive) {
        std::cerr << "Error: Reactor is not active." << std::endl;
        return;
    }

    while (isActive) {
        int pollResult = poll(pollfds.data(), pollfds.size(), -1);
        if (pollResult < 0) {
            std::cerr << "Poll failed: " << strerror(errno) << std::endl;
            continue;
        }
        // Monitor all file descriptors for input
        for (const auto& pfd : pollfds) {
            // POLLIN := There is data to read. look through pollfds to find a file descriptor with data to read
            if (pfd.revents & POLLIN) {
                auto handlerIt = handlers.find(pfd.fd);     // finds its handler
                if (handlerIt != handlers.end()) {
                    // run the handler
                    handlerIt->second(pfd.fd);      // take the reactorFunc out of the pair and run it
                }
            }
        }
    }
}
