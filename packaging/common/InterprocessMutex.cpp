#include "InterprocessMutex.h"


InterprocessMutex::InterprocessMutex(std::string mutexIdentifier):
	_timeoutClock(this)
{
	this->_mutexIdentifier = mutexIdentifier;
	this->_locked = false;
	this->_lockTimeOut = false;
}

InterprocessMutex::~InterprocessMutex()
{
	if (this->_locked) {
		this->unlock();
	}
}

/*
 * Checks whether there's a shared lock and if the process owner is
 * actually running.
 *
 */
bool InterprocessMutex::checkMemoryLock()
{
	this->_shmId = shm_open(this->_mutexIdentifier.c_str(), O_RDWR| O_CREAT, S_IRWXU | S_IRWXG);
	if (this->_shmId < 0) {
		throw TwException(this->getErrorMessage(errno));
	}
	int * memOwnerPid = (int*)mmap(NULL, sizeof(int), PROT_READ , MAP_SHARED, this->_shmId, 0);
	
	struct stat memStat;
	int retries = 3;
	fstat(this->_shmId, &memStat);
	while (memStat.st_size == 0 && retries) {
		// maybe the actual owner is still trying to acquire the lock
		usleep(LOCK_INTERVAL);
		fstat(this->_shmId, &memStat);
		retries--;
	}
	if (memStat.st_size == 0) {
		// not a valid memory. Probably the process died before finishing
		// the lock
		return false;
	}
	if (memOwnerPid == NULL || memOwnerPid == (int*)-1)
		return false;
	
	// checks whether the process owning the lock is actually running
	std::string processPath = "/proc/";
	processPath+=Util::toString(*memOwnerPid);
	FILE * fp = fopen(processPath.c_str(), "r");
	if (fp == NULL) {
		// the mutex owner died for some reason
		return false;
	}
	fclose(fp);
	return true;
}

/*
 * Writes this process PID to the shared memory.
 */
void InterprocessMutex::writeMemoryLock()
{
	int * memOwnerPid = (int*)mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, this->_shmId, 0);
	if (memOwnerPid == NULL || memOwnerPid == (int*)-1) {
		throw TwException("Failed to write to the shared memory");
	}
		
	*memOwnerPid = getpid();
}

/*
 * Start timeout clock.
 */
void InterprocessMutex::startClock(int timeout)
{
	if (timeout == -1)
		return;

	this->_timeoutClock.startTimer(timeout);
	this->_lockTimeOut = false;
}


/*
 * Translates the errno cod to a text message explaining the error.
 *
 * @param cod errno to translate
 */
std::string InterprocessMutex::getErrorMessage(int cod)
{
	std::string str;
	switch(errno) {
		case EINTR:
			str="  The shm_open() operation was interrupted by a signal.";
			break;

		case EINVAL:
			str=" The shm_open() operation is not supported for the given name.";
			break;

		case EMFILE:
			str=" Too many file descriptors are currently in use by this process.";
			break;

		case ENAMETOOLONG:
			str=" The length of the name argument exceeds {PATH_MAX} or a pathname component is longer than {NAME_MAX}.";
			break;

		case ENFILE:
			str=" Too many shared memory objects are currently open in the system.";
			break;

		case ENOENT:
			str=" O_CREAT is not set and the named shared memory object does not exist.";
			break;

		case ENOSPC:
			str=" There is insufficient space for the creation of the new shared memory object.";
			break;
	}
	return str;
}

int InterprocessMutex::lock()
{
	bool go = true;

	while(go) {
		this->_shmId = shm_open(this->_mutexIdentifier.c_str(), O_RDWR| O_CREAT | O_EXCL, S_IRWXU | S_IRWXG);

		if (this->_shmId < 0) {
			std::string str;
			if (errno == EEXIST) {
				if (!this->checkMemoryLock()) {
					// invalid memory; release it
					shm_unlink(this->_mutexIdentifier.c_str());
				}
				else {
					usleep(LOCK_INTERVAL);
				}
				continue;
			}
			throw TwException(this->getErrorMessage(errno));
		}
		else {
			go = false;
		}
	}
	this->writeMemoryLock();
	this->_locked = true;

	return 0;
}

int InterprocessMutex::trylock()
{
	
	this->_shmId = shm_open(this->_mutexIdentifier.c_str(), O_RDWR| O_CREAT | O_EXCL, S_IRWXU | S_IRWXG);
	if (this->_shmId < 0) {
		std::string str;
		if (errno == EEXIST) {
			if (!this->checkMemoryLock()) {
				// the lock has been acquired, but the owner process is not
				// running; release the lock
				shm_unlink(this->_mutexIdentifier.c_str());
			}
			else {
				return EBUSY;
			}
		}
		else {
			throw TwException(this->getErrorMessage(errno));
		}
	}
	
	// acquires the lock
	this->writeMemoryLock();
	this->_locked = true;
	return 0;
}

int InterprocessMutex::timedlock(int timeout)
{
	bool go = true;
	
	this->startClock(timeout);
	while(go && !this->_lockTimeOut) {
		this->_shmId = shm_open(this->_mutexIdentifier.c_str(), O_RDWR| O_CREAT | O_EXCL, S_IRWXU | S_IRWXG);
		if (this->_shmId < 0) {
			std::string str;
			if (errno == EEXIST) {
				if (!this->checkMemoryLock()) {
					shm_unlink(this->_mutexIdentifier.c_str());
				}
				else {
					usleep(LOCK_INTERVAL);
				}
				continue;
			}
			throw TwException(this->getErrorMessage(errno));
			// acquires the lock
		}
		else {
			go = false;
		}
	}
	if (this->_lockTimeOut) {
		throw TimeoutException();
	}
	this->writeMemoryLock();
	this->_locked = true;
	return true;
}


int InterprocessMutex::unlock()
{
	shm_unlink(this->_mutexIdentifier.c_str());
	return 0;
}

