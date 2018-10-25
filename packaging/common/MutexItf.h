#ifndef _MUTEX_ITF_H
#define _MUTEX_ITF_H

/**
 * @brief MutexItf defines a general interface for interface. 
 * 
 * @author Gabriel Takeuchi <gabriel.takeuchi@siemens.com>
 */
class MutexItf
{
public:
	virtual ~MutexItf() {}
    virtual int lock() = 0;
    virtual int trylock() = 0;
    virtual int unlock() = 0;

    virtual int timedlock(int timeout) = 0;

};

#endif // _MUTEX_H

