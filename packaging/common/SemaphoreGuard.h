#ifndef _SEMAPHOREGUARD_H
#define _SEMAPHOREGUARD_H

/**
 * @file SemaphoreGuard.h
 * @brief Defines the SemaphoreGuard class.
 */

#include "common/Semaphore.h"

/**
 * @brief SemaphoreGuard class is a RAII class for Semaphores.
 * 
 * @author Alexandre Maia Godoi <alexandre.maia@siemens.com>
 */
class SemaphoreGuard
{
public:
    SemaphoreGuard(Semaphore &s, bool initialLock=true, int timeout=-1);
    ~SemaphoreGuard();

    void wait(int timeout=-1);
    void post();
    bool isLocked();

private:
    Semaphore &mSemaphore;   ///< Semaphore object.
    bool mIsLocked;          ///< Locked semaphore flag.
};

#endif // _SemaphoreGUARD_H

