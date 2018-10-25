#include "common/MutexGuard.h"

/**
 * @brief MutexGuard constructor.
 *
 * Locks the Mutex referenced by the first argument if it is intended.
 *
 * @param m            Mutex to be locked.
 * @param initialLock  true if the Mutex is to be locked, otherwise false. 
 *                     The default value is true.
 * @param timeout      Timeout to lock the Mutex. Any value less than zero will be 
 *                     considered as infinnite timeout. The default value is -1 (infinite).
 */
MutexGuard::MutexGuard(MutexItf &m, bool initialLock, int timeout)
: mMutex(m)
{
    mIsLocked = false;

    if (initialLock)
    {
        lock(timeout);
    }
}

/**
 * @brief Locks the Mutex referenced by MutexGuard::mMutex.
 *
 * @param Timeout to lock the Mutex. Any value less than zero will be
 *        considered as infinnite timeout. The default value is -1 (infinite).
 */
void MutexGuard::lock(int timeout)
{
    if (timeout >= 0)
    {
        if (mMutex.timedlock(timeout) == 0)
        {
            mIsLocked = true;
        }
    }
    else
    {
        if (mMutex.lock() == 0)
        {
            mIsLocked = true;
        }
    }
}

/**
 * @brief Unlocks the Mutex referenced by MutexGuard::mMutex.
 */
void MutexGuard::unlock()
{
    if (mIsLocked)
    {
	    if (mMutex.unlock() == 0)
        {
            mIsLocked = false;
        }
    }
}

/**
 * @brief Returns the state of the MutexGuard::mMutex.
 *
 * @returns true if MutexGuard::mMutex is locked, otherwise false.
 */
bool MutexGuard::isLocked()
{
    return mIsLocked;
}

/**
 * @brief MutexGuard destructor.
 *
 * Unlocks the Mutex referenced by MutexGuard::mMutex as needed.
 */
MutexGuard::~MutexGuard()
{
    unlock();
}


