#ifndef __COMMON_SAFE_QUEUE__
#define __COMMON_SAFE_QUEUE__

#include <list>
#include <iostream>
#include "ActiveObjectSafeAttribute.h"

/**
 * @brief A protected queue.
 *
 *  A SafeQueue can be used in a multithread environment to share
 *  data between two points, in a provider/consumer paradigm
 *
 * @param _AttributeType The type of objects stored in the queue.
 */
template <typename _AttributeType>
class SafeQueue {
public:
	typedef ActiveObjectSafeAttribute< std::list<_AttributeType> >
		    DataQueueType;

private:	
	DataQueueType dataQueue;///< The actual queue
	Mutex emptyLock; ///< The lock used when the queue is empty
	int isEmptyLocked;
public:
	/**
	 * @brief The constructor 
	 */
	SafeQueue() {
		// this will make sure that a attempt to lock the flag will cause
		// the thread to wait in the empty flag mutex
		this->lockEmptyFlag();
	}

	/**
	 * @brief Queue an object.
	 *
	 * Locks the object's actual queue and insert the given object. 
	 *
	 * @param mb The object to queue.
	 */
	void queue(_AttributeType mb) {
		this->lockQueue();
		this->dataQueue->push_back(mb);

		// indicates that the queue is not empty anymore
		this->unlockEmptyFlag();

		this->unlockQueue();
	}

	/**
	 * @brief Queue a set of objects.
	 *
	 * Locks the object's actual queue and insert the given objects. 
	 *
	 * @param dataToQueue the list of objects to queue.
	 */

	void queue(std::list<_AttributeType> dataToQueue) {
		this->lockQueue();
		typename std::list<_AttributeType>::iterator it;
		for (it=dataToQueue.begin(); it != dataToQueue.end(); it++) {
			this->dataQueue->push_back(*it);
		}
		
		// indicates that the queue is not empty anymore
        this->unlockEmptyFlag();

		this->unlockQueue();
	}

	/**
	 *  @brief Dequeues an object.
	 *
	 * Retrieves the first element in the queue, if possible. The queue's
	 * mutex will be locked to prevent race conditions.
	 *
	 * The element is also removed from the queue. If the queue's amount of
	 * elements gets to 0, the emptyLock will be locked, making it possible
	 * to wait until there's more data available.
	 *
	 * Notice that for this to work properly, _AttributeType should be able
	 * to initialize itself with an undefined value, so the return can be
	 * checked for such value.
	 *
	 * @returns The first element in the queue, if possible. An "undefined"
	 * element if the queue is empty.
	 */
	_AttributeType dequeue()
	{
		_AttributeType mb;

		// lock the message queue to prevent it from changing
		this->lockQueue();

		if (this->dataQueue->size() == 0) {
			// nothing here, unlock and return NULL
			//std::cout << "release lock for queue " << (int)this << std::endl;
			this->unlockQueue();
			return mb;
		}

		// pop the first message 
		mb = this->dataQueue->front();
		this->dataQueue->pop_front();

		if (this->dataQueue->size() == 0) {
			if (!this->isEmptyLocked)
				this->lockEmptyFlag();
		}

		// unlock the queue
		this->unlockQueue();

		return mb;
	}

	DataQueueType dequeueAll() {
		_AttributeType mb;
		DataQueueType resQueue;
		
		// lock the message queue to prevent it from changing
		this->lockQueue();
		if (this->dataQueue->size() == 0) {
			this->unlockQueue();
			return resQueue;
		}

		while (this->dataQueue->size() > 0) {
			// pop the first message 
			mb = this->dataQueue->front();
			this->dataQueue->pop_front();
			resQueue->push_back(mb);
		}

		// makes sure the next attemp
		if (!this->isEmptyLocked)
			this->lockEmptyFlag();

		// unlock the queue
		this->unlockQueue();

		return resQueue;
	}

	/**
	 *  @brief Forces the queue to unlock.
	 *
	 * Method used if one want's to force the waitForData method's return
	 * even when there's no data. 
	 *
	 * @see waitForData
	 */
	inline void ForceUnlock() {
		this->unlockEmptyFlag();
	}

	/**
	 * @brief Waits until there's data available in the queue.
	 * 
	 * Returns only when there's available data in the queue.
	 *
	 * @returns Always 0.
	 */
	int waitForData() {
		// try to lock the empty lock to force the calling thread to wait if
		// the queue is empty

		this->lockQueue();
		if (this->dataQueue->size() == 0) {
			this->unlockQueue();
			this->lockEmptyFlag();
			// release the lock. the queue is still not empty, after all
			this->unlockEmptyFlag();
		}
		else
			this->unlockQueue();
		return 0;
	}

	/**
	 * @brief Waits until there's data available in the queue or if time
	 * expired.
	 * 
	 * Returns only when there's available data in the queue.
	 *
	 * @returns 0 if OK, -1 if time expired.
	 */
	int waitForData(int timeout) {
		// try to lock the empty lock to force the calling thread to wait if
		// the queue is empty
		this->lockQueue();
		int res = 0;
		if (this->dataQueue->size() == 0) {
			this->unlockQueue();
			if (this->lockEmptyFlag(timeout) == 0) {
				// release the lock. the queue is still not empty, after all
				this->unlockEmptyFlag();
			}
			else {
				res = -1;
			}
		}
		else
			this->unlockQueue();
		return 0;
	}

	inline void lockQueue() {
//		std::cout << (int)this << " - acquire queue lock" << std::endl;
		this->dataQueue.Lock();
//		std::cout << (int)this << " - queue lock acquired" << std::endl;
	}

	inline void unlockQueue() {
//		std::cout << (int)this << " - release queue lock" << std::endl;
		this->dataQueue.Unlock();
	}

	inline int lockEmptyFlag(int timeout=-1) {
//		std::cout << (int)this << " - acquire empty lock" << std::endl;
		if (timeout == -1)
			this->emptyLock.lock();
		else {
			if (this->emptyLock.timedlock(timeout) != 0)
				return -1;
		}
//		std::cout << (int)this << " - empty lock acquired" << std::endl;
			
		this->isEmptyLocked = true;
		return 0;
	}


	inline void unlockEmptyFlag() {
//		std::cout << (int)this << " - release empty lock" << std::endl;
		this->isEmptyLocked = false;
		this->emptyLock.unlock();
	}

	inline int size() {
		return this->dataQueue->size();
	}
};

#endif
