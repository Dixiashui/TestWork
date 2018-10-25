#include "common/ReadWriteMutexGuard.h"

/**
 * @brief ReadWriteMutexGuard constructor.
 *
 * Locks the Mutex referenced by the first argument if it is intended. The
 * lock type is defined by the second argument.
 *
 * @param m            Mutex to be locked.
 * @param lockType     Can be ReadWriteMutexGuard::READER or ReadWriteMutexGuard::WRITER.
 * @param initialLock  true if the Mutex is to be locked, otherwise false. 
 *                     The default value is true.
 * @param timeout      Timeout to lock the Mutex. Any value less than zero will be 
 *                     considered as infinnite timeout. The default value is -1 (infinite).
 */
ReadWriteMutexGuard::ReadWriteMutexGuard(ReadWriteMutex &m, LockType lockType, bool initialLock, int timeout)
: mMutex(m), mIsLocked(initialLock), mLockType(lockType) {
    if (initialLock) {
        lock(timeout);
    }
}

/**
 * @brief Locks the Mutex referenced by ReadWriteMutexGuard::mMutex.
 *
 * It uses ReadWriteMutexGuard::mLockType member variable to define the lock type.
 *
 * @param Timeout to lock the Mutex. Any value less than zero will be
 *        considered as infinnite timeout. The default value is -1 (infinite).
 */
void ReadWriteMutexGuard::lock(int timeout) {
    if (timeout >= 0) {
        if (mLockType == READER) {
            if (mMutex.timedrdlock(timeout) == 0) {
                mIsLocked = true;
            }
        } else {
            if (mMutex.timedwrlock(timeout) == 0) {
                mIsLocked = true;
            }
        }
    } else {
        if (mLockType == READER) {
            if (mMutex.rdlock() == 0) {
                mIsLocked = true;
            }
        } else {
            if (mMutex.wrlock() == 0) {
                mIsLocked = true;
            }
        }
    }
}

/**
 * @brief Unlocks the Mutex referenced by ReadWriteMutexGuard::mMutex.
 */
void ReadWriteMutexGuard::unlock() {
    if (mIsLocked) {
        if (mMutex.unlock() == 0) {
            mIsLocked = false;
        }
    }
}

/**
 * @brief Returns the state of the ReadWriteMutexGuard::mMutex.
 *
 * @returns true if ReadWriteMutexGuard::mMutex is locked, otherwise false.
 */
bool ReadWriteMutexGuard::isLocked() {
    return mIsLocked;
}

/**
 * @brief ReadWriteMutexGuard destructor.
 *
 * Unlocks the Mutex referenced by ReadWriteMutexGuard::mMutex as needed.
 */
ReadWriteMutexGuard::~ReadWriteMutexGuard() {
    unlock();
}

