#include "common/SemaphoreGuard.h"

/**
 * @brief SemaphoreGuard constructor.
 *
 * Locks the Semaphore referenced by the first argument if it is intended.
 *
 * @param s            Semaphore to be locked.
 * @param initialLock  true if the Semaphore is to be locked, otherwise false. 
 *                     The default value is true.
 * @param timeout      Timeout to lock the Semaphore. Any value less than zero will be 
 *                     considered as infinnite timeout. The default value is -1 (infinite).
 */
SemaphoreGuard::SemaphoreGuard(Semaphore &s, bool initialLock, int timeout)
: mSemaphore(s)
{
    mIsLocked = false;

    if (initialLock)
    {
        wait(timeout);
    }
}

/**
 * @brief Locks the Semaphore referenced by SemaphoreGuard::mSemaphore.
 *
 * @param Timeout to lock the Semaphore. Any value less than zero will be
 *        considered as infinnite timeout. The default value is -1 (infinite).
 */
void SemaphoreGuard::wait(int timeout)
{
    if (timeout >= 0)
    {
        if (mSemaphore.timedwait(timeout) == 0)
        {
            mIsLocked = true;
        }
    }
    else
    {
        if (mSemaphore.wait() == 0)
        {
            mIsLocked = true;
        }
    }
}

/**
 * @brief Unlocks the Semaphore referenced by SemaphoreGuard::mSemaphore.
 */
void SemaphoreGuard::post()
{
    if (mIsLocked)
    {
	    if (mSemaphore.post() == 0)
        {
            mIsLocked = false;
        }
    }
}

/**
 * @brief Returns the state of the SemaphoreGuard::mSemaphore.
 *
 * @returns true if SemaphoreGuard::mSemaphore is locked, otherwise false.
 */
bool SemaphoreGuard::isLocked()
{
    return mIsLocked;
}

/**
 * @brief SemaphoreGuard destructor.
 *
 * Unlocks the Semaphore referenced by SemaphoreGuard::mSemaphore as needed.
 */
SemaphoreGuard::~SemaphoreGuard()
{
    post();
}

