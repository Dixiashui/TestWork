#include "RTTimer.h"

RTTimer::RTTimer()
{
    pthread_mutex_init(&rateMutex, NULL);
    pthread_cond_init(&rateCondition, NULL);
    isInitialized = false;
}

RTTimer::~RTTimer()
{
    pthread_mutex_destroy(&rateMutex);
    pthread_cond_destroy(&rateCondition);
}

void RTTimer::reset()
{
    struct timeval timeReference;
    gettimeofday(&timeReference, NULL);
    timeToWait.tv_sec = timeReference.tv_sec;
    timeToWait.tv_nsec = timeReference.tv_usec * 1000;
    isInitialized = true;
}

//period type should be the same as specified in <time.h>
void RTTimer::tick(long int period)
{
    if (!isInitialized)
        return;

    timeToWait.tv_nsec += period;
    if (timeToWait.tv_nsec > N_1S_in_nS)
    {
        timeToWait.tv_sec += timeToWait.tv_nsec / N_1S_in_nS;
        timeToWait.tv_nsec = timeToWait.tv_nsec % N_1S_in_nS;
    }

    // wait until the time expires
    pthread_mutex_lock(&rateMutex);
    usleep(1); // force the pthread_cond_timedwait to work
    pthread_cond_timedwait(&rateCondition, &rateMutex, &timeToWait);
    pthread_mutex_unlock(&rateMutex);
}
