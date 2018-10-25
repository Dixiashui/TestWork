#include "SbSctpIp.h"

/**
 * @brief SbSctpIp constructor.
 *
 * Pass port parameter to JsiTwConnection constructor and starts 
 * all variable members.
 */
SbSctpIp::SbSctpIp(int id, char *port)
    :JsiTwConnection(port)
{
    mOutStream = DEFAULT_SCTP_OUT_STREAM;
	this->mpSctpConn = NULL;
}

/**
 * @brief SbSctpIp destructor.
 */
SbSctpIp::~SbSctpIp()
{
    this->cleanUp();
}
    
/**
 * @brief Executes a generic command.
 *
 * @param buf   Buffer with COMMAND + DATA SIZE + DATA.
 */
int SbSctpIp::execCommand(const char *buf)
{
    int command;
    int msgSize;
    char *msg;
    int ret = SB_SCTP_IP_OK;
    
    parseCommand(buf, command, msgSize, msg);

    switch (command)
    {
        case COMMAND_SET_OUTPUT_STREAM:
            ret = setOutputStream((int)*msg);
            break;
        
        case COMMAND_SET_SCTP_LOAD_CONTROL:
            ret = setSctpLoadControl((int)*msg);
            break;
                        
        default:
            ret = SB_SCTP_IP_CANNOT_EXECUTE_COMMAND;
            break;
    }
            
    if (ret) 
    {
        mLog.log(LOG_TW_ERRO, (getErrorMsg(ret)).c_str());
    }
    
    
    return ret;
}

/**
 * @brief Sends data through Sctp/IP hierarchy.
 *
 * @param buff  Data buffer to be sent.
 * @param size  Data buffer size.
 *
 * @returns Error code.
 */
int SbSctpIp::send(char *buffer, int size)
{
    mLog.log(LOG_TW_DBUG, "SbSctpIp::send() - BEGIN");
    
    int r = 0;
    int ret = SB_SCTP_IP_OK;
    unsigned short stream = mOutStream;
    
    // Checks if Sctp Socket is still connected to peer
    if (mpSctpConn->mIsSctpSocketClosed) 
    {
        mLog.log(LOG_TW_ERRO,"SbSctpIp::send() - sctp socket is already closed.");
    	ret = SB_SCTP_IP_CANNOT_SEND_DATA;

    }
    else if (mpSctpUpperSend != NULL)
    {
        r = mpSctpUpperSend->send((const void *)buffer, size, stream, true);
        if (r)
        {
            mLog.log(LOG_TW_DBUG, "SbSctpIp::send() buffer: %p, size: %d, stream: %d", buffer, size, stream);
            ret = SB_SCTP_IP_OK;
        }
        else
        {
            ret = SB_SCTP_IP_CANNOT_SEND_DATA;
            mLog.log(LOG_TW_ERRO, (this->getErrorMsg(ret)).c_str());
        }
    }
    else
    {
        ret = SB_SCTP_IP_NOT_CONFIGURED;
    }
    mLog.log(LOG_TW_DBUG, (this->getErrorMsg(ret)).c_str());
    mLog.log(LOG_TW_DBUG, "SbSctpIp::send() - END");

    return ret;
}

