#include "SctpServer.h"
#include <time.h>
#include <unistd.h>
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
SctpServer::SctpServer()
{
    _stopSctpServer = false;
}

SctpServer::SctpServer(bool ipCheck)
{
    mIpCheck = ipCheck;
	_stopSctpServer = false;
}
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
SctpServer::~SctpServer()
{
    this->stop();
}

/**
@brief Starts a SCTP server connection.

@param localAddrs Network local addresses.
@param localPort The local port identifying the socket application.
@param remoteAddrs The remote network address of the socket to which this object 
 is connected, like "192.168.80.187". Only the first address in the vector is used.
@param numOstreams Represents the number of outbound SCTP streams an application would like to request.
@param maxInstreams Represents the maximum number of inbound streams the application is prepared to allow.
@param pCallback Callback object to receive data from socket.
@param timeout Timeout in milliseconds. Not used.
@param numOstreamsOut  Inbound streams assigned.
@param maxInstreamsOut Outbound streams assigned.
@returns Nonzero if the function is successful; otherwise 0, and a specific 
 error code can be retrieved by calling getLastError().
@see Sctp::getLastError
*/
bool SctpServer::start(const VectorString &localAddrs, unsigned short localPort, 
                       const VectorString &remoteAddrs, unsigned short remotePort,
                       unsigned short numOstreams, unsigned short maxInstreams,
                       SctpUpperReceive *pCallback, int timeout,
                       unsigned short &numOstreamsOut, unsigned short &maxInstreamsOut)
{
    mLog.log(LOG_TW_DBUG, "SctpServer::start() - BEGIN");
    std::string s;
    bool bRet = true;

    _localAddrs = localAddrs;
    _localPort = localPort; 
    _remoteAddrs = remoteAddrs; 
    _remotePort = remotePort;
    _numOstreams = numOstreams;
    _maxInstreams = maxInstreams;
    _timeout = timeout;
    _numOstreamsOut = numOstreamsOut;
    _maxInstreamsOut = maxInstreamsOut;      
    
    s = "SctpServer::Start(localAddrs: " + this->getAddrs(localAddrs) + "  localPort: " + Util::toString(localPort) +
        "  remoteAddrs: " + this->getAddrs(remoteAddrs) + "  remotePort: "  + Util::toString(remotePort) +
        "  numOstreams: " + Util::toString(numOstreams) + "  maxInstreams: " +  Util::toString(maxInstreams) +
        "  pCallback: " + Util::toHexString((intptr_t)pCallback) + "  timeout: " + Util::toString(timeout) + ") - BEGIN";
    mLog.log(LOG_TW_DBUG, s.c_str());

    bRet = mSctpServer.create();

    if (bRet)
        bRet = mSctpServer.setNotifications(true);

    if (bRet)
        bRet = mSctpServer.init(numOstreams, maxInstreams);

    if (bRet)
        bRet = mSctpServer.bind(localAddrs, localPort);

    if (bRet)
        bRet = mSctpServer.listen();

    if (bRet && mIpCheck)
        bRet = this->accept(remoteAddrs, remotePort, timeout);

    if (bRet && mIpCheck)
	{
        mSctpCnx.getStatus(maxInstreamsOut, numOstreamsOut);
	}
    
    if (bRet)
        bRet = this->startThread(pCallback);

    // verify error
    if (!bRet && mIpCheck)
    {
        s = this->getLastErrorMsg();
        mLog.log(LOG_TW_ERRO, s.c_str());
        
        this->stopThread();
        mSctpServer.close(); // close server
        mSctpCnx.close();    // close connection
    }

    //LOG_DBUG_JSI("SctpServer::Start() - END");
    mLog.log(LOG_TW_DBUG, "SctpServer::start()bRet=%d - END", bRet);
    return bRet;
}

/**
@brief Stops the SCTP server connection.
*/
void SctpServer::stop()
{
    mLog.log(LOG_TW_DBUG, "SctpServer::Stop() - BEGIN");
	if(mIpCheck == false)
	{
		stopServer();
		return;
	}
    
    // wait callbackThread() termination
    mLog.log(LOG_TW_DBUG, "SctpServer::Stop() - Stopping Thread");
    _stopSctpServer = true;
    this->stopThread();
    
    // close sockets
    mLog.log(LOG_TW_DBUG, "SctpServer::Stop() - Closing Server");
    mSctpServer.close();
    mLog.log(LOG_TW_DBUG, "SctpServer::Stop() - Closing Socket");
    mSctpCnx.close();
    
    mLog.log(LOG_TW_DBUG, "SctpServer::Stop() - END");
}

