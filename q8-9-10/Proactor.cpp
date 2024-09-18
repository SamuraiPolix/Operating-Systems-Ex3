#include "Proactor.hpp"
#include <iostream>
#include <unistd.h>
#include <cstring>
#include <arpa/inet.h>
#include <cerrno>

// Initialize the static members
std::unordered_map<pthread_t, Proactor *> Proactor::proactors;
pthread_mutex_t Proactor::proactorsMutex = PTHREAD_MUTEX_INITIALIZER;

Proactor::Proactor() : sockfd(-1), threadFunc(nullptr), running(false) {
    pthread_mutex_init(&graphMutex, nullptr);
}

Proactor::~Proactor() {
    running.store(false);
    pthread_mutex_destroy(&graphMutex);
    if (sockfd != -1) {
        close(sockfd);
    }
}

pthread_t Proactor::startProactor(int sockfd, proactorFunc threadFunc) {
    pthread_t tid;
    Proactor *newProactor = new Proactor();
    newProactor->sockfd = sockfd;
    newProactor->threadFunc = threadFunc;
    newProactor->running.store(true);

    // Create the proactor thread
    if (pthread_create(&tid, nullptr, Proactor::proactorThread, static_cast<void *>(newProactor)) != 0) {
        std::cerr << "Error: Failed to create proactor thread. " << strerror(errno) << std::endl;
        delete newProactor;
        return pthread_t(); // Return an invalid thread id
    }

    // Store the proactor instance
    pthread_mutex_lock(&proactorsMutex);        // safety first!
    proactors[tid] = newProactor;
    pthread_mutex_unlock(&proactorsMutex);

    return tid;
}

int Proactor::stopProactor(pthread_t tid) {
    pthread_mutex_lock(&proactorsMutex);
    auto it = proactors.find(tid);
    if (it != proactors.end()) {
        Proactor *proactor = it->second;
        proactor->running.store(false);

        // Wait for the thread to finish
        if (pthread_join(tid, nullptr) != 0) {
            std::cerr << "Error: Failed to join proactor thread. " << strerror(errno) << std::endl;
            pthread_mutex_unlock(&proactorsMutex);
            return -1;
        }

        delete proactor;
        proactors.erase(it);
    } else {
        pthread_mutex_unlock(&proactorsMutex);
        std::cerr << "Error: Proactor thread not found." << std::endl;
        return -1;
    }
    pthread_mutex_unlock(&proactorsMutex);

    return 0;
}

void *Proactor::proactorThread(void *arg) {
    Proactor *proactor = static_cast<Proactor *>(arg);

    while (proactor->running.load()) {
        struct sockaddr_storage clientAddr;
        socklen_t addrLen = sizeof(clientAddr);

        // Accept a new connection
        int clientSockfd = accept(proactor->sockfd, (struct sockaddr *)&clientAddr, &addrLen);
        if (clientSockfd == -1) {
            if (proactor->running.load()) {
                std::cerr << "Error: Failed to accept connection: " << strerror(errno) << std::endl;
            }
            continue;
        }

        std::cout << "New connection from " << inet_ntoa(((struct sockaddr_in *)&clientAddr)->sin_addr) << " socket " << clientSockfd << std::endl;

        // Launch the provided function in a new thread
        pthread_t clientThread;
        auto clientSockfdPtr = new int(clientSockfd); // allocate memory for the clientSockfd
        // pass socket and proactor to the thread
        void* args[2] = {clientSockfdPtr, proactor};

        if (pthread_create(&clientThread, nullptr, Proactor::threadFuncWrapper, args) != 0) {
            std::cerr << "Error: Failed to create client thread. " << strerror(errno) << std::endl;
            close(clientSockfd);
            delete clientSockfdPtr; // Clean up the allocated memory
            continue;
        }

        // Detach the client thread so it can clean up itself
        pthread_detach(clientThread);
    }

    return nullptr;
}

void* Proactor::threadFuncWrapper(void* arg) {
    void** args = static_cast<void**>(arg);
    int clientSockfd = *static_cast<int*>(args[0]);
    Proactor* proactor = static_cast<Proactor*>(args[1]);
    std::cout << "Created new client thread for socket " << clientSockfd << std::endl;
    delete static_cast<int*>(args[0]); // Clean up the allocated memory

    // Call the actual function
    // pthread_mutex_lock(&proactorsMutex);
    void* result = proactor->threadFunc(clientSockfd);
    // pthread_mutex_unlock(&proactorsMutex);

    close(clientSockfd);
    return result;
}

void Proactor::lockProactorsMutex() {
    pthread_mutex_lock(&proactorsMutex);
}

void Proactor::unlockProactorsMutex() {
    pthread_mutex_unlock(&proactorsMutex);
}