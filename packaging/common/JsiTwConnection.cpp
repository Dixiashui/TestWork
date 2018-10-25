#include "common/JsiTwConnection.h"
#include "common/MutexGuard.h"

JsiTwConnection* JsiTwConnection::mCon = NULL;

/**
 * Creates an JsiTwConnection to connect to Pegasus connection listening on given port
 * 
 * @param port TCP port where Pegasus is listening to
 */
JsiTwConnection::JsiTwConnection(char* port)
{
    //mId = id;
    mIsConfigured = false;
    mPort = port;
    JsiTwConnection::mCon = this;
	logEnabled = false;
	mDataIndExtLen = 0;

}

/**
 * @brief JsiTwConnection destructor.
 */
JsiTwConnection::~JsiTwConnection()
{
}

/**
 * @brief Deliver data arrived from network to Pegasus
 * 
 * @param buf address of the arrived data
 * @param size size of buffer to be delivered
 */
void JsiTwConnection::dataInd(char *buf, int size)
{
    sendMessage(DATAIND, buf, size);
}
    
/**
 * @brief Delivers data to be processed for lower layer.
 *
 * Any derived class that intends a generic command execution must overwrite 
 * this method to parse the data buffer and execute a command.
 * 
 * @param buf   Data to be processed.
 */
int JsiTwConnection::execCommand(const char *buf)
{
    return 0;
}

/**
 * @brief Asks Pegasus for the value for an integer configuration parameter
 *
 * Asks Pegasus for the value for an integer configuration parameter. 
 * Pegasus is responsible for determining the correct value (mostly this value is readed 
 * from System Configuration) and return it.
 *
 * @param paramName name of the parameter 
 * @param ret address where the value should be stored
 * @param sizeOfRet size of the return buffer
 * @return 0 if the value was successfully retrieved, -1 if some error happened.
 */
int JsiTwConnection::getConfig(const char* paramName, void *ret, int sizeOfRet)
{
    int x;
    char buffer[5];
    int actId, size;
    int status;

    // request the data
    sendMessage(GETPARAM, (void*) paramName, strlen(paramName));

    // Read the next PDU, which contain the response

    // read the action
    mSocket.readn((char*)&actId, 4);
    
    // read the size
    mSocket.readn((char*)&size, 4);

    // read the data
    mSocket.readn(buffer, 5);

    status = buffer[0];
    memcpy(&x, buffer+1, sizeof(int));

    // was the received message ok?
    if ( (actId != GETPARAMRESP) || size != 5 )
    {
        return -1;
    }
    
    // error getting the paramter?
    if (status != 0)
    {
        return -1;
    }
     
    // ok, we got the value. Lets cast it in the return buffer
    switch(sizeOfRet)
    {
        case 1:
            (* (int8_t*)ret)  = 0x000000FF & x;
            break;

        case 2:
            (* (int16_t*)ret) = 0x0000FFFF & x;
            break;

        case 4:
            (* (int32_t*)ret) = 0xFFFFFFFF & x;
            break;
            
        case 8:
            (* (int64_t*)ret) = 0xFFFFFFFFFFFFFFFF & x;
            break;

        default:
            // Invalid size!
            this->mTwLog.log(LOG_TW_ERRO, "JsiTwConnection::getConfig()  Invalid sizeOfRet = %d", sizeOfRet);
            return -1;
    }

    return 0;
}

/**
 * @brief Asks Pegasus for the value for a string configuration parameter
 *
 * Asks Pegasus for the value for a string configuration parameter. 
 * Pegasus is responsible for determining the correct value (mostly this value is readed 
 * from System Configuration) and return it.
 *
 * @param paramName name of the parameter 
 * @param s Reference of string object that receives the value.
 * @return 0 if the value was successfully retrieved, -1 if some error happened.
 */
int JsiTwConnection::getConfig(const char* paramName, std::string &s)
{
    char *buffer = NULL;
    int actId;
    int size;
    char status;

    // request the data
    sendMessage(GETPARAMSTRING, (void *)paramName, strlen(paramName));

    // Read the next PDU, which contain the response

    // read the action
    mSocket.readn((char*)&actId, 4);
    
    // read the size
    mSocket.readn((char*)&size, 4);

    // alloc buffer
    buffer = new char[size+1];

    // read the status and data
    mSocket.readn(&status, 1);
    mSocket.readn(buffer, size);

    // add '\0' to finish C string
    buffer[size] = '\0';

    // get string parameter
    s =  buffer;

    // buffer is not needed anymore
    delete [] buffer;

    // was the received message ok?
    if (actId != GETPARAMRESP) 
    {
        return -1;
    }
    
    // error getting the paramter?
    if (status != 0)
    {
        return -1;
    }
     
    return 0;
}