/**
 * @brief Converts a given SB_M3UA error code to a string explaining the error.
 * 
 * @param error error code
 * @returns string explaining the error
*/
std::string SbSctpIp::getErrorMsg(int error)
{
    std::string s;

    switch (error)
    {
        case SB_SCTP_IP_NOT_CONFIGURED:
            s = STR_SB_SCTP_IP_NOT_CONFIGURED;
            break;

        case SB_SCTP_IP_CANNOT_SEND_DATA:
            s = STR_SB_SCTP_IP_CANNOT_SEND_DATA;
            break;

        case SB_SCTP_IP_ALREADY_CONFIGURED:
            s = STR_SB_SCTP_IP_ALREADY_CONFIGURED;
            break;

        case SB_SCTP_IP_CANNOT_GET_CONFIG_VALUES:
            s = STR_SB_SCTP_IP_CANNOT_GET_CONFIG_VALUES;
            break;

        case SB_SCTP_IP_CANNOT_CONFIGURE:
            s = STR_SB_SCTP_IP_CANNOT_CONFIGURE;
            break;
            
        case SB_SCTP_IP_SCTP_CONNECTION_FAIL:
            s = STR_SB_SCTP_IP_SCTP_CONNECTION_FAIL;
            break;

        case SB_SCTP_IP_CANNOT_SET_OUTPUT_STREAM:
            s = STR_SB_SCTP_IP_CANNOT_SET_OUTPUT_STREAM;
            break;

        case SB_SCTP_IP_CANNOT_EXECUTE_COMMAND:
            s = STR_SB_SCTP_IP_CANNOT_EXECUTE_COMMAND;
            break;
            
        case SB_SCTP_IP_RE_CONNECT_TIMEOUT:
            s = STR_SB_SCTP_IP_RE_CONNECT_TIMEOUT;
            break;            

        default:
            s = STR_SB_SCTP_IP_INVALID_ERROR_CODE;
            break;
    }
    
    return s;
}

/**
 * @brief configures all protocols and establishes a connection
 * 
 * @returns SB_SCTP_IP_OK or an error code
 * @see SbSctpIp::config , SbSctpIp::getConfigValues
*/
int SbSctpIp::configReq()
{
    mLog.log(LOG_TW_DBUG,"SbSctpIp::configReq() - BEGIN");
    int ret = SB_SCTP_IP_OK;

    if (mpSctpConn == NULL)
    {
        ret = getConfigValues(this->mConfigSbSctpIp);

        if (!ret)
            ret = config();
       
        // save variables to be used in send() function
        if (!ret)
            ret = SB_SCTP_IP_OK;

        // check error
        if (ret)
        {
            mLog.log(LOG_TW_ERRO, (this->getErrorMsg(ret)).c_str());
            this->cleanUp();
        } 
    }
    else
    {
        ret = SB_SCTP_IP_ALREADY_CONFIGURED;
    }
    
    mLog.log(LOG_TW_DBUG,"SbSctpIp::configReq() - END");
    return ret;
}

/**
 * @brief clears all configuration and disconnects from remote host
 * 
 * @returns SB_SCTP_IP_OK or an error code
*/
int SbSctpIp::cleanUp()
{
    mLog.log(LOG_TW_DBUG,"SbSctpIp::cleanUp() - BEGIN");

    if (mpSctpConn != NULL)
    {
        mLog.log(LOG_TW_DBUG,"SbSctpIp::cleanUp() - Stopping mpSctpConn");
        mpSctpConn->stop(); // close connection and clean up resources
        mLog.log(LOG_TW_DBUG,"SbSctpIp::cleanUp() - Stopped mpSctpConn");
        delete mpSctpConn;
        mpSctpConn = NULL;
        mLog.log(LOG_TW_DBUG,"SbSctpIp::cleanUp() - Deleted point mpSctpConn");
    }
    
    mLog.log(LOG_TW_DBUG,"SbSctpIp::cleanUp() - END");
    
    return SB_SCTP_IP_OK;
}

