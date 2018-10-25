#ifndef _CALLBACKTIMERTHREAD_H
#define _CALLBACKTIMERTHREAD_H

/**
 * @file CallbackTimerThread.h
 * @brief Defines the CallbackTimerThread class.
 */

/**
 * @brief This class is used to notify when a timer expires.
 *
 * Any class intended to receive expires notification must derive from 
 * this class and overwrites the CallbackTimerThread::callbackTimerThreadExpired()
 * method.
 * 
 * @author Alexandre Maia Godoi <alexandre.maia@siemens.com>
 */
class CallbackTimerThread
{
public:
    /**
     * @brief CallbackTimerThread constructor.
     */
    CallbackTimerThread() {};

    /**
     * @brief CallbackTimerThread destructor.
     */
    virtual ~CallbackTimerThread() {};
    
    /**
     * @brief This method is called whenever timer expires.
     *
     * The timer must be restarted everytime this method 
     * is called in order to get next timer expiration.
     */
    virtual void callbackTimerThreadExpired() = 0;
    virtual void callbackAssignTimerThreadExpired(int timer_id) { callbackTimerThreadExpired(); }
};

#endif // ifndef _CALLBACKTIMERTHREAD_H 


