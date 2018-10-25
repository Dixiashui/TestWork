#include <stdio.h>
#include <errno.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/poll.h>
#include <fcntl.h>
#include "NamedPipe.h"

#define NP_WRITE (O_WRONLY)
#define NP_READ  (O_RDONLY)

#define NP_ALL   0777 


/**
 * @brief Constructs a namedpipe.
 *
 * @param name The base name of the namedpipe files.
 */
NamedPipe::NamedPipe(const char *name)
{
    // sets the pipe name
    mName = name;
    
    // init
    mFile1 = -1;
    mFile2 = -1;
    mError = 0;
    
    // create the first namedpipe
    string file1 = name;
    file1 += "_1";

    if ((mkfifo(file1.c_str(), NP_ALL) == -1) && (errno != EEXIST))
    {
        mError = NAMEDPIPE_ERROR_CREATE_FILE1;
        return;
    }

    // create the second namedpipe
    string file2 = name;
    file2 += "_2";

    if ((mkfifo(file2.c_str(), NP_ALL) == -1) && (errno != EEXIST))
    {
        mError = NAMEDPIPE_ERROR_CREATE_FILE2;
        return;
    }
    
    // force the file permissions in case mkfifo doesnt
    chmod(file1.c_str(), NP_ALL);
    chmod(file2.c_str(), NP_ALL);
    
    // setup this namedpipe as a type1 or type2
    if(createLockFile(name, "1") == 0)
    {
        if((mFile1 = open(file1.c_str(), NP_READ)) < 0 || (mFile2 = open(file2.c_str(), NP_WRITE)) < 0)
        {
            mError = NAMEDPIPE_ERROR_CREATE_NP1;
            close(mFile1);
            close(mFile2);
            return;
        }
        
        mPipeType = 1;
    }
    else if(createLockFile(name, "2") == 0)
    {
        if((mFile1 = open(file1.c_str(), NP_WRITE)) < 0 || (mFile2 = open(file2.c_str(), NP_READ)) < 0)
        {
            mError = NAMEDPIPE_ERROR_CREATE_NP2;
            close(mFile1);
            close(mFile2);
            return;
        }
        
        mPipeType = 2;
    }
    else
    {
        mError = NAMEDPIPE_ERROR_CREATE_FLAGS;
    }
}
 
/**
 * @brief Closes open handles and deletes the flag files.
 */
NamedPipe::~NamedPipe()
{
    if(mError == 0)
    {
        close(mFile1);
        close(mFile2);

        // force the removal of both files
        string pipeFile1 = mName;
        string pipeFile2 = mName;

        pipeFile1 += "_1";
        pipeFile2 += "_2";

        remove(pipeFile1.c_str());
        remove(pipeFile2.c_str());

        // remove the lock file of this pipe
        remove(mLock.c_str());
    }
}

/**
 * @brief Gets the name of the namedpipe file.
 *
 * @returns A string with the name of the namedpipe file.
 */
string NamedPipe::getName()
{
    return mName;
}

/**
 * @brief Gets the name of the namedpipe file.
 *
 * @returns A string with the name of the namedpipe file.
 */
string NamedPipe::getLockName()
{
    return mLock;
}

/**
 * @brief Returns internal errors.
 *
 * @returns An error value. Described in NamedPipe.h
 */
char NamedPipe::getError()
{
    return mError;
}

/**
 * @brief Reads data from the pipe.
 *
 * @param buffer The buffer to store the read data.
 *
 * @param size The maximum size of the buffer.
 *
 * @param timeout A timeout value for a nonblocking read. If negative, performs a blocking read.
 *
 * @returns 0 if ok. -1 on error.
 */
int NamedPipe::read(void *buffer,  unsigned int size, int timeout)
{
    int file = -1;
    
    if(mPipeType == 1)
        file = mFile1;
    else if(mPipeType == 2)
        file = mFile2;
    else
        return -1;

    if(timeout > -1)
    {
        struct pollfd pollDescriptor = { file, POLLIN, 0 };

        poll(&pollDescriptor, 1, timeout);
        if(!(pollDescriptor.revents & POLLIN))
            return -1;
    }
    
    return ::read(file, buffer, size);
}

/**
 * @brief Writes data to the pipe.
 *
 * @param buffer The buffer containing the data to output.
 *
 * @param size The size of the buffer to be sent.
 *
 * @param timeout A timeout value for a nonblocking write. If negative, performs a blocking write.
 *
 * @returns 0 if ok. -1 on error.
 */
int NamedPipe::write(void *buffer,  unsigned int size, int timeout)
{
    int file = -1;
    
    if(mPipeType == 1)
        file = mFile2;
    else if(mPipeType == 2)
        file = mFile1;
    else
        return -1;

    if(timeout > -1)
    {
        struct pollfd pollDescriptor = { file, POLLOUT, 0 };

        poll(&pollDescriptor, 1, timeout);
        if(!(pollDescriptor.revents & POLLOUT))
            return -1;
    }
    
    return ::write(file, buffer, size);
}

/**
 * @brief Creates a flag pipe, used only to signal that file1 is being used.
 *
 * @param name The base name of the flag pipe.
 *
 * @param flagName The sufix of the flag.
 *
 * @returns 0 if the flag is created. -1 if an error occurs (including if the flag exists).
 */
int NamedPipe::createLockFile(const char *name, const char *flagName)
{
    // its safe to overwrite mLock here because createLockFile should be called only once or twice
    mLock = name;
    mLock += "_flag";
    mLock += flagName;
        
    return mkfifo(mLock.c_str(), NP_ALL);
}

