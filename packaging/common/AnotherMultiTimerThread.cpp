#include "common/MutexGuard.h"
#include "AnotherTimerTask.h"
#include "AnotherMultiTimerThread.h"
#include <iostream>
using namespace std;

AnotherMultiTimerThread::AnotherMultiTimerThread()
{
    this->interval = INTERVAL;
}

AnotherMultiTimerThread::AnotherMultiTimerThread(int interval)
{
    this->interval = interval;
}

AnotherMultiTimerThread::~AnotherMultiTimerThread()
{
    mIsTimerRunning = false; // stop timer
    mMutexTimer.unlock(); // unlock mutex to unblock timer
    mIsThreadRunning = false; // stop thread

    map<int, AnotherTimerTask*>::iterator iter;
    for (iter = timers.begin(); iter != timers.end(); iter++)
    {
        delete iter->second;
    }
    pthread_join(mTID, NULL); // make sure the callbackThread() has finished
}

void AnotherMultiTimerThread::init()
{

    mTID = 0;
    mIsThreadRunning = true;
    mIsTimerRunning = false;

    mMutexTimer.lock(); // lock mutex to block timer

    pthread_create(&mTID, NULL, AnotherMultiTimerThread::callbackThread, (void *) this);

    this->idCount = 0;
}

void AnotherMultiTimerThread::startTimer()
{
    init();
    this->startTimer(interval);
}

int AnotherMultiTimerThread::addTimer(AnotherTimerTask *task)
{
    mapMutex.lock();
    this->idCount++;
    task->setInterval(task->getInterval());
    timers.insert(map<int, AnotherTimerTask*>::value_type(this->idCount, task));
    mapMutex.unlock();
    return this->idCount;
}

void AnotherMultiTimerThread::removeTimer(int timerId)
{
    mapMutex.lock();
    map<int, AnotherTimerTask*>::iterator it = timers.find(timerId);
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

void AnotherMultiTimerThread::restartTimer(int timerId)
{
    AnotherTimerTask *tt;
    mapMutex.lock();
    if ((tt = findTimer(timerId)) != NULL)
    {
        tt->restart();
    }
    mapMutex.unlock();
}

void AnotherMultiTimerThread::restartTimer(int timerId, int time)
{
    AnotherTimerTask *tt;
    mapMutex.lock();
    if ((tt = findTimer(timerId)) != NULL)
    {
        tt->restart(time);
    }
    mapMutex.unlock();
}

bool AnotherMultiTimerThread::isTimerRunning(int timerId)
{
    AnotherTimerTask *tt;
    bool rtn = false;
    
    mapMutex.lock();
    if ((tt = findTimer(timerId)) != NULL)
    {
        rtn = !tt->isPause();
    }
    mapMutex.unlock();
    
    return rtn;
}

void AnotherMultiTimerThread::pauseTimer(int timerId)
{
    AnotherTimerTask *tt;
    mapMutex.lock();
    if ((tt = findTimer(timerId)) != NULL)
    {
        tt->pause();
    }
    mapMutex.unlock();
}

void AnotherMultiTimerThread::checkTimer()
{
    map<int, AnotherTimerTask*>::iterator iter;
    for (iter = timers.begin(); iter != timers.end(); iter++)
    {
        if (!iter->second->isPause())
        {
            iter->second->onTick(iter->first);
        }
    }
}

AnotherTimerTask* AnotherMultiTimerThread::findTimer(int timerId)
{
    map<int, AnotherTimerTask*>::iterator it = timers.find(timerId);
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
void AnotherMultiTimerThread::startTimer(int time)
{
    mTime = time;
    mIsTimerRunning = true;
    mMutexTimer.trylock(); // try to lock mutex to block timer
}

/**
 * @brief Stops to count time.
 */
void AnotherMultiTimerThread::stopTimer()
{
    mIsTimerRunning = false;
    mMutexTimer.unlock();
}

/**
 * @brief Signals to stop running the callback and waits for it to finish.
 */
void AnotherMultiTimerThread::synchronousStopTimer()
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
void *AnotherMultiTimerThread::callbackThread(void *arg)
{
    AnotherMultiTimerThread *p = (AnotherMultiTimerThread *) arg;

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
void AnotherMultiTimerThread::run()
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