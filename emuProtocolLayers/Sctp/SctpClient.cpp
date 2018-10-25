#include "SctpClient.h"

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

SctpClient::SctpClient() {
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

SctpClient::~SctpClient() {
    this->stop();
}

/**
@brief Starts a SCTP client connection.

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
bool SctpClient::start(const VectorString &localAddrs, unsigned short localPort,
        const VectorString &remoteAddrs, unsigned short remotePort,
        unsigned short numOstreams, unsigned short maxInstreams,
        SctpUpperReceive *pCallback, int timeout,
        unsigned short &numOstreamsOut, unsigned short &maxInstreamsOut) {
    std::string s;
    bool bRet = true;

    s = "SctpClient::Start(localAddrs: " + this->getAddrs(localAddrs) + "  localPort: " + Util::toString(localPort) +
            "  remoteAddrs: " + this->getAddrs(remoteAddrs) + "  remotePort: " + Util::toString(remotePort) +
            "  numOstreams: " + Util::toString(numOstreams) + "  maxInstreams: " + Util::toString(maxInstreams) +
            "  pCallback: " + Util::toHexString((intptr_t) pCallback) + "  timeout: " + Util::toString(timeout) + ") - BEGIN";
    //LOG_DBUG_JSI(s.c_str());

    //LOG_DBUG_JSI("Calling Sctp::Create()");
    bRet = mSctpCnx.create();

    if (bRet) {
        //LOG_DBUG_JSI("Calling Sctp::SetNotifications()");
        bRet = mSctpCnx.setNotifications(true);
    }

    if (bRet) {
        //LOG_DBUG_JSI("Calling Sctp::Init()");
        bRet = mSctpCnx.init(numOstreams, maxInstreams);
    }

    if (bRet) {
        //LOG_DBUG_JSI("Calling Sctp::Bind()");
        bRet = mSctpCnx.bind(localAddrs, localPort);
    }

    if (bRet) {
        //LOG_DBUG_JSI("Calling SctpClient::Connect()");
        bRet = this->connect(remoteAddrs, remotePort);
    }

    if (bRet) {
        //LOG_DBUG_JSI("Calling Sctp::GetStatus()");
        mSctpCnx.getStatus(maxInstreamsOut, numOstreamsOut);
        s = "Returning Sctp::GetStatus(maxInstreamsOut: " + Util::toString(maxInstreamsOut) +
                "  numOstreamsOut: " + Util::toString(numOstreamsOut) + ")";
        //LOG_DBUG_JSI(s.c_str());
    }

    if (bRet) {
        //LOG_DBUG_JSI("Calling SctpCnx::StartThread()");
        bRet = this->startThread(pCallback);
    }

    // verify error
    if (!bRet) {
        s = this->getLastErrorMsg();
        //LOG_ERRO_JSI(s.c_str());

        this->stopThread();
        mSctpCnx.close(); // close socket
    }

    //LOG_DBUG_JSI("SctpClient::Start() - END");

    return bRet;
}

/**
@brief Stops the SCTP client connection.
 */
void SctpClient::stop() {
    mLog.log(LOG_TW_DBUG, "SctpClient::Stop() - BEGIN");

    // wait callbackThread() termination
    mLog.log(LOG_TW_DBUG, "SctpClient::Stop() - Stopping Thread");
    this->stopThread();

    // close socket
    mLog.log(LOG_TW_DBUG, "SctpClient::Stop() - Closing Socket");
    mSctpCnx.close();

    mLog.log(LOG_TW_DBUG, "SctpClient::Stop() - END");
}

/**
@brief Connects to the Sctp peer server.

@param remoteAddrs Server address.
@param remotePort Server port to connect.
@returns true if it was able to connect to the server.
 */
bool SctpClient::connect(const VectorString &remoteAddrs, unsigned short remotePort) {
    //LOG_DBUG_JSI("SctpClient::Connect() - BEGIN");

    std::string s;
    bool bRet;

    if (remoteAddrs.size() > 0) {
        //bRet = mSctpCnx.connect(remoteAddrs, remotePort);
        errno = 0;
        mLog.log(LOG_TW_DBUG, "SctpClient::connect() remotePort = %u", remotePort);
        for(int i = 0; i < remoteAddrs.size(); i++)
        {
            mLog.log(LOG_TW_DBUG, "SctpClient::connect() remoteAddrs[%d] = %s", i, remoteAddrs[i].c_str());
        }
        bRet = mSctpCnx.connectx(remoteAddrs, remotePort);
        if (!bRet) {
            if (errno == ECONNREFUSED)
                mLog.log(LOG_TW_ERRO, "SctpClient::connect() connection refused.");
            else
                mLog.log(LOG_TW_ERRO, "SctpClient::connect() error connecting to addrs: %s.\n", strerror(errno));
        }
    } else {
        errno = EINVAL; // Invalid argument
        bRet = false;

    }
    //LOG_DBUG_JSI("SctpClient::Connect() - END");

    return bRet;
}
