#include <errno.h>
#include "common/ReadWriteMutex.h"

/**
 * @brief ReadWriteMutex constructor.
 *
 * Initializes the mutex referenced by ReadWriteMutex::mMutex with default attributes.
 * Upon successful initialization, the state of the ReadWriteMutex::mMutex 
 * becomes initialized and unlocked.
 */
ReadWriteMutex::ReadWriteMutex()
{
    pthread_rwlock_init(&mMutex, NULL);
}

/**
 * @brief ReadWriteMutex destructor.
 *
 * Destroys the mutex object referenced by ReadWriteMutex::mMutex. The ReadWriteMutex::mMutex 
 * object becomes, in effect, uninitialized.
 */
ReadWriteMutex::~ReadWriteMutex()
{
    pthread_rwlock_destroy(&mMutex);
}

/**
 * @brief Locks the mutex as reader.
 *
 * Applies a read lock to the read-write lock referenced by 
 * ReadWriteMutex::mMutex. The calling thread acquires the read lock if a 
 * writer does not hold the lock and there are no writers blocked on the lock.
 *
 * @throws ReadWriteMutexException.
 *
 * @returns Error code. See man pthread_rwlock_rdlock.
 */
int ReadWriteMutex::rdlock()
{
    int err =  pthread_rwlock_rdlock(&mMutex);

    if (err == EDEADLK) {
        throw ReadWriteMutexException("Resource deadlock would occur");
    }

    return err;
}

/**
 * @brief Tries to lock the mutex as reader.
 *
 * Applies a read lock as in the ReadWriteMutex::lockrd() function, with the 
 * exception that the function shall fail if the equivalent ReadWriteMutex::lockrd()  
 * call would have blocked the calling thread. In no case shall the 
 * ReadWriteMutex::trylockrd() function ever block; it always either acquires the lock 
 * or fails and returns immediately.
 *
 * @returns Error code. See man pthread_rwlock_tryrdlock.
 */
int ReadWriteMutex::tryrdlock()
{
    return pthread_rwlock_tryrdlock(&mMutex);
}

/**
 * @brief Locks the mutex as reader or terminates if timeout expires.
 *
 * Applies a read lock to the read-write lock referenced by ReadWriteMutex::mMutex 
 * as in the ReadWriteMutex::lockrd() function. However, if the lock cannot be acquired 
 * without waiting for other threads to unlock the lock, this wait shall be terminated 
 * when the specified timeout expires.
 *
 * @param timeout Timeout in milliseconds.
 * @throws ReadWriteMutexException.
 * 
 * @returns Error code. See man pthread_rwlock_timedrdlock.
 */
int ReadWriteMutex::timedrdlock(int timeout)
{
    timespec timeWait;

    Util::absTimeout(timeout, timeWait);

    int err = pthread_rwlock_timedrdlock(&mMutex, &timeWait);
    
    if (err == EDEADLK) {
        throw ReadWriteMutexException("Resource deadlock would occur");
    }

    return err;
}

/**
 * @brief Locks the mutex as writer.
 *
 * Applies a write lock to the read-write lock referenced by ReadWriteMutex::mMutex. 
 * The calling thread acquires the write lock if no other thread (reader or writer) 
 * holds the read-write lock ReadWriteMutex::mMutex. Otherwise, the thread shall block 
 * until it can acquire the lock. The calling thread may deadlock if at the time the call 
 * is made it holds the read-write lock (whether a read or write lock).
 *
 * @throws ReadWriteMutexException.
 *
 * @returns Error code. See man pthread_rwlock_wrlock.
 */
int ReadWriteMutex::wrlock()
{
    int err =  pthread_rwlock_wrlock(&mMutex);
    
    if (err == EDEADLK) {
        throw ReadWriteMutexException("Resource deadlock would occur");
    }

    return err;
}

/**
 * @brief Tries to lock the mutex as writer.
 *
 * Applies a write lock like the ReadWriteMutex::wrlock() function, with the exception that 
 * the function shall fail if any thread currently holds rwlock (for reading or writing).
 *
 * @returns Error code. See man pthread_rwlock_trywrlock.
 */
int ReadWriteMutex::trywrlock()
{
    return pthread_rwlock_trywrlock(&mMutex);
}

/**
 * @brief Locks the mutex as writer or terminates if timeout expires.
 *
 * Applies a write lock to the read-write lock referenced by ReadWriteMutex::mMutex as in 
 * the ReadWriteMutex::wrlock() function. However, if the lock cannot be acquired without 
 * waiting for other threads to unlock the lock, this wait shall be terminated when the 
 * specified timeout expires.
 *
 * @param timeout Timeout in milliseconds.
 * @throws ReadWriteMutexException.
 * 
 * @returns Error code. See man pthread_rwlock_timedwrlock.
 */
int ReadWriteMutex::timedwrlock(int timeout)
{
    timespec timeWait;

    Util::absTimeout(timeout, timeWait);

    int err = pthread_rwlock_timedwrlock(&mMutex, &timeWait);
    
    if (err == EDEADLK) {
        throw ReadWriteMutexException("Resource deadlock would occur");
    }

    return err;
}

/**
 * @brief Releases the mutex.
 *
 * Releases the mutex object referenced by ReadWriteMutex::mMutex.
 *
 * @returns Error code. See man pthread_rwlock_unlock.
 */
int ReadWriteMutex::unlock()
{
    return pthread_rwlock_unlock(&mMutex);
}

