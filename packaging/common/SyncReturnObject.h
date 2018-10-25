#ifndef SYNC_RESULT__H__
#define SYNC_RESULT__H__

#include "common/Mutex.h"
#include "TwException.h"

/**
 * @brief Implements a return object that can be used to synchronize the
 * call of a method whose response is asynchronous.
 *
 * The SyncReturnObject implements an object used to synchronize an
 * asynch method and its caller. A method can return an object that will be
 * set when the result is actually available. Meanwhile, if the caller needs
 * the function's return, it can check the returned object and stay locked 
 * until the object set its value.
 *
 * @param _ReturnType The type of the value the object will represent.
 */
template <typename _ReturnType>
class SyncReturnObject
{
private:
	bool isLocked;
public:
	/**
	 *  @brief The constructor.
  	 *
	 *  Just initializes the mutex used by the synchronization.
	 */
	SyncReturnObject()
	{
		isLocked = true;
		// get the lock to the object result. 
		this->acrMutex.lock();
	}

	/**
	 *  @brief The destructor.
	 */
	~SyncReturnObject() {
	}

	void ResetLock() {
		if (!isLocked)
			this->acrMutex.lock();
	}

	/**
	 *  @brief Set the return value.
	 *
	 *  Used by the object that returned the SyncReturnObject to set the
	 *  return value when appropriate.
	 *
	 * @param value The return value.
	 */
	void Set(_ReturnType value)
	{
		// set the result value...
		this->return_result = value;

		// and unlock the object.
		this->acrMutex.unlock();
	}

	/**
	 *  @brief Gets the return result or wait for it, when appropriate.
	 *
	 *  The caller will call this method to retrieve the return result. If
	 *  the callee didn't set it, the caller's thread will lock and stay
	 *  until the callee sets the return value.
	 *
	 * @returns The value returned by the callee.
	 */
	_ReturnType Get(int timeout=-1) {
		if (timeout == -1) {
			// wait until the resource is set, and therefore, released
			this->acrMutex.lock();
		}
		else {
			if (this->acrMutex.timedlock(timeout) != 0) {
				throw TimeoutException();
			}
		}
		
		// the lock was released, so the value must be set
		return this->return_result;
	}

private:
	_ReturnType return_result;///< The return value
	Mutex acrMutex;///< The mutex used to sync the caller and the callee.
};

#endif