void SctpServer::stopServer()
{
	_stopSctpServer = true;

	deleteAllSctp();
	mSctpServer.close();
	
	this->stopThread();
	 mLog.log(LOG_TW_DBUG, "SctpServer::Stop() - END");
	
}
/**
@brief Accepts SCTP connection with the specified address/port pairs.

@param remoteAddrs Sctp client addresses to accept connections.
@param remotePort Sctp client port to accept connections.
@returns true if it was able to accept the peer connection.
*/
bool SctpServer::accept()
{
    bool bRet = true;
	Sctp *pSctp;

        pSctp = allocSctp();
    bRet = mSctpServer.accept(*pSctp);

    return bRet;
}
bool SctpServer::accept(const VectorString &remoteAddrs, unsigned short remotePort, 
        int timeout)
{
    //LOG_DBUG_JSI("SctpServer::Accept() - BEGIN");
    
    struct pollfd pfd;
    unsigned int ct;              // current timeout
    bool bIsTimeExpired = false;
    bool bFound = false;
    std::string s;
    bool bRet = true;
    unsigned int nRoundTime = 500; // 500ms every round
    unsigned int nExpiredTime = 0;
    pfd.fd      = mSctpServer.getRawSocket();
    pfd.events  = POLLIN | POLLPRI;
    pfd.revents = 0;
    
    if (timeout > 0)
    {
        while(!bFound && !bIsTimeExpired && bRet && !_stopSctpServer)
        {
            ct = timeout;
            //mLog.log(LOG_TW_WARN, "SctpServer::Accept() begin to accept");
            int r = poll(&pfd, 1, nRoundTime);
            if (r > 0)
            {
                //LOG_DBUG_JSI("SctpServer::Accept() - Calling mSctpServer.Accept()");
                bRet = mSctpServer.accept(mSctpCnx);
                if (bRet)
                {    
//                    mLog.log(LOG_TW_INFO,"SctpServer::Accept() - Verify remote address");
                    if (this->isValidAddr(remoteAddrs, remotePort, mSctpCnx))
                    {
                        //LOG_DBUG_JSI("SctpServer::Accept() - Required address was found");
                        bFound = true;       // required address found
                        mSctpServer.close(); // do not accept connections anymore
                    }
                    else
                    {
                        //LOG_WARN_JSI("SctpServer::Accept() - It is not the required address ... try again");
                        mLog.log(LOG_TW_WARN, "SctpServer::Accept() - It is not the required address ... try again");
                        mSctpCnx.close();  // it is not the required address ... try again
                    }
                }
            }
            else if (r == -1)
            {
                // poll() error
                bRet = false;
            }
            else if (r == 0)
            {
                nExpiredTime += nRoundTime;
                // poll() timeout
                if(nExpiredTime >= timeout)
                {
                    bIsTimeExpired = true;
                    errno = ETIME;   // Timer expired
                    bRet = false;
                }
            }
        }
    }
    else
    {
        errno = EINVAL;  //Invalid argument
        bRet = false;
    }

    //LOG_DBUG_JSI("SctpServer::Accept() - END");

    return bFound;
}

/**
@brief Checks if the specified address matches the values of the incoming sctp socket connection.
@param localAddrs List with peers local addresses.
@param remoteAddrs List with peers remote addresses.
@sctpHost Sctp object with the incoming sctp parameters.
@return true if match found, false if not
*/
//bool SctpServer::nsnPrivateAddrsCmp(const VectorString localAddrs,const VectorString remoteAddrs,Sctp &sctpHost)
//{
//    int len = localAddrs.size();
//    if(localAddrs.size() != remoteAddrs.size())
//    {
//        mLog.log(LOG_TW_WARN, "SctpServer::nsnPrivateAddrsCmp() - different address list size, local : %d , remote : %d.", localAddrs.size(), remoteAddrs.size());
//    }
//    for (unsigned int i=0; i<len; i++)
//    {
//        if (sctpHost.addrsCmp(remoteAddrs[i], localAddrs[i]))
//                        bRet = true;
//    }
//}