/**
 * @brief Called by ConfigReq to start the stack configuration
 * 
 * If the sctp layer is configured as a client, it will try connect 3 times before hang up.
 * 
 * @returns SB_SCTP_IP_OK or an error code
*/
int SbSctpIp::config()
{
    mLog.log(LOG_TW_DBUG,"SbSctpIp::config() - BEGIN");
    bool r = false;
    int ret = SB_SCTP_IP_CANNOT_CONFIGURE;

    std::string localAddr = mConfigSbSctpIp.localAddr;
    std::string remoteAddr = mConfigSbSctpIp.remoteAddr;    
    VectorString localAddrs;   // list of local addresses (only one is used)
    VectorString remoteAddrs;  // list of remote addresses (only one is used)

    localAddrs.push_back(localAddr);
    remoteAddrs.push_back(remoteAddr);

    for(unsigned int i = 0; i < mConfigSbSctpIp.optLocalAddrs.size(); i++)
    {
        localAddrs.push_back(mConfigSbSctpIp.optLocalAddrs[i]);
    }
    
    for(unsigned int i = 0; i < mConfigSbSctpIp.optRemoteAddrs.size(); i++)
    {
        remoteAddrs.push_back(mConfigSbSctpIp.optRemoteAddrs[i]);
    }     
    
    int timeout = 0;
    if(this->mConfigSbSctpIp.isServer)
    {
       	timeout = mConfigSbSctpIp.timeout;
       	mpSctpConn = new SctpServer(mConfigSbSctpIp.ipCheck);
    }
    else
    {
      	sleep(1);
      	mpSctpConn = new SctpClient();
    }

    mpSctpConn->setUpperReceive(this);
    mpSctpConn->setProtocol(mConfigSbSctpIp.protocolId);
    this->setSctpUpperSend(mpSctpConn);
    mpSctpConn->configSctp(this->mConfigSctpLayer);
    
    unsigned short numOstreamsOut;
    unsigned short maxInstreamsOut;
    
    // If it is client, try to connect 10 times before giving up.
    for(int i = 0; i < 10 ; i++) 
    {
        r = mpSctpConn->start(localAddrs, mConfigSbSctpIp.localPort, remoteAddrs,  mConfigSbSctpIp.remotePort, mConfigSbSctpIp.outStreams, mConfigSbSctpIp.inStreams, this, timeout, numOstreamsOut, maxInstreamsOut);
            if(this->mConfigSbSctpIp.isServer)
                break;
            else if(!r)
                sleep(1);
            else
                break;
    }

    // check return value
    if (r)
        ret = SB_SCTP_IP_OK;
    else
        ret = SB_SCTP_IP_SCTP_CONNECTION_FAIL;

    mLog.log(LOG_TW_DBUG,"SbSctpIp::config() - END");
    return ret;
}

