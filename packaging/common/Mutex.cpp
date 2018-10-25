#include "common/Mutex.h"

/**
 * @brief Mutex constructor.
 *
 * Initializes the mutex referenced by Mutex::mMutex with default attributes.
 * Upon successful initialization, the state of the Mutex::mMutex 
 * becomes initialized and unlocked.
 */
Mutex::Mutex()
{
    pthread_mutex_init(&mMutex, NULL);
}

/**
 * @brief Mutex destructor.
 *
 * Destroys the mutex object referenced by Mutex::mMutex. The Mutex::mMutex 
 * object becomes, in effect, uninitialized.
 */
Mutex::~Mutex()
{
    pthread_mutex_destroy(&mMutex);
}

/**
 * @brief Locks the mutex.
 *
 * If the mutex is already locked, the calling thread shall block until 
 * the mutex becomes available.  This operation shall return with the 
 * mutex object referenced by Mutex::mMutex in the locked state with the calling 
 * thread as its owner.
 *
 * @returns Error code. See man pthread_mutex_lock.
 */
int Mutex::lock()
{
    return pthread_mutex_lock(&mMutex);
}

/**
 * @brief Tries to lock the mutex.
 *
 * This method is equivalent to Mutex::lock(), except that if the mutex 
 * object referenced by Mutex::mMutex is currently locked the call shall 
 * return immediately.
 *
 * @returns Error code. See man pthread_mutex_trylock.
 */
int Mutex::trylock()
{
    return pthread_mutex_trylock(&mMutex);
}

/**
 * @brief Releases the mutex.
 *
 * Release the mutex object referenced by Mutex::mMutex.
 *
 * @returns Error code. See man pthread_mutex_unlock.
 */
int Mutex::unlock()
{
    return pthread_mutex_unlock(&mMutex);
}
   
/**
 * @brief Locks the mutex or terminates if timeout expires.
 *
 * If the mutex is already locked, the calling thread shall block until 
 * the mutex becomes available as in the Mutex::lock() method. If the mutex 
 * cannot be locked without waiting for another thread to unlock the mutex, 
 * this wait shall be terminated when the specified timeout expires.
 *
 * @param timeout Timeout in milliseconds.
 * 
 * @returns Error code. See man pthread_mutex_timedlock.
 */
int Mutex::timedlock(int timeout)
{
    timespec timeWait;

    Util::absTimeout(timeout, timeWait);

    return pthread_mutex_timedlock(&mMutex, &timeWait);
}

