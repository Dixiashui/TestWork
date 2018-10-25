/* 
 * File:   TimerTask.h
 * Author: pengch
 *
 * Created on July 13, 2010, 5:20 PM
 */

#ifndef _TIMERTASK_H
#define	_TIMERTASK_H

#include "CallbackTimerThread.h"
#include <iostream>

class TimerTask
{
public:

    TimerTask(int interval, CallbackTimerThread *callback)
    {
        this->interval = interval;
        this->callback = callback;
        this->paused = true;
    }

    ~TimerTask()
    {
        if (this->callback != NULL) delete this->callback;
    }

    int getInterval()
    {
        return interval;
    }

    void setInterval(int inter)
    {
        interval = inter;
    }
    
    void restart()
    {
        remains = interval;
        paused = false;
    }

    void restart(int time)
    {
        remains = interval = time;
        paused = false;
    }

    void pause()
    {
        paused = true;
    }

    bool isPause()
    {
        return paused;
    }

    void onTick()
    {
        remains--;
        if (remains == 0)
        {
            paused = true;
            //callback();
            callback->callbackTimerThreadExpired();
        }
    }
private:
    int interval;
    int remains;
    bool paused;
    //void (*callback)(void);
    CallbackTimerThread *callback;
};

#endif	/* _TIMERTASK_H */