/**
 * @breif Get configuration parameters from the System configuration file.
 * 
 * @returns SB_SCTP_IP_OK or an error code
*/
int SbSctpIp::getConfigValues(ConfigSbSctpIp &mConfigSbSctpIp)
{
    mLog.log(LOG_TW_DBUG,"SbSctpIp::getConfigValues() - BEGIN");
    int ret = SB_SCTP_IP_OK;
    int r = 0;
    std::string s;
  
    r = getConfig("localAddr", mConfigSbSctpIp.localAddr);
    s = "SbSctpIp::getConfigValues: localAddr -> " + mConfigSbSctpIp.localAddr;
    mLog.log(LOG_TW_DBUG,s.c_str());
    
   /* if (!r)
    {
        int numOptLocalAddrs;
        r = getConfig("numOptLocalAddrs", &numOptLocalAddrs, sizeof(numOptLocalAddrs));
        if(!r)
        {
	    s = "SbSctpIp::getConfigValues: numOptLocalAddrs -> " + Util::toString(numOptLocalAddrs);
	    mLog.log(LOG_TW_DBUG,s.c_str());            
	    std::string param;
            mConfigSbSctpIp.optLocalAddrs.clear();
            for(int i = 0; i < numOptLocalAddrs && !r; i++)
            {
                param = "optLocalAddr[" + Util::toString(i) + "]";
                std::string addr;
                r = getConfig(param.c_str(), addr);
                s = "SbSctpIp::getConfigValues: optLocalAddr " + Util::toString(i+1) + " -> " + addr;
                mLog.log(LOG_TW_DBUG,s.c_str());
                mConfigSbSctpIp.optLocalAddrs.push_back(addr);
            }
        }
	else
        {
	    r = 0; // Dont generate errors to keep compliance with older SysCfg versions.
        }		
    }    */
    if (!r)
    {
        r = getConfig("localPort", &mConfigSbSctpIp.localPort, sizeof(mConfigSbSctpIp.localPort));
    	s = "SbSctpIp::getConfigValues: localPort -> " + Util::toString(mConfigSbSctpIp.localPort);
    	mLog.log(LOG_TW_DBUG,s.c_str());        
    }
    if (!r)
    {
        r = getConfig("remoteAddr", mConfigSbSctpIp.remoteAddr);
    	s = "SbSctpIp::getConfigValues: remoteAddr -> " + mConfigSbSctpIp.remoteAddr;
    	mLog.log(LOG_TW_DBUG,s.c_str());        
    } 
    /*if (!r)
    {
        int numOptRemoteAddrs;
        r = getConfig("numOptRemoteAddrs", &numOptRemoteAddrs, sizeof(numOptRemoteAddrs));
        if(!r)
        {
            s = "SbSctpIp::getConfigValues: numOptRemoteAddrs -> " + Util::toString(numOptRemoteAddrs);
            mLog.log(LOG_TW_DBUG,s.c_str());   
	    std::string param;
            mConfigSbSctpIp.optRemoteAddrs.clear();
            for(int i = 0; i < numOptRemoteAddrs && !r; i++)
            {
                param = "optRemoteAddr[" + Util::toString(i) + "]";
                std::string addr;
                r = getConfig(param.c_str(), addr);
                s = "SbSctpIp::getConfigValues: optRemoteAddr " + Util::toString(i+1) + " -> " + addr;
                mLog.log(LOG_TW_DBUG,s.c_str());
                mConfigSbSctpIp.optRemoteAddrs.push_back(addr);
            }
        }
	else
        {
	    r = 0; // Dont generate errors to keep compliance with older SysCfg versions.
        }	
    }     */
    if (!r)
    {
        r = getConfig("remotePort", &mConfigSbSctpIp.remotePort, sizeof(mConfigSbSctpIp.remotePort));
    	s = "SbSctpIp::getConfigValues: remotePort -> " + Util::toString(mConfigSbSctpIp.remotePort);
    	mLog.log(LOG_TW_DBUG,s.c_str());        
    }          
    if (!r)
    {
        r = getConfig("outStreams", &mConfigSbSctpIp.outStreams, sizeof(mConfigSbSctpIp.outStreams));
    	s = "SbSctpIp::getConfigValues: outStreams -> " + Util::toString(mConfigSbSctpIp.outStreams);
    	mLog.log(LOG_TW_DBUG,s.c_str());        
    }         
    if (!r)
    {
        r = getConfig("inStreams", &mConfigSbSctpIp.inStreams, sizeof(mConfigSbSctpIp.inStreams));
    	s = "SbSctpIp::getConfigValues: inStreams -> " + Util::toString(mConfigSbSctpIp.inStreams);
    	mLog.log(LOG_TW_DBUG,s.c_str());        
    }        
    if (!r)
    {
        int isServer = 1;
        r = getConfig("isServer", &isServer, sizeof(isServer));
        //r = getConfig("isServer", &mConfigSbSctpIp.isServer, sizeof(mConfigSbSctpIp.isSever));
        if(isServer == 0)
        {
        	mConfigSbSctpIp.isServer = false;
        	s = "SbSctpIp::getConfigValues: isServer -> false";
    		mLog.log(LOG_TW_DBUG,s.c_str());
					mConfigSbSctpIp.ipCheck = false;			
        } 
        else
        {
        	mConfigSbSctpIp.isServer = true;
        	s = "SbSctpIp::getConfigValues: isServer -> true";
    		mLog.log(LOG_TW_DBUG,s.c_str());
			mConfigSbSctpIp.ipCheck = true;
			/*if (!r)
			{
				int ipCheck = 1;
				r = getConfig("anyRemoteAddr", &ipCheck, sizeof(ipCheck));
				if(!r)
				{
					if(ipCheck == 1)
					{
						mConfigSbSctpIp.ipCheck = false;
					}
					else
					{
						mConfigSbSctpIp.ipCheck = true;
					}
				}
				else
				{
					mConfigSbSctpIp.ipCheck = true;
					r = 0;
				}
		    	s = "SbSctpIp::getConfigValues: anyRemoteAddr -> " + Util::toString(mConfigSbSctpIp.ipCheck);
		    	mLog.log(LOG_TW_DBUG,s.c_str()); 
			}*/
        }        	
    }
    if (!r)
    {
        r = getConfig("timeout", &mConfigSbSctpIp.timeout, sizeof(mConfigSbSctpIp.timeout));
        if(mConfigSbSctpIp.timeout == 0)
        {
            mConfigSbSctpIp.timeout = SCTP_SERVER_DEFAULT_TIMEOUT;
        }
        s = "SbSctpIp::getConfigValues: timeout -> " + Util::toString(mConfigSbSctpIp.timeout);
        mLog.log(LOG_TW_DBUG,s.c_str());
    }
    if (!r)
    {
        r = getConfig("protocolId", &mConfigSbSctpIp.protocolId, sizeof(mConfigSbSctpIp.protocolId));
    	s = "SbSctpIp::getConfigValues: protocolId -> " + Util::toString(mConfigSbSctpIp.protocolId);
    	mLog.log(LOG_TW_DBUG,s.c_str());        
    } 
    
    if (!r)
    {
        r = getConfig("associationMaxRetrans", &mConfigSctpLayer.associationMaxRetrans, sizeof(mConfigSctpLayer.associationMaxRetrans));
    	s = "SbSctpIp::getConfigValues: associationMaxRetrans -> " + Util::toString(mConfigSctpLayer.associationMaxRetrans);
    	mLog.log(LOG_TW_DBUG,s.c_str());        
    } 
    
    if (!r)
    {
        r = getConfig("pathMaxRetrans", &mConfigSctpLayer.pathMaxRetrans, sizeof(mConfigSctpLayer.pathMaxRetrans));
    	s = "SbSctpIp::getConfigValues: pathMaxRetrans -> " + Util::toString(mConfigSctpLayer.pathMaxRetrans);
    	mLog.log(LOG_TW_DBUG,s.c_str());        
    } 
    if (!r)
    {
        r = getConfig("maxInitRetransmits", &mConfigSctpLayer.maxInitRetransmits, sizeof(mConfigSctpLayer.maxInitRetransmits));
    	s = "SbSctpIp::getConfigValues: maxInitRetransmits -> " + Util::toString(mConfigSctpLayer.maxInitRetransmits);
    	mLog.log(LOG_TW_DBUG,s.c_str());        
    } 
    if (!r)
    {
        r = getConfig("hbInterval", &mConfigSctpLayer.hbInterval, sizeof(mConfigSctpLayer.hbInterval));
    	s = "SbSctpIp::getConfigValues: hbInterval -> " + Util::toString(mConfigSctpLayer.hbInterval);
    	mLog.log(LOG_TW_DBUG,s.c_str());        
    } 
    if (!r)
    {
        r = getConfig("validCookieLife", &mConfigSctpLayer.validCookieLife, sizeof(mConfigSctpLayer.validCookieLife));
    	s = "SbSctpIp::getConfigValues: validCookieLife -> " + Util::toString(mConfigSctpLayer.validCookieLife);
    	mLog.log(LOG_TW_DBUG,s.c_str());        
    } 
    if (!r)
    {
        r = getConfig("cookiePreserveEnable", &mConfigSctpLayer.cookiePreserveEnable, sizeof(mConfigSctpLayer.cookiePreserveEnable));
    	s = "SbSctpIp::getConfigValues: cookiePreserveEnable -> " + Util::toString(mConfigSctpLayer.cookiePreserveEnable);
    	mLog.log(LOG_TW_DBUG,s.c_str());        
    } 
    if (!r)
    {
        r = getConfig("maxBurst", &mConfigSctpLayer.maxBurst, sizeof(mConfigSctpLayer.maxBurst));
    	s = "SbSctpIp::getConfigValues: maxBurst -> " + Util::toString(mConfigSctpLayer.maxBurst);
    	mLog.log(LOG_TW_DBUG,s.c_str());        
    } 
    if (!r)
    {
        r = getConfig("rtoInitial", &mConfigSctpLayer.rtoInitial, sizeof(mConfigSctpLayer.rtoInitial));
    	s = "SbSctpIp::getConfigValues: rtoInitial -> " + Util::toString(mConfigSctpLayer.rtoInitial);
    	mLog.log(LOG_TW_DBUG,s.c_str());        
    } 
    if (!r)
    {
        r = getConfig("rtoMin", &mConfigSctpLayer.rtoMin, sizeof(mConfigSctpLayer.rtoMin));
    	s = "SbSctpIp::getConfigValues: rtoMin -> " + Util::toString(mConfigSctpLayer.rtoMin);
    	mLog.log(LOG_TW_DBUG,s.c_str());        
    }
    if (!r)
    {
        r = getConfig("rtoMax", &mConfigSctpLayer.rtoMax, sizeof(mConfigSctpLayer.rtoMax));
    	s = "SbSctpIp::getConfigValues: rtoMax -> " + Util::toString(mConfigSctpLayer.rtoMax);
    	mLog.log(LOG_TW_DBUG,s.c_str());        
    } 
    if (!r)
    {
        r = getConfig("rtoAlphaExpDivisor", &mConfigSctpLayer.rtoAlphaExpDivisor, sizeof(mConfigSctpLayer.rtoAlphaExpDivisor));
    	s = "SbSctpIp::getConfigValues: rtoAlphaExpDivisor -> " + Util::toString(mConfigSctpLayer.rtoAlphaExpDivisor);
    	mLog.log(LOG_TW_DBUG,s.c_str());        
    } 
    if (!r)
    {
        r = getConfig("rtoBetaExpDivisor", &mConfigSctpLayer.rtoBetaExpDivisor, sizeof(mConfigSctpLayer.rtoBetaExpDivisor));
    	s = "SbSctpIp::getConfigValues: rtoBetaExpDivisor -> " + Util::toString(mConfigSctpLayer.rtoBetaExpDivisor);
    	mLog.log(LOG_TW_DBUG,s.c_str());        
    } 
    if (!r)
    {
        r = getConfig("addipEnable", &mConfigSctpLayer.addipEnable, sizeof(mConfigSctpLayer.addipEnable));
    	s = "SbSctpIp::getConfigValues: addipEnable -> " + Util::toString(mConfigSctpLayer.addipEnable);
    	mLog.log(LOG_TW_DBUG,s.c_str());        
    }
    
    if (!r)
        ret = SB_SCTP_IP_OK;
    else
        ret = SB_SCTP_IP_CANNOT_GET_CONFIG_VALUES;
    
    mLog.log(LOG_TW_DBUG,"SbSctpIp::getConfigValues() - END");
    return ret;
}
    
