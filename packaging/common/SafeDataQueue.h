#ifndef _SAFE_DATA_QUEUE_H
#define _SAFE_DATA_QUEUE_H

/**
 * @file SafeDataQueue.h
 * @brief Defines the SafeDataQueue class.
 */

#include "common/Semaphore.h"
#include "common/Mutex.h"
#include "common/MutexGuard.h"
#include <list>
#include <stdexcept>

/**
 * @brief This class is used to save and restore data in thread safe mode.
 */
template <class T> class SafeDataQueue
{
public:
    SafeDataQueue() { }
    ~SafeDataQueue() { }

    /**
     * @brief Waits for data.
     *
     * @param timeout Timeout to wait for data.
     *
     * @returns true if data arrived, otherwise false.
     */
    bool waitData(int timeout)
    {
        if (_sema.timedwait(timeout) == 0) {
            return true;
        }

        return false;
    }

    /**
     * @brief Gets the next saved data in the queue.
     *
     * @returns The next saved data.
     *
     * @throws std::logic_error.
     */
    T getData()
    {
        MutexGuard m(_mutex);

        if (_buffers.empty()) {
            throw std::logic_error("(SafeDataQueue) No data available");
        }

        T data = _buffers.front();
        _buffers.pop_front();

        return data;
    }

    /**
     * @brief Saves the specified data in the queue.
     *
     * @param data Data.
     */
    void saveData(const T &data)
    {
        MutexGuard mm(_mutex);

        _buffers.push_back(data);

        _sema.post();
    }

    /**
     * @brief Clears the queue.
     */
    void clear()
    {
        _buffers.clear();
    }

private:
    Semaphore _sema;
    Mutex _mutex;
    std::list<T> _buffers;
};

#endif // _SAFE_DATA_QUEUE_H

