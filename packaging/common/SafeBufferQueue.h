#ifndef _SAFE_BUFFER_QUEUE_H
#define _SAFE_BUFFER_QUEUE_H

/**
 * @file SafeBufferQueue.h
 * @brief Defines the SafeBufferQueue class.
 */

#include "common/Semaphore.h"
#include "common/Mutex.h"
#include "common/MutexGuard.h"
#include <vector>
#include <list>
#include <stdexcept>

/**
 * @brief This class is used to save and restore buffers in thread safe mode.
 */
template <class T> class SafeBufferQueue
{
public:
    SafeBufferQueue() { }
    ~SafeBufferQueue() { }

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
    std::vector<T> getData()
    {
        MutexGuard m(_mutex);

        if (_buffers.empty()) {
            throw std::logic_error("(SafeBufferQueue) No data available");
        }

        std::vector<T> buffer = _buffers.front();
        _buffers.pop_front();

        return buffer;
    }

    /**
     * @brief Saves the specified data buffer in the queue.
     *
     * @param buffer Data buffer.
     * @param size   Data buffer size.
     */
    void saveData(const T *buffer, size_t size)
    {
        MutexGuard mm(_mutex);

        std::vector<T> v;

        _buffers.push_back(v);

        std::vector<T> &buf = _buffers.back();

        buf.resize(size);

        std::copy(buffer, buffer + size, buf.begin());

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
    std::list<std::vector<T> > _buffers;
};

#endif // _SAFE_BUFFER_QUEUE_H

