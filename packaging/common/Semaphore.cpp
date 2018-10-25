#include "common/Semaphore.h"

/**
 * @brief Semaphore constructor.
 *
 * Initializes the semaphore referred by Semaphore::mSema.
 */
Semaphore::Semaphore(unsigned int value)
{
    sem_init(&mSema, 0, value);
}

/**
 * @brief Semaphore destructor.
 *
 * Destroys the semaphore indicated by Semaphore::mSema.
 */
Semaphore::~Semaphore()
{
    sem_destroy(&mSema);
}

/**
 * @brief Locks the semaphore.
 *
 * Locks the semaphore referenced by Semaphore::mSema by performing 
 * a semaphore lock operation on that semaphore. If the semaphore 
 * value is currently zero, then the calling thread shall not return 
 * from the call until it either locks the semaphore.
 *
 * @returns Error code. See man sem_wait.
 */
int Semaphore::wait()
{
    return sem_wait(&mSema);
}

/**
 * @brief Tries to lock the semaphore.
 *
 * Locks the semaphore referenced by Semaphore::mSema only if the 
 * semaphore is currently not locked.
 *
 * @returns Error code. See man sem_trywait.
 */
int Semaphore::trywait()
{
    return sem_trywait(&mSema);
}

/**
 * @brief Unlocks the semaphore.
 *
 * Unlocks the semaphore referenced by Semaphore::mSema by 
 * performing a semaphore unlock operation on that semaphore.
 *
 * @returns Error code. See man sem_post.
 */
int Semaphore::post()
{
    return sem_post(&mSema);
}

/**
 * @brief Locks the semaphore or terminates if timeout expires.
 *
 * Locks the semaphore referenced by Semaphore::mSema as in the 
 * Semaphore::wait() method. However, if the semaphore cannot be 
 * locked without waiting for another thread to unlock the semaphore 
 * by performing a Semaphore::post(), this wait shall be terminated 
 * when the specified timeout expires.
 *
 * @param timeout  Timeout in milliseconds.
 *
 * @returns Error code. See man sem_timedwait.
 */
int Semaphore::timedwait(int timeout)
{
    timespec timeWait;

    Util::absTimeout(timeout, timeWait);
    
    return sem_timedwait(&mSema, &timeWait);
}

