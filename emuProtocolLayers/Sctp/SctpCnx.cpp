//#include <bits/basic_string.h>

#include "SctpCnx.h"
/*test by huanghao*/
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

SctpCnx::SctpCnx() {
    mpCallback = NULL;
    mTidCallback = 0;
    mbStopThread = true;
    mPayloadControl = SCTP_LOAD_READ_START;
    mIsSctpSocketClosed = false; // Is set only when connection is closed/reset by peer.
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

SctpCnx::~SctpCnx() {
	deleteAllSctp();
}

/**
@brief Sends data to a remote address.

@param msg Buffer with data to be sent.
@param len Buffer length in bytes.
@param stream Sctp stream to be used.
@param ordered Sends ordered data if true, otherwise sends unordered.
@returns Nonzero if the function is successful; otherwise 0, and a specific 
 error code can be retrieved by calling getLastError().
@see Sctp::getLastError
 */
bool SctpCnx::send(const void *msg, unsigned int len, unsigned short stream,bool ordered, void *pConn)
{
    //unsigned int ppid;
    unsigned int timetolive;
    unsigned int context;
    unsigned int flags;
    VectorString vetAddrs;
    unsigned short port;
    bool bRet = false;
	std::string s;
	Sctp *pSctp = NULL;
    mLog.log(LOG_TW_INFO, "SctpCnx::send() start.");

    s = "SctpCnx::Send(msg: " + Util::toHexString((intptr_t) msg) + "  len: " + Util::toString(len) +
            "  stream: " + Util::toString(stream) + "  ordered: " + Util::toString(ordered) +
            ") - BEGIN";
    //LOG_DBUG_JSI(s.c_str());

	if(pConn == NULL)
		return false;
    // set send values
    //ppid       = htonl(M3UA_PROTOCOL);   // M3UA
    timetolive = SCTP_SEND_TIME; // infinite time
    context = 0; // any context
	flags = ordered ? 0 : 1;
	pSctp = (Sctp *)pConn;

	// get remote addresses
	bRet = pSctp->getPeerAddrs(vetAddrs, port);

    if (bRet) {
        mLog.log(LOG_TW_INFO, "SctpCnx::send() peer addresses got.");
        bRet = false;
        //        for (unsigned int i = 0; i < vetAddrs.size(); i++){
        //            if(mSctpCnx.sendMsg(msg, len, this->ppid, flags, stream, timetolive, context, vetAddrs[i].c_str(), port) != -1){
        //               bRet = true;
        //               break;
        //            }
        //        }
		if (pSctp->sendMsg(msg, len, this->ppid, flags, stream, timetolive, context, vetAddrs[0].c_str(), port) != -1){
			bRet = true;
        }
        if (bRet == false) {
            mLog.log(LOG_TW_ERRO, "SctpCnx::send() send failed to any of the %d addresses.", vetAddrs.size());
        }
        //if (mSctpCnx.sendMsg(msg, len, this->ppid, flags, stream, timetolive, context, vetAddrs[0].c_str(), port) != -1)
        //        if (mSctpCnx.sendMsg(msg, len, this->ppid, flags, stream, timetolive, context, NULL, port) != -1)
        //        {
        //            bRet = true;
        //        }
        //        else
        //        {
        //            bRet = false;
        //        }
    }else{
        mLog.log(LOG_TW_ERRO, "SctpCnx::send() error getting peer addresses. Error Message: %s", Socket::getLastErrorMsg().c_str());
    }

    // log error
//    if (!bRet) {
//        s = this->getLastErrorMsg();
//        mLog.log(LOG_TW_ERRO, s.c_str());
//    }

    //LOG_DBUG_JSI("SctpCnx::Send() - END");

    return bRet;
}

bool SctpCnx::send(const void *msg, unsigned int len, unsigned short stream,
        bool ordered) {

    return send(msg, len, stream, ordered, (void *)&mSctpCnx);
}

/**
@brief Gets the error status for the last operation that failed.

@returns the return value indicates the error code (errno) for the last 
 sockets API routine performed by this thread.
 */
int SctpCnx::getLastError() {
    return errno;
}

/**
@brief Gets the error status for the last operation that failed as a string.

@returns the return string indicates the error string for the last 
 sockets API routine performed by this thread.
 */
std::string SctpCnx::getLastErrorMsg() {
    std::string s;

    s = std::string("(SCTP) ") + strerror(errno);

    return s;
}

/**
@brief Brings up the Sctp Thread for polling data.

@param pCallback callback function to be called to deliver incoming data.
@return true if the thread was started succesfully.
 */
bool SctpCnx::startThread(SctpUpperReceive *pCallback) {
    int r;
    std::string s;
    bool bRet = false;

    s = "SctpCnx::StartThread(pCallback: " + Util::toHexString((intptr_t) pCallback) + ") - BEGIN";
    //LOG_DBUG_JSI(s.c_str());

    mpCallback = pCallback;
    mbStopThread = false;

    r = pthread_create(&mTidCallback, NULL, SctpCnx::callbackThread, this);
    if (r == 0) {
        bRet = true;
        //LOG_DBUG_JSI("SctpCnx::StartThread() - Thread started");
    } else {
        errno = r; // errno gets pthread_create() return value
        bRet = false;
    }

    // log error
    if (!bRet) {
        s = this->getLastErrorMsg();
        //LOG_ERRO_JSI(s.c_str());
    }

    //LOG_DBUG_JSI("SctpCnx::StartThread() - END");

    return bRet;
}

/**
@brief Stops the Sctp thread.
 */
void SctpCnx::stopThread() {
    mLog.log(LOG_TW_DBUG, "SctpCnx::StopThread() - BEGIN");

    if (mTidCallback != 0) {
        mLog.log(LOG_TW_DBUG, "SctpCnx::StopThread() - Thread Join");
        mbStopThread = true;
        pthread_join(mTidCallback, NULL);
        mTidCallback = 0;
        mLog.log(LOG_TW_DBUG, "SctpCnx::StopThread() - Thread Stopped");
    }

    mLog.log(LOG_TW_DBUG, "SctpCnx::StopThread() - END");
}
Sctp* SctpCnx::allocSctp()
{
	Sctp *pSctp;
	pSctp = new Sctp;
	mSctpCnxs.push_back(pSctp);
	
	return pSctp;
}

void SctpCnx::deleteAllSctp()
{
	Sctp *pSctp;
	SctpConns::iterator iterPos = mSctpCnxs.begin();
	for(; iterPos != mSctpCnxs.end(); iterPos++)
	{
		pSctp = (* iterPos);
		mSctpCnxs.erase(iterPos);
		if(pSctp != NULL)
		{
			pSctp->close();
			delete pSctp;
		}
	}

	return;
}

void SctpCnx::deleteSctp(SctpConns::iterator iterPos)
{
	Sctp *pSctp;

	pSctp = (* iterPos);
	mSctpCnxs.erase(iterPos);

	if(pSctp != NULL)
	{
		pSctp->close();
		delete pSctp;
	}
	
}

SctpConns::iterator SctpCnx::getSctpConn(int index)
{
	int count = 0;
	SctpConns::iterator iterPos;

	iterPos = mSctpCnxs.begin();
	for(; iterPos != mSctpCnxs.end(); iterPos++, count++)
	{
		if(index == count)
		{
			return iterPos;
		}
	}

	return (SctpConns::iterator)NULL;
}

/**
@brief Static method invoked when the Sctp thread is started.

@param arg SctpCnx pointer.
 */
void *SctpCnx::callbackThread(void *arg) {
    std::string s;

    SctpCnx *pSctpCnx;

    pSctpCnx = (SctpCnx *) arg;

    pSctpCnx->callbackThread(); // execute thread

    pthread_exit(NULL);
}

/**
@brief Keeps polling data from the Sctp socket.
 */
void SctpCnx::callbackThread() {
    mLog.log(LOG_TW_DBUG, "SctpCnx::CallbackThread() - BEGIN");

    char msg[MAX_SCTP_FRAME];
    unsigned int len = MAX_SCTP_FRAME;
    unsigned int stream;
    unsigned int ppid;
    unsigned int context;
    unsigned int flags;
    int msg_flags;
    std::string addr;
    unsigned short port;
    struct pollfd pfd; // struciture to poll()
    bool bStop = false; // stop thread/loop
    std::string s;
    int ret;

    // set poll events
    pfd.fd = mSctpCnx.getRawSocket();
    pfd.events = POLLIN | POLLPRI;
    pfd.revents = 0;

    while (!mbStopThread && !bStop)
    {
        int r = poll(&pfd, 1, READ_POLL_TIMEOUT);
        if (r > 0)
        {
            if(SCTP_LOAD_READ_START == mPayloadControl)
            {
                ret= mSctpCnx.recvMsg((void *)msg, len, stream, ppid, context, flags, msg_flags, addr, port);
                if (ret > 0)
                {
                    if(ppid == this->ppid || this->ppid == UNSPECIFIED_PROTOCOL || ppid == UNSPECIFIED_PROTOCOL)
                    {
                        if ( !(msg_flags & MSG_NOTIFICATION) )
                        {
                            mLog.log(LOG_TW_DBUG, "SctpCnx::CallbackThread() - Data");
                            mpCallback->receive(msg, ret, stream);
                            mLog.log(LOG_TW_DBUG, "SctpCnx::CallbackThread() - Data arrived");
                        }
                    }
                }
                else if (ret == -1)
                {
                    mLog.log(LOG_TW_DBUG, "SctpCnx::CallbackThread() - ret == -1");
                    if (Sctp::getLastError() == ECONNRESET)
                    {
                        mLog.log(LOG_TW_INFO, "SctpCnx::CallbackThread() - ########## Connection reset by peer ##########");
                        //connection reset by peer
                    }
                    else
                    {
                        //mLog.log(LOG_TW_INFO, "SctpCnx::CallbackThread() - Any Error");
                        s = this->getLastErrorMsg();   
                        mLog.log(LOG_TW_ERRO, "SctpCnx::CallbackThread() recvMsg failed - error message: %s", s.c_str());
                    }

                    mSctpCnx.close();
                    mIsSctpSocketClosed = true;
                    bStop = true;
                    // RecvMsg() error
                }
                else if (ret == 0)
                {
                    mLog.log(LOG_TW_INFO, "SctpCnx::CallbackThread() - ########## Connection closed by peer ##########");
                    //connection closed by peer
                    mSctpCnx.close();
                    mIsSctpSocketClosed = true;
                    bStop = true;
                }
                else
                {
                    mLog.log(LOG_TW_INFO, "SctpCnx::CallbackThread() - ########## Connection closed Unknown reason,%d", ret);
                    mSctpCnx.close();
                    mIsSctpSocketClosed = true;
                    bStop = true;                
                }
            }
            else
            {
                sleep(1);
            }
        }
        else if (r == -1)
        {       
            mLog.log(LOG_TW_DBUG, "SctpCnx::CallbackThread() - poll() error");
            mSctpCnx.close();
            mIsSctpSocketClosed = true;
            bStop = true;              
            // poll() error
        }
        else if (r == 0)
        {
            // poll() timeout
        }
    }

    mLog.log(LOG_TW_DBUG, "SctpCnx::CallbackThread() - END");
}

/**
@brief Auxiliary method to print addresses of a specified VectorString object.

@param addrs List with the addresses.
 */
std::string SctpCnx::getAddrs(const VectorString &addrs) {
    std::string s;

    for (unsigned int i = 0; i < addrs.size(); i++) {
        s += addrs[i] + " ";
    }

    return s;
}

