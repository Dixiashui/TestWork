#ifndef _THREAD_H
#define _THREAD_H

/**
 * @file Thread.h
 * @brief Defines the Thread class.
 */

#include <pthread.h>
#include "common/Semaphore.h"

/**
 * @brief Thread class is a wrapper for Linux pthreads.
 *
 * OBS: THREAD in detached mode is not working. It was disabled!
 *
 * @author Alexandre Maia Godoi <alexandre.maia@siemens.com>
 */
class Thread
{
public:
    Thread();
    virtual ~Thread();

    void start();
    void join();

    unsigned long int getId();
    static unsigned long int getCurrentId();

protected:
    /**
     * @brief This method is executed in a separate thread.
     *
     * Subclasses of Thread should override this method.
     */
    virtual void run() = 0;

private:
    pthread_t __mId;     ///< Thread identification number.
    bool __mIsRunning;   ///< Running flag.

    void runThread();
    static void * runThread(void *myself);
};

#endif // ifndef _THREAD_H 

