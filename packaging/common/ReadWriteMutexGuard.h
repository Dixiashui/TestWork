#ifndef _READWRITEMUTEXGUARD_H
#define _READWRITEMUTEXGUARD_H

/**
 * @file ReadWriteMutexGuard.h
 * @brief Defines the ReadWriteMutexGuard class.
 */

#include "common/ReadWriteMutex.h"

/**
 * @brief MutexGuard class is a RAII class for ReadWriteMutexe.
 * 
 * @author Alexandre Maia Godoi <alexandre.godoi@nsn.com>
 */
class ReadWriteMutexGuard {
public:

    enum LockType {
        READER = 0,
        WRITER = 1
    };

    ReadWriteMutexGuard(ReadWriteMutex &m, LockType lockType, bool initialLock = true, int timeout = -1);
    ~ReadWriteMutexGuard();

    void lock(int timeout = -1);
    void unlock();
    bool isLocked();

private:
    ReadWriteMutex &mMutex; ///< Mutex object.
    bool mIsLocked; ///< Locked mutex flag.
    LockType mLockType; ///< Type of lock operation. Can be ReadWriteMutexGuard::READER or ReadWriteMutexGuard::WRITER.

    ReadWriteMutexGuard(const ReadWriteMutexGuard &r); // copy is not allowed
    ReadWriteMutexGuard & operator=(const ReadWriteMutexGuard &); // assign is not allowed
};

#endif // _READWRITEMUTEXGUARD_H

