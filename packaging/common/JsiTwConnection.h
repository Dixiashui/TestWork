#ifndef _JSITWCONNECTION_H
#define _JSITWCONNECTION_H

#include "common/Socket4.h"
#include "common/Mutex.h"
#include "common/TwLog.h"

//#include <iostream>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <sys/poll.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <errno.h>
#include <netdb.h>
#include <unistd.h>
#include <string.h>
#include <netinet/tcp.h>
#include <netinet/in.h>
#include <string>
#include <typeinfo>

#define CONFIGREQ            1
#define CONFIGREQRESP        2
#define SEND                 3
#define SENDRESP             4
#define DATAIND              5
#define LOGMSG               6
#define CLEANUP              7
#define CLEANUPRESP          8
#define GETPARAM             9
#define GETPARAMRESP        10
#define GETERRORSTRING      11
#define GETERRORSTRINGRESP  12
#define GETPARAMSTRING      13
#define EXECCOMMAND         14
#define EXECCOMMANDRESP     15
#define SETLOGLEVEL         16

#define DATAINDEXT          17

#define DATAINDEXT_BUF_LEN 50
#define DATA_BUF_LEN 100000

#define LOG_FUNCTION_NOT_INPLEMENTED    mTwLog.log(LOG_TW_ERRO, "%s is not implemented yet", __FUNCTION__)

#define DEFINE_DEFAULT_GET_ERROR_MSG \
    static const char* mErrorMsg[];             \
    static const int maxErrorNum;               \
    virtual std::string getErrorMsg(int error) \
    {                                       \
        if(error < 0 || error > maxErrorNum)                    \ 
        return "unknown error number";               \
        return std::string(mErrorMsg[error]);}
     

#define PRINT_PARAM_VALUE(paramname, value)   \
        mTwLog.log(LOG_TW_DBUG, "%s::%s: %s -> %s", getStackName().c_str(), __FUNCTION__, #paramname, Util::toString(value).c_str());

#define GET_CONFIG_STRING(ret, paramname, value)  \
    if (!ret) {                                                         \
        ret = getConfig(#paramname, value);                               \
        mTwLog.log(LOG_TW_DBUG, "%s::%s: %s -> %s", getStackName().c_str(), __FUNCTION__, #paramname, value.c_str());}

#define GET_CONFIG_INT(ret, paramname, value)  \
    if (!ret) {                                                                 \
        ret = getConfig(#paramname, &value, sizeof (value));                    \
        PRINT_PARAM_VALUE(paramname, value)                                   \
    }
    
#define GET_CONFIG_BOOL(ret, paramname, value)  \
    if (!ret) {                                                                 \
        std::string str;                                                \
        ret = getConfig(#paramname, str);                               \
        if (strcmp(str.c_str(), "false") == 0)                          \
            value = false;                                              \
        else                                                            \
            value = true;                                               \
        PRINT_PARAM_VALUE(paramname, value)                                   \
    }
    
/**
  @brief This class represents a TestworkConnection that communicates with Pegasus using sockets.

 It was initially developed to bypass a limitation of CCPU MPT3 stack
 that only allows one MTP3 instance per process.
 So, Pegasus will fork several native process and communicate
 with these process using sockets.
 This class provides the basic mechanisms for this communication.

 Pegasus will open a server socket just before to fork this
 process. Then, it launches this application passing the port number as parameter.
 When this program starts, it launches a thread for communicate with
 Pegasus and continues running the stack. The launched thread
 will connect to the port received as argument and use it for
 communication.
 
 @author Samuel Lucas Vaz de Mello
 @date 08 dec 2005.
*/
class JsiTwConnection 
{
protected:
    bool mIsConfigured;
    
    char *mPort; ///< the port to connect to

    // Testwork LOG
    TwLog mTwLog;
    
    // pointer to a instance of this class. 
    // Used by logMsg that is static
    static JsiTwConnection *mCon;
    char mDataIndExtBuf[DATAINDEXT_BUF_LEN];
    int mDataIndExtLen;

    // log methods
    void logDbug(const std::string &s);
    void logTrac(const std::string &s);
    void logInfo(const std::string &s);
    void logWarn(const std::string &s);
    virtual void logErro(int err);    

public:

    // constuctor & destructor
    JsiTwConnection(char *port);
    virtual ~JsiTwConnection();
    std::string getStackName();
   
    // these functions will be overriden by the class that 
    // inherit from this and implements a stack 
    virtual int send(char *buf, int size) = 0; ///< virtual function to handle SEND requests
    virtual int configReq() = 0; ///< virtual function to handle CONFIGREQ requests
    virtual int cleanUp() = 0; ///< virtual function to handle CLEANUP requests
    virtual void dataInd(char *buf, int size);
    virtual void dataIndExt(char *buf, int size);
    virtual int send(char *buf, int size, char *bufext, int len) {mTwLog.log(LOG_TW_WARN,"only for sctp ip"); return 0;};
    virtual int execCommand(const char *buf);
    
    
    /**
     * @brief virtual function to return error messages for Pegasus
     *
     * Pegasus sends JSI commands to JSITwConnection, such as SEND or CONFIGREQ. 
     * These command are processed by processMsg, which calls the corresponding handling 
     * function and sends back to Pegasus the return code.
     * 
     * These handling functions (as JSITwConnection->send) are expected to return 0
     * on success and any value different of 0 on error. When Pegasus receives a non-zero 
     * return (an error) it calls getErrorMsg passing as parameter the non-zero error code
     * to get a string describing the error.
     *
     * 
<pre>     
 Pegasus      (tcp messages)        JSITwCon     function call
--------                           ---------- 
    |            (send,x)              |                   .
    |=================================>|     send(x)       :
    |                                  |------------------>|
    |                                  |       -26         |
    |                                  |<------------------|
    |          (sendresp,-26)          |                   :
    |<=================================|                   .
    |                                  |
    |                                  |
    |                                  |
    |                                  |
    |                                  |
    |                                  |
    |         (geterrormsg,-26)        |
    |=================================>|                   .
    |                                  |  getErrorMsg(-26) :
    |                                  |------------------>|
    |                                  |   "Broken Pipe"   |
    |                                  |<------------------|
    | (getErrorMsgResp,"Broken Pipe")  |                   :
    |<=================================|                   .
</pre>
     */
    virtual std::string getErrorMsg(int error) = 0; 
    
    
    static inline JsiTwConnection *getInstance() { return mCon; };
private:
    Socket4 mSocket; ///< socket use for communication with Pegasus
    
    Mutex mMutexSend; ///< mutex for avoid two sends at the same time.
   
    bool logEnabled;
    
    // receive messages and deliver it for processMsg
    void receiveMessages();

    // process a received message
    void processMsg(int actId, char *buffer, int size);

public:
    // sends a message for Pegasus
    void sendMessage (int actId, void *buffer, int size);

    // start processing
    void run();

    // to be overriden with user code
    // called by run();
    void init(); 
    
    bool inline getLogEnabled(){  return logEnabled; };
    
    // ask Pegasus for the value of a parameter and return it
    int getConfig(const char* paramName, void *ret, int sizeOfRet);
    
    // ask Pegasus for the value of a parameter and return it
    int getConfig(const char* paramName, std::string &s);

    // sends a log message for Pegasus        
    static void logMsg(int severity, const char *msg);
};

#endif // ifndef _JSITWCONNECTION_H

