#ifndef INTERPROCESSMUTEX_H
#define INTERPROCESSMUTEX_H

#include <stdio.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <string>
#include <signal.h>

#include "TimerThread.h"
#include "TwException.h"
#include "Util.h"
#include "MutexItf.h"

#define LOCK_INTERVAL 5000

/**
 * InterprocessMutex represents an external lock that can be shared
 * by multiple processes. 
 *
 * Using the InterprocessMutex it is possible for two different process 
 * to synchronize and protected shared resources.
 *
 */
class InterprocessMutex : public CallbackTimerThread, public MutexItf {
private:
	std::string _mutexIdentifier;
	bool _locked;
	bool _lockTimeOut;
	TimerThread _timeoutClock;
	int _shmId;

	/*
	 * Translates the errno cod to a text message explaining the error.
	 *
	 * @param cod errno to translate
	 */
	std::string getErrorMessage(int cod);

	/*
	 * Checks whether there's a shared lock and if the process owner is
	 * actually running.
	 *
	 */
	bool checkMemoryLock();

	/*
	 * Writes this process PID to the shared memory.
	 */
	void writeMemoryLock();

	/*
	 * Start timeout clock.
	 */
	void startClock(int timeout);
public:
	InterprocessMutex(std::string mutexIdentifier);
	~InterprocessMutex();

	int lock();
	int trylock();
	int timedlock(int timeout);
	int unlock();

	inline void callbackTimerThreadExpired() {
		this->_lockTimeOut = true;
	}
};

#endif
