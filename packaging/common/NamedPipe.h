#ifndef NAMEDPIPE_H_INCLUDED
#define NAMEDPIPE_H_INCLUDED

#include <string>
using std::string;


#define NAMEDPIPE_ERROR_CREATE_FILE1         -100
#define NAMEDPIPE_ERROR_CREATE_FILE2         -101
#define NAMEDPIPE_ERROR_CREATE_NP1           -102
#define NAMEDPIPE_ERROR_CREATE_NP2           -103
#define NAMEDPIPE_ERROR_CREATE_FLAGS         -104


/**
 * @brief This class implements a named pipe wrapper.
 * 
 * This wrapper will create (or open) a full-duplex named pipe with the given name.
 * Basically, 4 files are created (given two NamedPipe classes are created for the same namedpipe).
 * The files are:
 * name_1
 * name_2
 * name_lock1
 * name_lock2
 *
 * The lock files are used for internal control of the class. Files name_1 and name_2 are used for
 * full-duplex communication in the namedpipe.
 *
 * This class *IS* blocking. Meaning it will block when the class is instantiated and when
 * read/write calls are made.
 * The class will block on instantiation because it needs the two ends of the pipe to be "built",
 * thus, requiring two instances of the class for the same pipe.
 *
 * @author Daniel Cavalcanti Jeronymo <daniel.jeronymo.ext@siemens.com>
 */
class NamedPipe
{
private:
    string mName;      ///> Name of this pipe
    string mLock;      ///> Name of the lock file
    int mFile1;        ///> A file descriptor to the first namedpipe file. For a type 1 pipe, this is read-only. For a type 2, this is write-only.
    int mFile2;        ///> A file descriptor to the second namedpipe file For a type 1 pipe, this is write-only. For a type 2, this is read-only.
    
    char mPipeType;    ///> The type of this pipe. This value can be 1 or 2.
    
    char mError;       ///> In case of any errors in the creation, this value will be negative.
    
private:
    int createLockFile(const char *name, const char *flagName);
    
public:
    NamedPipe(const char *name);
    ~NamedPipe();
    
    string getName();
    string getLockName();
    char getError();
    
    int read(void *buffer,  unsigned int size, int timeout = -1);
    int write(void *buffer,  unsigned int size, int timeout = -1);
};

#endif
