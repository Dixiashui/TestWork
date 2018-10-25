/* 
 * File:   AnotherMultiTimerThread.h
 * Author: pengchao
 *
 * Created on December 6, 2010, 10:09 PM
 */

#ifndef _ANOTHERMULTITIMERTHREAD_H
#define	_ANOTHERMULTITIMERTHREAD_H

#include "TimerThread.h"
#include "Mutex.h"
#include <map>
using namespace std;

#define INTERVAL 2
class AnotherTimerTask;

class AnotherMultiTimerThread
{
public:
    AnotherMultiTimerThread();
    AnotherMultiTimerThread(int interval);
    ~AnotherMultiTimerThread();
    // return timeId
    int addTimer(AnotherTimerTask *task);
    void restartTimer(int timerId);
    void restartTimer(int timerId, int time);
    void pauseTimer(int timerId);
    void removeTimer(int timerId);
    void startTimer();
    bool isTimerRunning(int timerId);
    void setInterval(int interval)
    {
        this->interval = interval;
    }

    int getInterval()
    {
        return this->interval;
    }
private:
    AnotherTimerTask *findTimer(int timerId);
    void init();
    void startTimer(int time);
    void checkTimer();
    void stopTimer();
    void run();
    void synchronousStopTimer();
private:
    map<int, AnotherTimerTask *> timers;
    int idCount;
    int interval;
    Mutex mapMutex; //for access map

    pthread_t mTID; ///< Thread ID.
    Mutex mMutexTimer; ///< Mutex to count time.
    Mutex mMutexCallback; ///< Mutex to lock access to the callback.
    int mTime; ///< Elapsed time
    bool mIsThreadRunning; ///< Thread running flag.
    bool mIsTimerRunning; ///< Timer running flag.

    static void *callbackThread(void *arg);
};

#endif	/* _ANOTHERMULTITIMERTHREAD_H */