/**
 * @brief Sends a JSI message for pegasus.
 *
 * All comunication with pegasus must be done through this function to avoid threading problems 
 * (i.e., two thread sending messages for pegasus at the same time).
 *
 * @param actId JSI action identification 
 * @param buffer data to be sent
 * @param size size of the buffer
 */
void JsiTwConnection::sendMessage (int actId, void *buffer, int size)
{
    mMutexSend.lock();
   
    // send it 
    //mSocket.send(&actId, sizeof(actId));
    mSocket.writen(&actId, sizeof(actId));
    
    //mSocket.send(&size, sizeof(size));
    mSocket.writen(&size, sizeof(size));
    
    //mSocket.send(buffer, size);
    mSocket.writen(buffer, size);
    
    //fsync(sockfd);
    fsync(mSocket.getSocket());

    // done
    mMutexSend.unlock();
}

void JsiTwConnection::dataIndExt(char *buf, int size)
{
	sendMessage(DATAINDEXT, buf, size);
}
/**
 * @brief Process a JSI command received from Pegasus
 *
 * Based on the received JSI action id, it calls the function responsible for
 * handling this action, gets the function return and send it back to Pegasus
 *
 * For example, if the JSI message has action id SEND, it calls 
 * JsiTwConnection->send function using the received buffer as parameter
 * and sends the return of the send function back to Pegasus with action id SENDRESP
 * 
 * @param actId JSI action identification 
 * @param buffer data recevied from Pegasus as parameters for action
 * @param size size of the buffer
 */
void JsiTwConnection::processMsg(int actId, char *buffer, int size)
{
    int x = 0;
    int error = 0;
    std::string s;
  
    switch(actId)
    {
        case EXECCOMMAND:
            x = execCommand(buffer);
            sendMessage(EXECCOMMANDRESP, &x, sizeof(x));
            break;

        case CONFIGREQ:
            x = configReq();
            sendMessage(CONFIGREQRESP, &x, sizeof(x));
            break;
            
		case DATAINDEXT:
			memset(mDataIndExtBuf, 0, sizeof(mDataIndExtBuf));
			memcpy(mDataIndExtBuf, buffer, size);
			mDataIndExtLen= size;

			break;
        case SEND:
            s = "JsiTwConnection::processMsg - SEND (buffer: " + Util::toHexString((unsigned char)buffer[0]) + 
                    " " + Util::toHexString((unsigned char)buffer[1]) + 
                    "..." + Util::toHexString((unsigned char)buffer[16]) + 
                    " " + Util::toHexString((unsigned char)buffer[17]) + ", size: " + Util::toString(size) + ")."; 
            mTwLog.log(LOG_TW_DBUG, s.c_str());
			if(mDataIndExtLen > 0)
			{			
				x = send(buffer, size, mDataIndExtBuf, mDataIndExtLen);
				mDataIndExtLen = 0;
				memset(mDataIndExtBuf, 0, sizeof(mDataIndExtBuf));
			}
			else
			{
	            x = send(buffer, size);
			}
            sendMessage(SENDRESP, &x, sizeof(x));
            break;

        case CLEANUP:
            s = "JsiTwConnection::processMsg - CLEANUP cmd arrived.";
            mTwLog.log(LOG_TW_DBUG, s.c_str());
            x = cleanUp();
            sendMessage(CLEANUPRESP, &x, sizeof(x));
            break;

        // We have requested a configuration and now the response
        // arrived. The first byte is the status (0 for sucess, -1 
        // otherwise) and the next four bytes are the value 
        case GETPARAMRESP:
            // this message should not be received outside of getConfig
            break;

        case GETERRORSTRING:
            memcpy(&error, buffer, sizeof(int));
            s = getErrorMsg(error);
            sendMessage(GETERRORSTRINGRESP, (void *)s.c_str(), s.size());
            break;

        case SETLOGLEVEL:
            if(size != sizeof(mTwLog.mLevel))
                mTwLog.log(LOG_TW_ERRO, "JsiTwConnection::processMsg Bad SETLOGLEVEL message with size %d\n", size);
            else
            {
                MutexGuard guard(mTwLog.mLevelMutex);
                memcpy(&mTwLog.mLevel, buffer, size);
                mTwLog.log(LOG_TW_DBUG, "JsiTwConnection::processMsg SETLOGLEVEL %d\n", mTwLog.mLevel);
            }
            break;
    }
}

