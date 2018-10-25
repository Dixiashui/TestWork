#ifndef _SctpCnx_h
#define _SctpCnx_h

#include <pthread.h>
#include <sys/poll.h>
#include "Sctp.h"
#include "SctpUpperSend.h"
#include "SctpUpperReceive.h"
#include "common/Util.h"
#include "common/TwLog.h"

/*  1 - IUA                                                 [RFC4233]
  2 - M2UA                                                [RFC3331]
  3 - M3UA                                                [RFC4666]
  4 - SUA                                                 [RFC2960]
  5 - M2PA                                                [RFC2960]
  6 - V5UA                                                [RFC2960]
  7 - H.248                                                 [H.248]
  8 - BICC/Q.2150.3                             [Q.1902.1/Q.2150.3] 
  9 - TALI                                                [RFC3094]
 10 - DUA                                                 [RFC4129]
 11 - ASAP       <draft-ietf-rserpool-asap-03.txt>            [Ong]             
 12 - ENRP       <draft-ietf-rserpool-enrp-03.txt>            [Ong] 
 13 - H.323                                                 [H.323]
 14 - Q.IPC/Q.2150.3                            [Q.2631.1/Q.2150.3]
 15 - SIMCO      <draft-kiesel-midcom-simco-sctp-00.txt>   [Kiesel]*/
#define MAX_SCTP_FRAME         4096
#define IUA_PROTOCOL           1
#define M3UA_PROTOCOL          3
#define H248_PROTOCOL          7
#define M2UA_PROTOCOL          2
#define UNSPECIFIED_PROTOCOL   0
#define SCTP_SEND_TIME         0
#define READ_POLL_TIMEOUT      100

#define SCTP_LOAD_READ_STOP     1
#define SCTP_LOAD_READ_START    2
#define SCTP_LOAD_READ_DROP     3
#define SCTP_FP_TEMP_BUF 1024
typedef std::vector<Sctp *> SctpConns;
/**
@brief SctpCnx base class to create Sctp connections.

@author Franco Sinhori <franco.sinhori.ext\@siemens.com> , Alexandre Godoi <alexandre.godoi\@siemens.com>
*/
class SctpCnx : public SctpUpperSend
{
public:
    SctpCnx();
    virtual ~SctpCnx();

    virtual bool start(const VectorString &localAddrs, unsigned short localPort,
                       const VectorString &remoteAddrs, unsigned short remotePort,
                       unsigned short numOstreams, unsigned short maxInstreams,
                       SctpUpperReceive *pCallback, int timeout,
                       unsigned short &numOstreamsOut, unsigned short &maxInstreamsOut) = 0;
    virtual void stop() = 0;
    static int getLastError();
    static std::string getLastErrorMsg();
    bool send(const void *msg, unsigned int len, unsigned short stream,
                   bool ordered);
    bool send(const void *msg, unsigned int len, unsigned short stream, bool ordered, void *pConn);
    inline void setUpperReceive(SctpUpperReceive *callback) {mpCallback = callback;}
    inline void setPayloadHandling(int c) {mPayloadControl = c;} ///< Set ppid which servers to identify the upper protocol.
    inline void setProtocol(int protocol) {ppid = htonl(protocol);} ///< Set ppid which servers to identify the upper protocol.
    bool mIsSctpSocketClosed; ///< Flag to alert upper layers when Sctp Socket is closed.
    inline void configSctp(ConfigSctpLayer param) {
        errno = 0;
        int ret = mSctpCnx.configSctp(param);
        char *s = new char[100];
       
        if(ret != 0){
            sprintf(s," error in configSctp(): %s ",strerror(errno));
            mLog.log(LOG_TW_ERRO, "SctpCnx::configSctp() - failed, %s ret=%d, errno=%d", s, ret, errno);
        }
        delete[] s;
        mhType = param.mhType;
    }
    
    inline void setMultiHomingType(ConfigSctpLayer &cfg, std::string type){
        if(type == "Standard")
            cfg.mhType = STANDARD_MULTI;
        else if(type == "NSN-Private")
            cfg.mhType = NSN_PRIVATE_MULTI;
        else{
            cfg.mhType = STANDARD_MULTI;
            mLog.log(LOG_TW_ERRO, "SCTP - not supported multi-homing type: %s, standard mode will be used instead.", type.c_str());
        }
    }
    
protected:
    Sctp mSctpCnx;
    SctpConns mSctpCnxs;

    SctpUpperReceive *mpCallback;   ///< callback object to deliver data to the upper protocol layer.
    pthread_t mTidCallback;         ///< thread id for callbackThread()
    bool mbStopThread;              ///< flag to stop callbackThread()
    unsigned int ppid;              ///< Identifies the upper protocol which Sctp is carring.
    multiHomingType mhType;         ///< Indicates multi-homing type
	unsigned int mPayloadControl;   ///< Identifies the process of the SCTP payload.

    bool startThread(SctpUpperReceive *pCallback);
    void stopThread();
    Sctp* allocSctp();
    void deleteAllSctp();
    SctpConns::iterator getSctpConn(int index);
    void setSctpConn(Sctp *pSctp);
    void deleteSctp(SctpConns::iterator iterPos);
    
    static void *callbackThread(void *arg);
    virtual void callbackThread();

    std::string getAddrs(const VectorString &addrs);
    
    TwLog mLog;                     ///< Object used for logging.
};

#endif // ifndef _SctpCnx_h
