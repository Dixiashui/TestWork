#include "common/MutexGuard.h"
#include "TimerTask.h"
#include "MultiTimerThread.h"
#include <iostream>
using namespace std;

MultiTimerThread::MultiTimerThread()
{
    this->interval = INTERVAL;
}

MultiTimerThread::MultiTimerThread(int interval)
{
    this->interval = interval;
}

MultiTimerThread::~MultiTimerThread()
{
    mIsTimerRunning = false; // stop timer
    mMutexTimer.unlock(); // unlock mutex to unblock timer
    mIsThreadRunning = false; // stop thread

    map<int, TimerTask*>::iterator iter;
    for (iter = timers.begin(); iter != timers.end(); iter++)
    {
        delete iter->second;
    }
    pthread_join(mTID, NULL); // make sure the callbackThread() has finished
}

void MultiTimerThread::init()
{

    mTID = 0;
    mIsThreadRunning = true;
    mIsTimerRunning = false;

    mMutexTimer.lock(); // lock mutex to block timer

    pthread_create(&mTID, NULL, MultiTimerThread::callbackThread, (void *) this);

    this->idCount = 0;
}

void MultiTimerThread::startTimer()
{
    init();
    this->startTimer(interval);
}

int MultiTimerThread::addTimer(TimerTask *task)
{
    mapMutex.lock();
    this->idCount++;
    task->setInterval(task->getInterval() / this->interval);
    timers.insert(map<int, TimerTask*>::value_type(this->idCount, task));
    mapMutex.unlock();
    return this->idCount;
}

void MultiTimerThread::removeTimer(int timerId)
{
    mapMutex.lock();
    map<int, TimerTask*>::iterator it = timers.find(timerId);
    if (it == timers.end())
    {
        //error;
    } else
    {
        timers.erase(it);
        delete it->second;
    }
    mapMutex.unlock();
}

void MultiTimerThread::restartTimer(int timerId)
{
    TimerTask *tt;
    mapMutex.lock();
    if ((tt = findTimer(timerId)) != NULL)
    {
        tt->restart();
    }
    mapMutex.unlock();
}

void MultiTimerThread::restartTimer(int timerId, int time)
{
    TimerTask *tt;
    mapMutex.lock();
    if ((tt = findTimer(timerId)) != NULL)
    {
        tt->restart(time/this->interval);
    }
    mapMutex.unlock();
}

bool MultiTimerThread::isTimerRunning(int timerId)
{
    TimerTask *tt;
    bool rtn = false;
    
    mapMutex.lock();
    if ((tt = findTimer(timerId)) != NULL)
    {
        rtn = !tt->isPause();
    }
    mapMutex.unlock();
    
    return rtn;
}

void MultiTimerThread::pauseTimer(int timerId)
{
    TimerTask *tt;
    mapMutex.lock();
    if ((tt = findTimer(timerId)) != NULL)
    {
        tt->pause();
    }
    mapMutex.unlock();
}

void MultiTimerThread::checkTimer()
{
    map<int, TimerTask*>::iterator iter;
    for (iter = timers.begin(); iter != timers.end(); iter++)
    {
        if (!iter->second->isPause())
        {
            iter->second->onTick();
        }
    }
}

TimerTask* MultiTimerThread::findTimer(int timerId)
{
    map<int, TimerTask*>::iterator it = timers.find(timerId);
    if (it == timers.end())
    {
        //error;
        cout << "cannot findTimer " << timerId << endl;
        return NULL;
    }
    return it->second;
}

/**
 * @brief Starts to count time.
 *
 * @param time  Time to be elapsed.
 */
void MultiTimerThread::startTimer(int time)
{
    mTime = time;
    mIsTimerRunning = true;
    mMutexTimer.trylock(); // try to lock mutex to block timer
}

/**
 * @brief Stops to count time.
 */
void MultiTimerThread::stopTimer()
{
    mIsTimerRunning = false;
    mMutexTimer.unlock();
}

/**
 * @brief Signals to stop running the callback and waits for it to finish.
 */
void MultiTimerThread::synchronousStopTimer()
{
    // avoid a deadlock (user could make a call from the callback)
    if (pthread_self() == mTID)
        return;

    MutexGuard callbackGuardian(mMutexCallback);

    mIsTimerRunning = false;
    mMutexTimer.unlock();
}

/**
 * @brief Timer thread.
 *
 * @param arg  TimerThread object.
 *
 * Calls TimerThread::run() to run timer thread as a member method.
 */
void *MultiTimerThread::callbackThread(void *arg)
{
    MultiTimerThread *p = (MultiTimerThread *) arg;

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
void MultiTimerThread::run()
{
    while (mIsThreadRunning)
    {
        if (mIsTimerRunning)
        {
            mMutexTimer.timedlock(mTime);
            MutexGuard callbackGuardian(mMutexCallback);
            if (mIsTimerRunning)
            {
                this->checkTimer();
            }
        }
    }
}