/**
 * @brief Parses a generic command.
 *
 * Splits a generic comand into COMMAND + MSG SIZE + MSG
 *
 * @param buffer    Buffer to be parsed.
 * @param command   Generic command.
 * @param msgSize   Message size.
 * @param msg       Generic command parameter (message).
 */
void SbSctpIp::parseCommand(const char *buffer, int &command, int &msgSize, char *&msg)
{
    command = (int)*buffer;

    buffer += sizeof(int);
    msgSize = (int)*buffer;

    if (msgSize)
    {
        buffer += sizeof(int);
        msg = (char *)buffer;
    }
    else
    {
        msg = NULL;
    }
}
 

/**
 * @brief Sets the sctp payload control  value.
 *
 * @param sctp payload control  value to be set.
 *
 * @return Error code.
 */
int SbSctpIp::setSctpLoadControl(int c)
{
    unsigned short os = c;
    int ret = SB_SCTP_IP_OK;
        
    mLog.log(LOG_TW_INFO, "SbSctpIp::setSctpLoadControl - try to change control to %d", c);

    mpSctpConn->setPayloadHandling(c);
    
    mLog.log(LOG_TW_INFO, "SbSctpIp::setSctpLoadControl - control changed to %d", c);


    return ret;
}

/**
 * @brief Sets the output stream value.
 *
 * @param stream Output stream to be set.
 *
 * @return Error code.
 */
