#include "Sctp.h"



//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

Sctp::Sctp() {
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

Sctp::~Sctp() {
}

/**
 @brief Returns a reference to mSocket member variable.

@returns A reference to mSocket member variable.
 */
Socket6& Sctp::getSctpSocket() {
    return mSocket;
}

/**
 @brief Returns the socket file descriptor.

@returns The file descriptor.
 */
int Sctp::getRawSocket() {
    return mSocket.getSocket();
}

/**
 @brief Create an SCTP (one-to-one) socket.

@returns Nonzero if the function is successful; otherwise 0, and a specific 
 error code can be retrieved by calling getLastError().
@see Sctp::getLastError
 */
bool Sctp::create() {
    bool bRet = false;

    bRet = mSocket.create(SOCK_STREAM, IPPROTO_SCTP);

    return bRet;
}

/**
 @brief Closes the Sctp socket.
 */
void Sctp::close() {
    mSocket.close();
}

/**
 @brief Attaches a socket(file descriptor) to a Socket object.

@param sock Contains a socket(file descriptor).
@returns Nonzero if the function is successful; otherwise 0, and a specific 
 error code can be retrieved by calling getLastError().
@see Sctp::getLastError
 */
bool Sctp::attach(int sock) {
    bool bRet = false;

    bRet = mSocket.attach(sock);

    return bRet;
}

/**
 @brief Detaches the socket(file descriptor) in the mSocket data member from the 
 Socket object and set mSocket to -1.

@returns socket(file descriptor) detached.
 */
int Sctp::detach() {
    int s;

    s = mSocket.detach();

    return s;
}

/**
 @brief  Sets the O_NONBLOCK flag.

Enables or Disables the SCTP's Nagle algorithm

@returns Nonzero if the function is successful; otherwise 0, and a specific 
 error code can be retrieved by calling getLastError().
@see Sctp::getLastError
 */
bool Sctp::setNonBlockMode() {
    int val;
    bool bRet = false;

    if ((val = mSocket.fcntl(F_GETFL, 0)) != -1) {
        if (mSocket.fcntl(F_SETFL, val | O_NONBLOCK) != -1)
            bRet = true;
        else
            bRet = false;
    } else
        bRet = false;

    return bRet;
}

/**
 @brief  Allows a caller to fetch, enable, or disable various SCTP notifications.

@param dataIoEvt Enables/disables sctp_sndrcvinfo to come with each recvmsg.
@param associationEvt Enables/disables associations notifications.
@param addressEvt Enables/disables address notifications.
@param sendFailureEvt Enables/disables message send failure notifications.
@param peerErrorEvt Enables/disables peer protocol error notifications.
@param shutdownEvt Enables/disables shutdown notifications.
@param partialDeliveryEvt Enables/disables partial-delivery API notifications.
@param adaptionLayerEvt Enables/disables adaptions layer notification.
@returns Nonzero if the function is successful; otherwise 0, and a specific 
 error code can be retrieved by calling getLastError().
@see Sctp::getLastError
 */
bool Sctp::setNotifications(bool dataIoEvt, bool associationEvt, bool addressEvt, bool sendFailureEvt,
        bool peerErrorEvt, bool shutdownEvt, bool partialDeliveryEvt, bool adaptionLayerEvt) {
    struct sctp_event_subscribe subscribe;
    bool bRet = false;

    subscribe.sctp_data_io_event = dataIoEvt ? 1 : 0;
    subscribe.sctp_association_event = associationEvt ? 1 : 0;
    subscribe.sctp_address_event = addressEvt ? 1 : 0;
    subscribe.sctp_send_failure_event = sendFailureEvt ? 1 : 0;
    subscribe.sctp_peer_error_event = peerErrorEvt ? 1 : 0;
    subscribe.sctp_shutdown_event = shutdownEvt ? 1 : 0;
    subscribe.sctp_partial_delivery_event = partialDeliveryEvt ? 1 : 0;
    subscribe.sctp_adaptation_layer_event = adaptionLayerEvt ? 1 : 0;

    bRet = mSocket.setSockopt(SOL_SCTP, SCTP_EVENTS, &subscribe, sizeof (subscribe));

    return bRet;
}

/**
 @brief  Associates "n" local addresses with the Sctp socket.

@param vetAddrs The network addresses list (IPV6 or IPV4).
@param port SCTP socket port to bind.
@returns Nonzero if the function is successful; otherwise 0, and a specific 
 error code can be retrieved by calling getLastError().
@see Sctp::getLastError
 */
bool Sctp::bind(const VectorString &vetAddrs, unsigned short port) {
    bool bRet = false;

    bRet = mSocket.sctp_bindx(vetAddrs, port, SCTP_BINDX_ADD_ADDR);

    return bRet;
}

/**
 @brief  Sets the default initial parameters used on an SCTP socket when sending out the INIT message.

@param numOstreams Represents the number of outbound SCTP streams an application would like to request.
@param maxInstreams Represents the maximum number of inbound streams the application is prepared to allow.
@param maxAttempts Expresses how many times the SCTP stack should send the initial 
 INIT message before considering the peer endpoint unreachable.
@param maxInitTimeo Represents the maximum RTO value for the INIT timer.
@returns Nonzero if the function is successful; otherwise 0, and a specific 
 error code can be retrieved by calling getLastError().
@see Sctp::getLastError
 */
bool Sctp::init(unsigned short numOstreams, unsigned short maxInstreams,
        unsigned short maxAttempts, unsigned short maxInitTimeo) {
    bool bRet = false;

    struct sctp_initmsg initmsg;

    initmsg.sinit_num_ostreams = numOstreams;
    initmsg.sinit_max_instreams = maxInstreams;
    initmsg.sinit_max_attempts = maxAttempts;
    initmsg.sinit_max_init_timeo = maxInitTimeo;

    bRet = mSocket.setSockopt(IPPROTO_SCTP, SCTP_INITMSG, &initmsg, sizeof (initmsg));

    return bRet;
}

//_syscall1(int, _sysctl, struct __sysctl_args *, args);
//int sysctl(int *name, int nlen, void *oldval, size_t *oldlenp,
//        void *newval, size_t newlen) {
//    struct __sysctl_args args = {name, nlen, oldval, oldlenp, newval, newlen};
//    return _sysctl(&args);
//}

/**
 @brief  Sets SCTP control parameters
 @param  param parameters read from system config file
 */
int Sctp::configSctp(ConfigSctpLayer param) {
    int ret;
    char cmd[500];
    //     sprintf(cmd, "sudo /sbin/sysctl -q -w net.sctp.rto_initial=3000 net.sctp.rto_min=1000 net.sctp.rto_max=60000 net.sctp.valid_cookie_life=60000 net.sctp.max_burst=4 net.sctp.association_max_retrans=10 net.sctp.path_max_retrans=5 net.sctp.max_init_retransmits=8 net.sctp.hb_interval=30000 net.sctp.cookie_preserve_enable=1 net.sctp.addip_enable=0");
    sprintf(cmd, "\nsudo /sbin/sysctl -q -w net.sctp.rto_initial=%d "
            "net.sctp.rto_min=%d "
            "net.sctp.rto_max=%d "
            "net.sctp.valid_cookie_life=%d "
            "net.sctp.max_burst=%d "
            "net.sctp.association_max_retrans=%d "
            "net.sctp.path_max_retrans=%d "
            "net.sctp.max_init_retransmits=%d "
            "net.sctp.hb_interval=%d "
            "net.sctp.cookie_preserve_enable=%d "
            //read only parameters in lksctp 1.0.11
            //            "net.sctp.rto_alpha_exp_divisor=%d "
            //            "net.sctp.rto_beta_exp_divisor=%d "
            "net.sctp.addip_enable=%d",
            param.rtoInitial,
            param.rtoMin,
            param.rtoMax,
            param.validCookieLife,
            param.maxBurst,
            param.associationMaxRetrans,
            param.pathMaxRetrans,
            param.maxInitRetransmits,
            param.hbInterval,
            param.cookiePreserveEnable,
            //            param.rtoAlphaExpDivisor,
            //            param.rtoBetaExpDivisor,
            param.addipEnable);
    mLog.log(LOG_TW_DBUG, "Sctp::configSctp() cdm = %s", cmd);
//    mLog.log(LOG_TW_INFO, "Sctp::configSctp() SCTP CTL parameters collected.");
    ret = system(cmd);
    //    ret = system("sudo rm -rf /home/qianl/cmdtest");
    return ret;

    //    int paramName[] = {CTL_NET, NET_SCTP_RTO_INITIAL};
    //    int ret = sysctl(paramName, 2, 0, 0, &param.rtoInitial, sizeof(param.rtoInitial));
    //    if (!ret){
    //        return ret;
    //    }
    //    paramName[1] = NET_SCTP_RTO_MIN;
    //    ret = sysctl(paramName, 2, 0, 0, &param.rtoMin, sizeof(param.rtoMin));
    //    if (!ret){
    //        return ret;
    //    }
    //    paramName[1] = NET_SCTP_RTO_MAX;
    //    ret = sysctl(paramName, 2, 0, 0, &param.rtoMax, sizeof(param.rtoMax));
    //    if (!ret){
    //        return ret;
    //    }
    //
    //    paramName[1] = NET_SCTP_RTO_ALPHA;
    //    ret = sysctl(paramName, 2, 0, 0, &param.rtoAlphaExpDivisor, sizeof(param.rtoAlphaExpDivisor));
    //    if (!ret){
    //        return ret;
    //    }
    //    paramName[1] = NET_SCTP_RTO_BETA;
    //    ret = sysctl(paramName, 2, 0, 0, &param.rtoBetaExpDivisor, sizeof(param.rtoBetaExpDivisor));
    //    if (!ret){
    //        return ret;
    //    }
    //    paramName[1] = NET_SCTP_VALID_COOKIE_LIFE;
    //    ret = sysctl(paramName, 2, 0, 0, &param.validCookieLife, sizeof(param.validCookieLife));
    //    if (!ret){
    //        return ret;
    //    }
    //    paramName[1] = NET_SCTP_ASSOCIATION_MAX_RETRANS;
    //    ret = sysctl(paramName, 2, 0, 0, &param.associationMaxRetrans, sizeof(param.associationMaxRetrans));
    //    if (!ret){
    //        return ret;
    //    }
    //    paramName[1] = NET_SCTP_PATH_MAX_RETRANS;
    //    ret = sysctl(paramName, 2, 0, 0, &param.pathMaxRetrans, sizeof(param.pathMaxRetrans));
    //    if (!ret){
    //        return ret;
    //    }
    //    paramName[1] = NET_SCTP_MAX_INIT_RETRANSMITS;
    //    ret = sysctl(paramName, 2, 0, 0, &param.maxInitRetransmits, sizeof(param.maxInitRetransmits));
    //    if (!ret){
    //        return ret;
    //    }
    //    paramName[1] = NET_SCTP_HB_INTERVAL;
    //    ret = sysctl(paramName, 2, 0, 0, &param.hbInterval, sizeof(param.hbInterval));
    //    if (!ret){
    //        return ret;
    //    }
    //    paramName[1] = NET_SCTP_MAX_BURST;
    //    ret = sysctl(paramName, 2, 0, 0, &param.maxBurst, sizeof(param.maxBurst));
    //    if (!ret){
    //        return ret;
    //    }
    //    paramName[1] = NET_SCTP_ADDIP_ENABLE;
    //    ret = sysctl(paramName, 2, 0, 0, &param.addipEnable, sizeof(param.addipEnable));
    //    if (!ret){
    //        return ret;
    //    }
    //    paramName[1] = NET_SCTP_PRESERVE_ENABLE;
    //    ret = sysctl(paramName, 2, 0, 0, &param.cookiePreserveEnable, sizeof(param.cookiePreserveEnable));
    //    if (!ret){
    //        return ret;
    //    }

}

/**
 @brief  Establishes a Sctp socket to listen for incoming connection requests.

@param backlog Defines  the maximum length the queue of pending connections may grow to.
@returns Nonzero if the function is successful; otherwise 0, and a specific 
 error code can be retrieved by calling getLastError().
@see Sctp::getLastError
 */
bool Sctp::listen(int backlog) {
    bool bRet = false;

    bRet = mSocket.listen(backlog);

    return bRet;
}

/**
 @brief  Accepts Sctp client connection request.

@param sctp sctp object to accept request.
 
@see Sctp::attach
 */
bool Sctp::accept(Sctp &sctp) {
    Socket6 sock;
    bool bRet = false;

    bRet = mSocket.accept(sock);

    if (bRet) {
        bRet = sctp.attach(sock.detach());
    }

    return bRet;
}

/**
 @brief  Establishes a connection to a peer Sctp socket.

@param addr The network address of the Sctp socket to which this object is connected.
@param port The port identifying the Sctp socket application.
@returns Nonzero if the function is successful; otherwise 0, and a specific 
 error code can be retrieved by calling getLastError().
@see Sctp::getLastError
 */
bool Sctp::connect(const char *addr, unsigned short port)
//bool Sctp::connect(const VectorString &addrs, unsigned short port)
{
    bool bRet = false;

    //bRet = mSocket.sctp_connectx(addrs, port);
    bRet = mSocket.connect(addr, port);

    return bRet;
}

bool Sctp::connectx(const VectorString &vetAddrs, unsigned short port)
//bool Sctp::connect(const VectorString &addrs, unsigned short port)
{
    bool bRet = false;

    //bRet = mSocket.sctp_connectx(addrs, port);
    bRet = mSocket.sctp_connectx(vetAddrs, port);

    return bRet;
}

/**
 @brief  Gets all locally bound addresses on the Sctp socket.

@param vetAddrs Vector to be filled with local addresses.
@param port Value filled with local bound port.
@returns Nonzero if the function is successful; otherwise 0, and a specific 
 error code can be retrieved by calling getLastError().
@see Sctp::getLastError
 */
bool Sctp::getLocalAddrs(VectorString &vetAddrs, unsigned short &port) {
    bool bRet = false;

    bRet = mSocket.sctp_getladdrs(vetAddrs, port);

    return bRet;
}

/**
 @brief  Gets all remote connected addresses on the Sctp socket.

@param vetAddrs Vector to be filled with remote addresses.
@param port Value filled with remote bound port.
@returns Nonzero if the function is successful; otherwise 0, and a specific 
 error code can be retrieved by calling getLastError().
@see Sctp::getLastError
 */
bool Sctp::getPeerAddrs(VectorString &vetAddrs, unsigned short &port) {
    bool bRet = false;

    bRet = mSocket.sctp_getpaddrs(vetAddrs, port);

    return bRet;
}

/**
 @brief Checks if two network addresses are iqual or not.

@param addr1 Network address to be compared.
@param addr2 Network address to be compared.
@returns true if matches.
 */
//////////////////////////////////////////////////////////////////////////////

bool Sctp::addrsCmp(std::string addr1, std::string addr2) {
    bool bRet = false;

    bRet = mSocket.addrs_cmp(addr1, addr2);

    return bRet;
}

/**
 @brief Returns the number of input and output streams.

@param instrms Number of input streams.
@param outstrms Number of output streams.
@returns Nonzero if the function is successful; otherwise 0, and a specific 
 error code can be retrieved by calling getLastError().
@see Sctp::getLastError
 */
bool Sctp::getStatus(unsigned short &instrms, unsigned short &outstrms) {
    struct sctp_status status;
    unsigned int size = sizeof (status);
    bool bRet = false;

    bzero(&status, sizeof (status));

    bRet = mSocket.sctp_opt_info(SCTP_STATUS, (void *) &status, size);
    if (bRet) {
        instrms = status.sstat_instrms;
        outstrms = status.sstat_outstrms;
    }

    return bRet;
}

bool Sctp::getSockname(std::string &addr, unsigned short &port) {
    return mSocket.getSockname(addr, port);
}

/**
 @brief Sends a message from a SCTP socket while using the advanced features of the SCTP protocol.

@param msg Message to be sent.
@param len Message length in bytes.
@param ppid Holds the pay-load protocol identifier that will be passed with the data chunk.
@param flags Composed of a bitwise OR of the following values: MSG_UNORDERED, 
 MSG_ADDR_OVER, MSG_ABORT, MSG_EOF
@param stream Identifies the stream number.
@param timetolive Specifies  the time duration in milliseconds, where 0 
 represents an infinite lifetime.
@param context An value that is passed back to the upper layer along with 
 the undelivered message if an error occurs on the send of the message.
@param addr Destination address.
@param port Destination port.
@returns Nonzero if the function is successful; otherwise 0, and a specific 
 error code can be retrieved by calling getLastError().
@see Sctp::getLastError
 */
int Sctp::sendMsg(const void *msg, unsigned int len, unsigned int ppid, unsigned int flags,
        unsigned short stream, unsigned int timetolive, unsigned int context,
        const char *addr, unsigned short port) {
    int ret;

    //ret = mSocket.sctp_sendmsg(msg, len, ppid, flags, stream, timetolive, context,
    //                           addr, port);
    ret = mSocket.sctp_sendmsg(msg, len, stream, addr, port, ppid, flags,
            timetolive, context);

//    ret = mSocket.send(mSocket.getSocket(), msg, len, 0);
//    if (ret == -1 && errno == EPIPE) {
//        ret = close(mSocket.getSocket());
//        if (ret != 0) {
//            fprintf(stderr, "close failed %s\n", strerror(errno));
//            exit(1);
//        }
//        *skp = sk = build_endpoint(argv0, local_port);
//        break;
//    }
    return ret;
}

/**
 @brief Receives a message from a SCTP socket while using the advanced features of the SCTP protocol.

@param len Message length in bytes.
@param msg Buffer to be filled with the received message.
@param sinfo A sctp_sndrcvinfo structure to be filled upon receipt of the message.
@param msg_flags A value that is filled with any message flags like MSG_NOTIFICATION or MSG_EOR.
@param addr Filled with the Remote address.
@param port Filled with the remote port.
@returns Nonzero if the function is successful; otherwise 0, and a specific 
 error code can be retrieved by calling getLastError().
@see Sctp::getLastError
 */
int Sctp::recvMsg(void *msg, unsigned int len, struct sctp_sndrcvinfo &sinfo,
        int &msg_flags, std::string &addr, unsigned short &port) {
    int ret;

    ret = mSocket.sctp_recvmsg(msg, len, sinfo, msg_flags, addr, port);

    return ret;
}

/**
 @brief Receives a message from a SCTP socket while using the advanced features of the SCTP protocol.

@param len Message length in bytes.
@param msg Buffer to be filled with the received message.
@param stream Filled with the stream number.
@param ppid Filled with the protocol payload identifier that will be passed with the data chunk.
@param context Filled with a value that is passed back to the upper layer along with the 
 undelivered message if an error occurs on the send of the message.
@param flags Filled with a composed bitwise OR of the following values: MSG_UNORDERED, 
 MSG_ADDR_OVER, MSG_ABORT, MSG_EOF
@param msg_flags A value that is filled with any message flags like MSG_NOTIFICATION or MSG_EOR.
@param addr Filled with the Remote address.
@param port Filled with the remote port.
@returns Nonzero if the function is successful; otherwise 0, and a specific 
 error code can be retrieved by calling getLastError().
@see Sctp::getLastError
 */
int Sctp::recvMsg(void *msg, unsigned int len, unsigned int &stream, unsigned int &ppid,
        unsigned int &context, unsigned int &flags, int &msg_flags,
        std::string &addr, unsigned short &port) {
    struct sctp_sndrcvinfo sinfo;
    int ret;

    bzero(&sinfo, sizeof (sinfo));

    ret = mSocket.sctp_recvmsg(msg, len, sinfo, msg_flags, addr, port);
    if (ret != -1) {
        stream = sinfo.sinfo_stream;
        ppid = sinfo.sinfo_ppid;
        context = sinfo.sinfo_context;
        flags = sinfo.sinfo_flags;
    }

    return ret;
}

/**
 @brief Gets the error status for the last operation that failed.

@returns the return value indicates the error code (errno) for the 
 last sockets API routine performed by this thread.
 */
int Sctp::getLastError() {
    return errno;
}

/**
 @brief Gets the error status for the last operation that failed as a string.

@returns the return string indicates the error string for the last sockets API 
 routine performed by this thread.
 */
std::string Sctp::getLastErrorMsg() {
    std::string s;

    s = strerror(errno);

    return s;
}