/**
@brief Checks if the specified pair of address/port matches with the values of the incoming sctp socket connection.

@param remoteAddrs List with peers remote addresses.
@param remotePort Port of the sctp peers.
@sctpHost Sctp object with the incoming sctp parameters.
*/
bool SctpServer::isValidAddr(const VectorString &remoteAddrs, unsigned short remotePort, 
                             Sctp &sctpHost)
{
    VectorString vetAddrs;
    unsigned short port;
    bool bRet = false;
    
    if (sctpHost.getPeerAddrs(vetAddrs, port))
    {
        if (remotePort == port)
        {
//            this->mLog.log(LOG_TW_INFO, "remotePort == port = %u", port);
            for (unsigned int i=0; i<remoteAddrs.size() && !bRet; i++)
                for (unsigned int j=0; j<vetAddrs.size() && !bRet; j++)
                {
//                    this->mLog.log(LOG_TW_INFO, "remoteAddrs[%u] = %s, vetAddrs[%u]= %s",i, remoteAddrs[i].c_str(), j, vetAddrs[j].c_str());
                    if (sctpHost.addrsCmp(remoteAddrs[i], vetAddrs[j]))
                        bRet = true;
                }
        }
//        this->mLog.log(LOG_TW_INFO, "remotePort =%u, port = %u", remotePort, port);
    }
    else
    {
        this->mLog.log(LOG_TW_ERRO, "cannot get peer addresses, error: %s, please make sure SELINUX is disabled.", sctpHost.getLastErrorMsg().c_str());
    }
        
    return bRet;
}
bool SctpServer::send(const void *msg, unsigned int len, unsigned short stream, bool ordered, char *ip, unsigned short &port)
{
	Sctp *pSctp;
	VectorString vetAddrs;
	std::string sIp;

	sIp = ip;
	vetAddrs.push_back(sIp);
	if(mIpCheck == true)
	{
		if(isValidAddr(vetAddrs, port, mSctpCnx))
		{
			SctpCnx::send(msg, len, stream, ordered, (void *)&mSctpCnx);
			return true;
		}
	}
	else
	{
		for(SctpConns::iterator iterPos = mSctpCnxs.begin(); iterPos != mSctpCnxs.end(); iterPos++)
		{
	            
			pSctp = (*iterPos);
			if(isValidAddr(vetAddrs, port, (*pSctp)))
			{
				this->mLog.log(LOG_TW_INFO, "***begin to send***");
				SctpCnx::send(msg, len, stream, ordered, (void *)pSctp);
				return true;
			}
		}
	}

	return false;
	
}

int SctpServer::receiveDataIndExOfIpPort(char *ip, unsigned short port)
{
	char buf[SCTP_TEMP_BUF_LEN];
	int buflen = 0;
	int iplen = 0;
	unsigned short bufport;
	
	if(ip == NULL || port == 0)
		return 0;

	memset(buf, 0, sizeof(char)*SCTP_TEMP_BUF_LEN);
	buf[buflen] = DATAINDEXT_FLAG_IP;
	buflen++;

	iplen = (strlen(ip)+1);
	buf[buflen] = iplen;
	buflen++;
	
	memcpy(&buf[buflen], ip, iplen);
	buflen+=iplen;
	
	buf[buflen] = DATAINDEXT_FLAG_PORT;
	buflen++;

	buf[buflen] = sizeof(short);
	buflen++;
	
	bufport = htons(port);
	memcpy(&buf[buflen], &bufport, sizeof(short));
	buflen+=sizeof(short);

	mpCallback->receiveEx(buf, buflen);
	return buflen;
	
}

