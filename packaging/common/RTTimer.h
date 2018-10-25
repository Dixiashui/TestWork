#ifndef RTTIMER_H_
#define RTTIMER_H_

#include <pthread.h>
#include <time.h>
#include <sys/time.h>
#include <unistd.h>

// how much is 1 sec in n/u/m sec
#define N_1S_in_nS 1000000000
#define N_1S_in_uS 1000000
#define N_1S_in_mS 1000

/**
 * @brief Creates a timer w/ period increased relative to an absolute time.
 * 
 * @author Franco Sinhori <franco.sinhori.ext@siemens.com>
 * @author Ricardo Handa <ricardo.handa@nsn.com> 
 */
class RTTimer
{
public:
	RTTimer();
	~RTTimer();
	void reset();
	void tick(long int period);
	
private:
    pthread_cond_t rateCondition;///condition to control the time
    pthread_mutex_t rateMutex;   ///condition's mutex	
    
	struct timespec timeToWait;  ///specify next absolute time to wake up

	bool isInitialized;
};

#endif /*RTTIMER_H_*/
