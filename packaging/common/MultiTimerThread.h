/* 
 * File:   MultiTimerThread.h
 * Author: pengch
 *
 * Created on July 13, 2010, 4:36 PM
 */

#ifndef _MULTITIMERTHREAD_H
#define	_MULTITIMERTHREAD_H

#include "TimerThread.h"
#include "Mutex.h"
#include <map>
using namespace std;

#define INTERVAL 10
class TimerTask;

class MultiTimerThread
{
public:
    MultiTimerThread();
    MultiTimerThread(int interval);
    ~MultiTimerThread();
    // return timeId
    int addTimer(TimerTask *task);
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
    TimerTask *findTimer(int timerId);
    void init();
    void startTimer(int time);
    void checkTimer();
    void stopTimer();
    void run();
    void synchronousStopTimer();
private:
    map<int, TimerTask *> timers;
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

#endif	/* _MULTITIMERTHREAD_H */