int SctpServer::receiveDataIndExOfDisconnect()
{
	int buflen = 0;
	char buf[SCTP_TEMP_BUF_LEN];

	memset(buf, 0, sizeof(char)*SCTP_TEMP_BUF_LEN);
	buf[buflen] = DATAINDEXT_DISCONNECT;
	buflen++;

	buf[buflen] = 1;
	buflen++;

	buf[buflen] = 0;
	buflen++;

	mpCallback->receiveEx(buf, buflen);
	return buflen;
}
void SctpServer::callbackSingleClientThread()
{
    bool bRet = true;
    while(!_stopSctpServer)
    {
        if(bRet)
            SctpCnx::callbackThread();
        mSctpServer.close();
        usleep(10000);
        bRet = mSctpServer.create();        
        if (bRet)
        {
            mLog.log(LOG_TW_DBUG, "SctpServer::create() - succeed");
            bRet = mSctpServer.setNotifications(true);        
        }
        if (bRet)
        {
            mLog.log(LOG_TW_DBUG, "SctpServer::setNotifications() - succeed");
            bRet = mSctpServer.init(_numOstreams, _maxInstreams);
        }
        if (bRet)
        {
            mLog.log(LOG_TW_DBUG, "SctpServer::init() - succeed");
            bRet = mSctpServer.bind(_localAddrs, _localPort);
        }
        if (bRet)
        {
            mLog.log(LOG_TW_DBUG, "SctpServer::bind() - succeed");
            bRet = mSctpServer.listen();
        }
        if (bRet)
        {
            mLog.log(LOG_TW_DBUG, "SctpServer::listen() - succeed");
            bRet = this->accept(_remoteAddrs, _remotePort, _timeout);
        }
        if (bRet)
        {
            mLog.log(LOG_TW_DBUG, "SctpServer::accept() - succeed");
            mSctpCnx.getStatus(_maxInstreamsOut, _numOstreamsOut);   
            mIsSctpSocketClosed = false; // Is set only when connection is closed/reset by peer.
        }
    }
}
void SctpServer::callbackThread()
{
    char msg[MAX_SCTP_FRAME];
    unsigned int len = MAX_SCTP_FRAME;
    unsigned int stream;
    unsigned int ppid;
    unsigned int context;
    unsigned int flags;
    int msg_flags;
    std::string addr;
    unsigned short port;
    struct pollfd pfd[SCTP_FP_TEMP_BUF];
	int fdnum = 0;
	int indfd;
	int timeout = 0;
    bool bStop = false; 
    std::string s;
    int ret;
	Sctp *pSctp;
	SctpConns::iterator iterPos;
	char acceptflag = 0;

	if(mIpCheck)
	{
		callbackSingleClientThread();
		return;
	}

	pfd[0].fd = mSctpServer.getRawSocket();
	pfd[0].events = POLLIN | POLLPRI;
	pfd[0].revents = 0;

    while (!bStop && !mbStopThread)
    {
		iterPos = mSctpCnxs.begin();
		for(fdnum=0; iterPos != mSctpCnxs.end(); iterPos++, fdnum++)
		{
			if(fdnum+1 >= SCTP_FP_TEMP_BUF)
				break;
		    pfd[fdnum+1].fd = (* iterPos)->getRawSocket();
		    pfd[fdnum+1].events = POLLIN | POLLPRI;
		    pfd[fdnum+1].revents = 0;
		}
		
        int r = poll(pfd, (unsigned long)fdnum+1, READ_POLL_TIMEOUT);
        if (r > 0)
        {
        	for(indfd=0; indfd<=fdnum; indfd++)
    		{
    			if(pfd[indfd].revents)
				{
					if(indfd == 0)
					{
						this->accept();
						acceptflag = 1;
						continue;
					}
					
		            if(SCTP_LOAD_READ_START == mPayloadControl)
		            {
		            	pSctp = NULL;
		            	iterPos = getSctpConn(indfd-1);
						if((* iterPos) == NULL)
						{
							 mLog.log(LOG_TW_ERRO, "SctpCnx::getSctpConn error!");
							 continue;
						}
						pSctp = (* iterPos);
						
		                ret= pSctp->recvMsg((void *)msg, len, stream, ppid, context, flags, msg_flags, addr, port);
		                if (ret > 0)
		                {
		                    if(ppid == this->ppid || this->ppid == UNSPECIFIED_PROTOCOL || ppid == UNSPECIFIED_PROTOCOL)
		                    {
		                        if ( !(msg_flags & MSG_NOTIFICATION) )
		                        {
		                            mLog.log(LOG_TW_DBUG, "SctpCnx::CallbackThread() - Data");
									receiveDataIndExOfIpPort((char *)addr.c_str(), port);
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
		                    }
		                    else
		                    {
		                        s = this->getLastErrorMsg();                  
		                    }

		                    pSctp->close();
							deleteSctp(iterPos);
							
		                }
		                else if (ret == 0)
		                {
		                    mLog.log(LOG_TW_INFO, "SctpCnx::CallbackThread() - ########## Connection closed by peer ##########");
		                    pSctp->close();
							deleteSctp(iterPos);
		                }
		                else
		                {
		                    mLog.log(LOG_TW_INFO, "SctpCnx::CallbackThread() - ########## Connection closed Unknown reason,%d", ret);
		                    pSctp->close();
							deleteSctp(iterPos);              
		                }
		            }
				}
    		}
        }
        else if (r == -1)
        {       
        	mSctpServer.close();
			deleteAllSctp();
			bStop = true;
        }
        else if (r == 0)
        {
        	if(acceptflag == 1)
    		{
    			continue;
    		}
        	timeout+=READ_POLL_TIMEOUT;
			if(_timeout <= timeout)
			{
				receiveDataIndExOfDisconnect();
				mSctpServer.close();
				deleteAllSctp();
				bStop = true;
			}
        }
    }
}