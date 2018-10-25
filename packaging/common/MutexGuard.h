#ifndef _MUTEXGUARD_H
#define _MUTEXGUARD_H

/**
 * @file MutexGuard.h
 * @brief Defines the MutexGuard class.
 */

#include "common/MutexItf.h"

/**
 * @brief MutexGuard class is a RAII class for Mutexes.
 * 
 * @author Daniel Cavalcanti Jeronymo <daniel.cavalcanti.ext@siemens.com>
 */
class MutexGuard
{
public:
    MutexGuard(MutexItf &m, bool initialLock=true, int timeout=-1);
    ~MutexGuard();

    void lock(int timeout=-1);
    void unlock();
    bool isLocked();

private:
    MutexItf &mMutex;   ///< Mutex object.
    bool mIsLocked;  ///< Locked mutex flag.
};

#endif // _MUTEXGUARD_H

