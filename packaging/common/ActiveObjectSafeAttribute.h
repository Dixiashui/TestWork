#include <errno.h>
#include "common/Mutex.h"

/**
 * @brief Implements an attribute that can be safely shared in
 * a multithreaded environment
 *
 * This class implements a variable protected by a mutex, which
 * automatically locks when a thread tries to write on it and provides
 * methods to lock it when appropriate.
 *
 * @param _AttributeType the attribute's type.
 */
template <typename _AttributeType>
class ActiveObjectSafeAttribute
{
public:
	/**
	 * @brief The constructor. 
	 */
	ActiveObjectSafeAttribute() {
		this->readLockCounter = 0;
	}

	/**
	 * @brief The destructor. 
	 */
	~ActiveObjectSafeAttribute() {
	}

	/**
	 *  Unlocks the variable.
	 */
	void Unlock() {
		// release the mutex
		this->attrMutex.unlock();
	}

	/**
	 * @brief Locks the variable for reading.
	 *
	 * The read lock is used to make sure anyone can read when there are
	 * only readers, but no one can write in the variable. Everytime it is
	 * called, the readlock is incremented. 
	 *
	 * When the reader stops reading, it should unlock the variable using
	 * ReadUnlock.
	 *
	 * @returns Whether the lock was sucessful.
	 * @see ReadUnlock
	 */
	bool ReadLock()
	{
		// increment the reader's lock count
		this->readLockCounterMutex.lock();

		if (!this->readLockCounter == 0) {
			// get the write lock
			this->Lock();
		}
		this->readLockCounter++;
		this->readLockCounterMutex.unlock();

		return true;
	}

	/**
	 * @brief Unlocks the variable for reading.
	 *
	 * Decrements the read counter lock. When the lockcounter is zero, the
	 * variable will be free to be written again. This method should only be
	 * called by thread that called ReadUnlock.
	 *
	 * @see ReadLock
	 */

	void ReadUnlock()
	{
		this->readLockCounterMutex.lock();

		if (this->readLockCounter > 0) {
			this->readLockCounter--;

			if (this->readLockCounter == 0) {
				this->Unlock();
			}
		}
		this->readLockCounterMutex.unlock();
	}
	

	/**
	 * @brief Tries to lock the variable for writing.
	 *
	 * @param blocking Whether the caller will stay locked if it can't get a
	 * lock
	 * @returns Whether the lock was sucessful. It can only return false if
	 * 		blocking was set to true and the lock can't be obtained.
	 */
	bool Lock(bool blocking=true)
	{
		// check for blocking 
		if (blocking) {
			// lock the mutex
			this->attrMutex.lock();
		}
		else {
			// try to acquire the mutex
			if (this->attrMutex.trylock() == EBUSY) {
				// not possible right now, just fail
				return false;
			}
		}
		return true;
	}
	
	/**
	 *  @brief Overrides the = operator to perform a lock on the mutex when
	 *  needed.
	 *
	 *  @param value The value to be set.
	 */
	_AttributeType operator =(_AttributeType value) {
		this->Set(value, true);

		return this->attrValue;
	}

	/**
	 * @brief Sets the variable's value. Lock if needed.
	 * @param value The value to be set.
	 * @param blocking Whether the caller will set it is not possible to get
	 * 	the variable's mutex.
	 *
	 * @returns Whether the value was succesfully set.
	 */
	bool Set(_AttributeType value, bool blocking=true)
	{
		if (!this->Lock()) {
			return false;
		}
		// set the value
		this->attrValue = value;
		this->Unlock();

		return true;
	}

	/**
	 * @brief Overrides the -> operator to return a pointer to the
	 * 	value the object's represents.
	 *
	 * @returns A pointer to the object's value.
	 */
	_AttributeType * operator ->() {
		return &this->attrValue;
	}

	/**
	 * @brief Returns a reference to the object's value.
	 * 
	 * @todo Call the ReadLock
	 */
	_AttributeType &Get() {
		return this->attrValue;
	}

private:
	// sync control
	Mutex attrMutex;///< The writer mutex
	Mutex readLockCounterMutex; ///< A mutex to proctect the readLockCounter
	int readLockCounter; ///< The amount of reader's who got to lock the variable

	// attribute value
	_AttributeType attrValue; ///< The value this object actually represents
};

