#ifndef _MUTEX_H
#define _MUTEX_H

/**
 * @file Mutex.h
 * @brief Defines the Mutex class.
 */

#include <pthread.h>
#include "common/MutexItf.h"
#include "common/Util.h"

/**
 * @brief Mutex class is a wrapper for Linux mutex object.
 * 
 * @author Alexandre Maia Godoi <alexandre.maia@siemens.com>
 */
class Mutex : public MutexItf
{
public:
    Mutex();
    virtual ~Mutex();

    virtual int lock();
    virtual int trylock();
    virtual int unlock();

    virtual int timedlock(int timeout);

private:
    pthread_mutex_t mMutex;  ///< Mutex object.
};

#endif // _MUTEX_H

