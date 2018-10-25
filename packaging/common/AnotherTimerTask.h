/* 
 * File:   AnotherTimerTask.h
 * Author: pengchao
 *
 * Created on December 6, 2010, 10:12 PM
 */

#ifndef _ANOTHERTIMERTASK_H
#define	_ANOTHERTIMERTASK_H

#include "CallbackTimerThread.h"
#include <iostream>
#include <sys/time.h>
#include <stdlib.h>
#include "Util.h"

class AnotherTimerTask
{
public:

    AnotherTimerTask(int interval, CallbackTimerThread *callback)
    {
        this->interval = interval;
        this->callback = callback;
        this->paused = true;
    }

    ~AnotherTimerTask()
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
        gettimeofday(&lastTime, NULL);
        this->paused = false;
    }

    void restart(int time)
    {
        //gettimeofday(&lastTime, NULL);
        restart();
        this->interval = time;
    }

    void pause()
    {
        paused = true;
    }

    bool isPause()
    {
        return paused;
    }

    void onTick(int timer_id)
    {
        struct timeval now;
        struct timeval dst;
        bool debug;
        gettimeofday(&now, NULL);
        
        dst.tv_sec = lastTime.tv_sec;
        long us = lastTime.tv_usec + interval * 1000;
        while (us > 1000000)
        {
            dst.tv_sec += 1;
            us -= 1000000;
        }
        dst.tv_usec = us;
        
        debug = Util::absTimeEqual(now, dst, 500);
        /*unsigned long src = now.tv_sec * 1000000 + now.tv_usec;
        unsigned long dst = lastTime + interval * 1000;
        unsigned long diff = dst - src;*/
        std::cout << "onTick now(s)=" << now.tv_sec << " now(us)=" << now.tv_usec << " dst(s)=" << dst.tv_sec << " dst(us)=" << dst.tv_usec << " debug=" << debug << std::endl;
        //if (diff < 500)
        if (debug)
        {// think as equal, first store lastTime, then invoke the method
            lastTime = now;
            callback->callbackAssignTimerThreadExpired(timer_id);
        }
    }
private:
    int interval;
    bool paused;
    struct timeval lastTime;
    //void (*callback)(void);
    CallbackTimerThread *callback;
};

#endif	/* _ANOTHERTIMERTASK_H */

