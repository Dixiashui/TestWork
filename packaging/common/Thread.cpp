#include "common/Thread.h"

/**
 * @brief Thread constructor.
 */
Thread::Thread()
{
    __mIsRunning  = false;
    __mId = 0;
}

/**
 * @brief Thread destructor.
 *
 * Joins thre thread.
 */
Thread::~Thread()
{
    join();
}

/**
 * @brief Causes this thread to begin execution. Calls the Thread::run() 
 * method.
 *
 * It is never legal to start a thread more than once. In particular, a 
 * thread may not be restarted once it has completed execution.
 */
void Thread::start()
{
    if (!__mIsRunning)
    {
        __mIsRunning = true;
        pthread_create(&__mId, NULL, Thread::runThread, this);
    }
}

/**
 * @brief Waits for this thread to die.
 *
 * This method should be called to avoid resource leak in joinable threads.
 */
void Thread::join()
{
    if (__mIsRunning)
    {
        __mIsRunning = false;
        pthread_join(__mId, NULL);
        __mId = 0;
    }
}

/**
 * @brief Returns the Thread Identification.
 */
unsigned long int Thread::getId()
{
    return __mId;
}

/**
 * @brief This is a static method that returns the current Thread Identification.
 */
unsigned long int Thread::getCurrentId()
{
    return pthread_self();
}

/**
 * @brief This is the static method that runs the thread
 */
void * Thread::runThread(void *myself)
{
    ((Thread*)myself)->runThread();

    pthread_exit(NULL);
}

/**
 * @brief This is a method that calls the Thread::run() method 
 * to start thread execution.
 */
void Thread::runThread()
{
    run();
}