int SbSctpIp::setOutputStream(int stream)
{
    unsigned short os = stream;
    int ret = SB_SCTP_IP_CANNOT_SET_OUTPUT_STREAM;
        
    mLog.log(LOG_TW_INFO,"SbSctpIp::setOutputStream - try to change stream to %d", stream);

    if ( os < mConfigSbSctpIp.outStreams )
    {
        mOutStream = os;
        ret = SB_SCTP_IP_OK;
        
        mLog.log(LOG_TW_INFO,"SbSctpIp::setOutputStream - stream changed to %d", mOutStream);
    }

    return ret;
}

/**
 * @brief Called by the sctp layer to deliver data.
 * 
 * The stack builder will foward data to the java part through JsiTwConnection.
 * 
*/
void SbSctpIp::receive(char *buffer, int size, unsigned int stream)
{
    mLog.log(LOG_TW_DBUG,"SbSctpIp::receive() - BEGIN");

    if (stream != mOutStream)
    {
        mLog.log(LOG_TW_WARN,"SbSctpIp::receive() - Received stream (%d) is different from expected stream (%d)", stream, mOutStream);
    }
    
    dataInd((char *)buffer, size);  // receiving data
    
    mLog.log(LOG_TW_DBUG,"SbSctpIp::receive() - END");
}
bool SbSctpIp::getIpPortFromExtCmd(char *bufext, int len, std::string &ip, unsigned short &port)
{
	int point = 0;
	int iplen = 0;
	if(bufext == NULL || len <= 0)
	{
		return false;
	}

	while(len > point)
	{
		switch(bufext[point])
		{
		case DATAINDEXT_FLAG_IP:
			point++;

			iplen = bufext[point];
			point+=sizeof(char);

			ip = &bufext[point];
			point+=iplen;
			break;
		case DATAINDEXT_FLAG_PORT:
			point++;
			point+=sizeof(char);

			memcpy(&port, &bufext[point], sizeof(short));
			port = ntohs(port);
			point+=sizeof(short);
			break;
		}
	}

	return true;
}
int SbSctpIp::send(char *buf, int size, char *bufext, int len)
{
	std::string ip;
	unsigned short port;
    int r = 0;
    int ret = SB_SCTP_IP_OK;
	
    unsigned short stream = mOutStream;
	
	if(!getIpPortFromExtCmd(bufext, len, ip, port))
	{
		ret = SB_SCTP_IP_CANNOT_SEND_DATA;
	}
	else
	{
		
		if(mpSctpUpperSend != NULL)
        {
			if(! mpSctpUpperSend->send(buf, size, stream ,true, (char *)ip.c_str(), port))
            {
				ret = SB_SCTP_IP_CANNOT_SEND_DATA;
            }
        }
		else
        {
			ret = SB_SCTP_IP_CANNOT_SEND_DATA;
        }
                
	}

	return ret;
}

void SbSctpIp::receiveEx(char *bufferEx, int size)
{
	dataIndExt((char *)bufferEx, size);
}

/**
 * @brief Creates a SbSctpIp object and start the thread
*/
int main(int argc, char* argv[])
{
    try
    {
    	std::auto_ptr<SbSctpIp> con (new SbSctpIp(1, argv[1]));
    	//SbSctpIp *con = new SbSctpIp(1, argv[1]);
        con->run();
    }
    catch(...)
    {
    }

    return 0;
}