/** 
 * @brief Thread responsible for receiving the commands from Pegasus via socket and processing it
 *
 * Each JSI message has the following format:
 *
 * <table border>
 * <tr><td>ActionID</td><td>Size</td><td>Buffer</td></tr>
 * <tr><td><i>(4 bytes)</td><td><i>(4 bytes)</td><td>(Size bytes)</td></tr>
 * </table>
 *
 * After receiving the message, processMsg is called to process it.
 *
 * @see processMsg
 */
void JsiTwConnection::receiveMessages()
{
    int actId;
    int size;
    char buffer[DATA_BUF_LEN];
    
    while (1)
    {

        // read the action
        if (mSocket.readn((char*)&actId, 4) <= 0)
        {
            throw "Disconnected!";
        }
        
        // read the data size
        if (mSocket.readn((char*)&size, 4) <= 0)
        {
            throw "Disconnected!";
        }
        
        // read the size
        if (size > 0)
        {
            // read data
            if (mSocket.readn(buffer, size) <= 0)
            {
                throw "Disconnected!";
            }
        }
        else
        {
            return;
        }
        
        // process it 
        processMsg(actId, buffer, size); 
    }
   
}

/**
 * @brief Connects to Pegasus and start receiving and processing messages
 */
void JsiTwConnection::run()
{
    // ccreate socket
    if (!mSocket.create(SOCK_STREAM, IPPROTO_TCP))
    {
        return;
    }

    // connect to server
    if (!mSocket.connect("127.0.0.1", atoi(mPort)))
    {
        return;
    }

    // enable TCP NODELAY
    if (!mSocket.setTcpNoDelay(true))
    {
        return;
    }
    
    // init user specific stuff
    init();

    logEnabled = true;

    // read and process the messages
    try{
	    receiveMessages();
    }catch(std::string &e){
	logWarn("FATAL ERROR: Closing connection due to error: "+e);
    }catch(...){
	logWarn("FATAL ERROR: Closing connection due to error");
    }

}

/**
 * @brief virtual method intended to be overriden. Called after connecting to Pegasus and before start receiving and processing messages
 */
void JsiTwConnection::init()
{
}
    
////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////
void JsiTwConnection::logDbug(const std::string &s)
{
    mTwLog.log(LOG_TW_DBUG, "%s", s.c_str());
}

////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////
void JsiTwConnection::logTrac(const std::string &s)
{
    mTwLog.log(LOG_TW_TRAC, "%s", s.c_str());
}

////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////
void JsiTwConnection::logInfo(const std::string &s)
{
    mTwLog.log(LOG_TW_INFO, "%s", s.c_str());
}

////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////
void JsiTwConnection::logWarn(const std::string &s)
{
    mTwLog.log(LOG_TW_WARN, "%s", s.c_str());
}

////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////
void JsiTwConnection::logErro(int err)
{
}

/**
 * Sends the given log message to Pegasus
 * @param msg string with log message
 * @param severity severity of the message
 */
void JsiTwConnection::logMsg(int severity, const char *msg)
{
    JsiTwConnection *con = JsiTwConnection::getInstance();
   
    char *msgbuf = (char*) malloc ( strlen(msg) * sizeof(char) + 2 );
    
    // first byte -> severity
    msgbuf[0] = (char) severity;
    
    // message
    strcpy (&msgbuf[1], msg);
    
    con->sendMessage(LOGMSG, msgbuf, strlen(msg)+1 );
    
    free(msgbuf); 
}

std::string JsiTwConnection::getStackName()
{
    std::string name = typeid(*this).name();
    std::string::size_type pos = name.find("Sb");
    if(pos == std::string::npos)
    {
        return name;
    }
    return std::string(typeid(*this).name()).substr(pos);
}

