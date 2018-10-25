#ifndef _SctpServer_h
#define _SctpServer_h

#include "SctpCnx.h"

#define SCTP_TEMP_BUF_LEN 256
#define DATAINDEXT_FLAG_IP  1
#define DATAINDEXT_FLAG_PORT 2
#define DATAINDEXT_DISCONNECT 3
/**
@brief SctpClient implements some SctpCnx methods in order to create Sctp Server connections.

@author Franco Sinhori <franco.sinhori.ext\@siemens.com> , Alexandre Godoi <alexandre.godoi\@siemens.com>
*/
class SctpServer : public SctpCnx
{
public:
    SctpServer();
    SctpServer(bool ipCheck);
    ~SctpServer();

    bool start(const VectorString &localAddrs, unsigned short localPort, 
               const VectorString &remoteAddrs, unsigned short remotePort,
               unsigned short numOstreams, unsigned short maxInstreams,
               SctpUpperReceive *pCallback, int timeout,
               unsigned short &numOstreamsOut, unsigned short &maxInstreamsOut);
    void stop();
    void stopServer();
    
private:
    Sctp mSctpServer;       ///< Sctp object to open Sctp connections.
    virtual void callbackThread();
    void callbackSingleClientThread();
    int receiveDataIndExOfIpPort(char *ip, unsigned short port);
    int receiveDataIndExOfDisconnect();
    bool send(const void *msg, unsigned int len, unsigned short stream, bool ordered, char *ip, unsigned short &port);
    bool accept();
    bool accept(const VectorString &remoteAddrs, unsigned short remotePort, int timeout);
    bool isValidAddr(const VectorString &remoteAddrs, unsigned short remotePort, Sctp &sctpHost);
    bool _stopSctpServer;
    
    VectorString _localAddrs;
    unsigned short _localPort; 
    VectorString _remoteAddrs; 
    unsigned short _remotePort;
    unsigned short _numOstreams;
    unsigned short _maxInstreams;
    int _timeout;
    unsigned short _numOstreamsOut;
    unsigned short _maxInstreamsOut;    
    bool mIpCheck;
};

#endif // ifndef _SctpServer_h


