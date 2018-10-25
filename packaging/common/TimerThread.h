#ifndef _TIMERTHREAD_H
#define _TIMERTHREAD_H

/**
 * @file TimerThread.h
 * @brief Defines the TimerThread class.
 */

// error codes
#define TIMER_THREAD_OK                          0
#define TIMER_THREAD_TIMER_ALREADY_STARTED       1
#define TIMER_THREAD_CANNOT_START_THREAD         2 
#define TIMER_THREAD_TIMER_ALREADY_STOPPED       3

// error codes as strings

#include <pthread.h>
#include <time.h>
#include <signal.h>
#include "common/CallbackTimerThread.h"
#include "common/Mutex.h"
#include "common/Semaphore.h"

/**
 * @brief This class implemets a simple timer using a thread.
 *
 * It starts a thread that will calls CallbackTimerThread::callbackTimerThreadExpired() 
 * whenever timer expires. After that timer shall be restarted to put timer thread 
 * in execution again.
 * 
 * @author Alexandre Maia Godoi <alexandre.maia@siemens.com>
 */
class TimerThread
{
public:
    TimerThread(CallbackTimerThread *callback);
    ~TimerThread();

    void startTimer(int time);
    void stopTimer();
    void synchronousStopTimer();
    bool isTimerRunning() {return this->mIsTimerRunning;}
private:
    pthread_t mTID;                  ///< Thread ID.
    Semaphore mSemaphoreThread;      ///< Semaphore to control thread execution.
    Mutex mMutexTimer;               ///< Mutex to count time.
    Mutex mMutexCallback;            ///< Mutex to lock access to the callback.
    CallbackTimerThread *mCallback;  ///< Pointer to CallbackTimerThread object.
    int mTime;                       ///< Elapsed time
    bool mIsThreadRunning;           ///< Thread running flag.
    bool mIsTimerRunning;            ///< Timer running flag.
    
    static void *callbackThread(void *arg);
    void run();
};

#endif // ifndef _LmSA_H

