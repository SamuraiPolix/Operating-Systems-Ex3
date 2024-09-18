#ifndef PROACTOR_HPP
#define PROACTOR_HPP

#include <pthread.h>
#include <atomic>
#include <unordered_map>

/*
 * Q8: Add a proactor template to the library from question 5.
 * The template will receive a socket on which it listens.
 * Every time a client connects (accept) the proactor will create a new thread with a function that will be accepted as a parameter.
 * Note that the graph is a shared resource and changes to the shared resource must be mutex protected.
 * In addition, there cannot be a situation where we change the graph while another thread is working on it (calculating the algorithm)
*/

/*
 * Q9: Implement step 7 with the help of the library implemented in step 8
*/

/*
 * Q10: Add to the server from question 9 a final thread that waits for a situation where at least 50% of the graph
 * (that is, if the graph has n vertices) At least /2n vertices) are in one binding element.
 * If after a kosaraju calculation (one initiated by a user, there is no need to perform a calculation for every insertion of a single arc) we have reached the situation 
 * As such, the thread that will print to stdout on the server side should be woken up.
 * "At least 50% of the graph belongs to the same SCC"
 * If after we have already reached the situation where most of the graph is in the same binding component we will reach the situation after calculating kosaraju
 * (due to removing arcs (we have reached a situation where 50% of the graph no longer belong to the same component, the algorithm will print
 * to stdout "At least 50% of the graph no longer belongs to the same SCC"
 * The extra thread has to be waken with POSIX cond.
 */

// Define a typedef for the function pointer type
typedef void* (*proactorFunc)(int);

class Proactor {
public:
    Proactor();
    ~Proactor();

    // Starts a new proactor and returns the proactor thread id
    static pthread_t startProactor(int sockfd, proactorFunc threadFunc);

    // Stops the proactor by thread id
    static int stopProactor(pthread_t tid);

    static void lockProactorsMutex();
    static void unlockProactorsMutex();

private:
    static std::unordered_map<pthread_t, Proactor*> proactors;
    static pthread_mutex_t proactorsMutex;

    int sockfd;
    proactorFunc threadFunc;
    std::atomic<bool> running;
    pthread_mutex_t graphMutex;

    static void* proactorThread(void *arg);
    static void* threadFuncWrapper(void* arg);
};

#endif // PROACTOR_HPP
