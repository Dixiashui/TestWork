#ifndef _SEMAPHORE__H  // must be different of _SEMAPHORE_H (#include <semaphore.h>)
#define _SEMAPHORE__H

/**
 * @file Semaphore.h
 * @brief Defines the Semaphore class.
 */

#include <pthread.h>
#include <semaphore.h>
#include "common/Util.h"

/**
 * @brief Semaphore class is a wrapper for Linux semaphore object.
 *
 * @author Alexandre Maia Godoi <alexandre.maia@siemens.com>
 */
class Semaphore
{
public:
    Semaphore(unsigned int value=0);
    ~Semaphore();

    int wait();
    int trywait();
    int post();

    int timedwait(int timeout);

private:
    sem_t mSema;  ///< Semaphore object.
};

#endif // ifndef _SEMAPHORE__H 

