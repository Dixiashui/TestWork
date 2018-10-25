#ifndef _READWRITEMUTEX_H
#define _READWRITEMUTEX_H

/**
 * @file ReadWriteMutex.h
 * @brief Defines the ReadWriteMutex class.
 */

#include <pthread.h>
#include "common/Util.h"

class ReadWriteMutexException {
public:

    ReadWriteMutexException(const char *s) {
        mMsg = s;
    }

    std::string getMsg() {
        return mMsg;
    }

private:
    std::string mMsg;
};

/**
 * @brief ReadWriteMutex class is a wrapper for Linux pthread_rwlock_t object.
 * 
 * @author Alexandre Maia Godoi <alexandre.godoi@nsn.com>
 */
class ReadWriteMutex {
public:
    ReadWriteMutex();
    ~ReadWriteMutex();

    int rdlock();
    int tryrdlock();
    int timedrdlock(int timeout);

    int wrlock();
    int trywrlock();
    int timedwrlock(int timeout);

    int unlock();

private:
    pthread_rwlock_t mMutex; ///< Mutex object.

    ReadWriteMutex(const ReadWriteMutex &r); // copy is not allowed
    ReadWriteMutex & operator=(const ReadWriteMutex &r); // assign is not allowed
};

#endif // _READWRITEMUTEX_H

