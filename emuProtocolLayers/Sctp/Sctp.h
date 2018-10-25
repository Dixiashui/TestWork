#ifndef _Sctp_h
#define _Sctp_h

#include <stdlib.h>
#include "common/TwLog.h"
#include "common/LogToFile.h"
#include "../../common/Socket6.h"
#include <sstream>
#include <fstream>


using namespace std;
//#include <iostream>

//#include <linux/unistd.h>
//#include <linux/types.h>
//#include <linux/sysctl.h>
enum multiHomingType{
    STANDARD_MULTI,
    NSN_PRIVATE_MULTI,
};

struct ConfigSctpLayer
{    
    int associationMaxRetrans;
    int pathMaxRetrans;
    int maxInitRetransmits;
//    long hbInterval;
//    long validCookieLife;
//    long cookiePreserveEnable;
    int hbInterval;
    int validCookieLife;
    int cookiePreserveEnable;
    int maxBurst;
//    long rtoInitial;
//    long rtoMin;
//    long rtoMax;
    int rtoInitial;
    int rtoMin;
    int rtoMax;
    int rtoAlphaExpDivisor;
    int rtoBetaExpDivisor;
    int addipEnable;
    
    multiHomingType mhType;
};

/**
@brief Sctp wrapper methods for Socket6 class using Sctp protocol.

@author Alexandre Godoi <alexandre.godoi\@siemens.com>
*/
class Sctp
{
public:
    Sctp();
    ~Sctp();

    Socket6& getSctpSocket();
    int getRawSocket();

    bool create();
    void close();

    bool attach(int sock);
    int detach();

    bool setNonBlockMode();
    bool setNotifications(bool dataIoEvt=true , bool associationEvt=false, bool addressEvt=false, 
                          bool sendFailureEvt=false, bool peerErrorEvt=false, bool shutdownEvt=false, 
                          bool  partialDeliveryEvt=false, bool adaptionLayerEvt=false);
    bool init(unsigned short numOstreams, unsigned short maxInstreams, unsigned short maxAttempts=8, unsigned short maxInitTimeo=3000);

    int configSctp(ConfigSctpLayer param);
    
    bool bind(const VectorString &vetAddrs, unsigned short port);
    bool listen(int backlog=5);
    bool accept(Sctp &sctp); 
    //bool connect(const VectorString &addrs, unsigned short port);
    bool connect(const char *addr, unsigned short port);
    bool connectx(const VectorString &vetAddrs, unsigned short port);

    bool getLocalAddrs(VectorString &vetAddrs, unsigned short &port);
    bool getPeerAddrs(VectorString &vetAddrs, unsigned short &port);

    bool getStatus(unsigned short &instrms, unsigned short &outstrms);

    int sendMsg(const void *msg, unsigned int len, unsigned int ppid, unsigned int flags,
                unsigned short stream, unsigned int timetolive, unsigned int context,
                const char *addr, unsigned short port);
    int recvMsg(void *msg, unsigned int len, struct sctp_sndrcvinfo &sinfo, int &msg_flags,
                std::string &addr, unsigned short &port);
    int recvMsg(void *msg, unsigned int len, unsigned int &stream, unsigned int &ppid,
                unsigned int &context, unsigned int &flags, int &msg_flags, 
                std::string &addr, unsigned short &port);
    bool addrsCmp(std::string addr1, std::string addr2);
    
    bool getSockname(std::string &addr, unsigned short &port);

    static int getLastError();
    static std::string getLastErrorMsg();
   
//    enum sctpCtlParam{
//         NET_SCTP_RTO_INITIAL = 1,
//         NET_SCTP_RTO_MIN     = 2,
//         NET_SCTP_RTO_MAX     = 3,
//         NET_SCTP_RTO_ALPHA   = 4,
//         NET_SCTP_RTO_BETA    = 5,
//         NET_SCTP_VALID_COOKIE_LIFE       =  6,
//         NET_SCTP_ASSOCIATION_MAX_RETRANS =  7,
//         NET_SCTP_PATH_MAX_RETRANS        =  8,
//         NET_SCTP_MAX_INIT_RETRANSMITS    =  9,
//         NET_SCTP_HB_INTERVAL             = 10,
//         NET_SCTP_PRESERVE_ENABLE         = 11,
//         NET_SCTP_MAX_BURST               = 12,
//         NET_SCTP_ADDIP_ENABLE            = 13,
//         NET_SCTP_PRSCTP_ENABLE           = 14,
//    };

private:
    Socket6 mSocket;        ///< Socket6 object to open Sctp connections.
    TwLog mLog;
};

#endif // ifndef _Sctp_h

