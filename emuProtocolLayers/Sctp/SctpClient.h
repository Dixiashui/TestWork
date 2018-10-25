#ifndef _SctpClient_h
#define _SctpClient_h

#include "SctpCnx.h"
#include "common/Util.h"

/**
@brief SctpClient implements some SctpCnx methods in order to create Sctp Client connections.

@author Franco Sinhori <franco.sinhori.ext\@siemens.com> , Alexandre Godoi <alexandre.godoi\@siemens.com>
*/
class SctpClient : public SctpCnx
{
public:
    SctpClient();
    ~SctpClient();

    bool start(const VectorString &localAddrs, unsigned short localPort,
               const VectorString &remoteAddrs, unsigned short remotePort,
               unsigned short numOstreams, unsigned short maxInstreams,
               SctpUpperReceive *pCallback, int timeout,
               unsigned short &numOstreamsOut, unsigned short &maxInstreamsOut);
    void stop();

private:
    bool connect(const VectorString &remoteAddrs, unsigned short remotePort);

};

#endif // ifndef _SctpClient_h

