#include "common/TimerThread.h"
#include "common/MutexGuard.h"

/**
 * @brief TimerThread constructor.
 *
 * This constructor creates the timer thread.
 */
TimerThread::TimerThread(CallbackTimerThread *callback)
{
    mTID             = 0;
    mCallback        = callback;
    mIsThreadRunning = true;
    mIsTimerRunning  = false;
    mTime            = 0;
    
    mMutexTimer.lock();        // lock mutex to block timer
       
    pthread_create(&mTID, NULL, TimerThread::callbackThread, (void *)this);
}

/**
 * @brief TimerThread destructor.
 *
 * Waits for timer thread termination.
 */
TimerThread::~TimerThread()
{
    mIsTimerRunning = false;     // stop timer

    mMutexTimer.unlock();        // unlock mutex to unblock timer

    mIsThreadRunning = false;    // stop thread
    
    mSemaphoreThread.post();     // release thread to run
            
    pthread_join(mTID, NULL);    // make sure the callbackThread() has finished
}

/**
 * @brief Starts to count time.
 *
 * @param time  Time to be elapsed.
 */
void TimerThread::startTimer(int time)
{
    mTime            = time;
    mIsTimerRunning  = true;
    mSemaphoreThread.post();  // release thread to run
    mMutexTimer.trylock();    // try to lock mutex to block timer
}

/**
 * @brief Stops to count time.
 */
void TimerThread::stopTimer()
{
    mIsTimerRunning  = false;
    mMutexTimer.unlock();
}

/**
 * @brief Signals to stop running the callback and waits for it to finish.
 */
void TimerThread::synchronousStopTimer()
{
	// avoid a deadlock (user could make a call from the callback)
	if(pthread_self() == mTID)
		return;
	
	MutexGuard callbackGuardian(mMutexCallback);
	
    mIsTimerRunning  = false;
    mMutexTimer.unlock();
}


/**
 * @brief Timer thread.
 *
 * @param arg  TimerThread object.
 *
 * Calls TimerThread::run() to run timer thread as a member method.
 */
void *TimerThread::callbackThread(void *arg)
{
    TimerThread *p = (TimerThread *)arg;

    // detach thread
    //pthread_detach(pthread_self());

    p->run();

    pthread_exit(NULL);
}

/**
 * @brief Timer thread.
 *
 * Count time and calls CallbackTimerThread::callbackTimerThreadExpired() 
 * when time was expired.
 */
void TimerThread::run()
{
    while (mIsThreadRunning)
    {
        // waiting for the timer signal
        mSemaphoreThread.wait();
        
        if (mIsTimerRunning)
        {
            mMutexTimer.timedlock(mTime);
            MutexGuard callbackGuardian(mMutexCallback);
            if (mIsTimerRunning)
            {
                mCallback->callbackTimerThreadExpired();
            }
        }
    }
}

