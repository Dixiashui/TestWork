#include "Socket4.h"

/**
 * @brief Socket4 constructor.
 */
Socket4::Socket4() {
}

/**
 * @brief Socket4 destructor.
 */
Socket4::~Socket4() {
    close();
}

/**
 * @brief Creates the IPV4 socket and attach it.
 *
 * @param type     Specifies the communication semantics.  Currently defined types are (more than this values are allowed):
 *                 SOCK_STREAM, SOCK_DGRAM, SOCK_SEQPACKET.
 * @param protocol Specifies a particular protocol to be used with the socket. Usually zero (0) is used to select the 
 *                 default protocol. IPPROTO_SCTP is used for SCTP protocol.
 *
 * @returns true if the function is successful; otherwise false, and a specific
 *          error code can be retrieved by calling Socket::GetLastError().
 */
bool Socket4::create(int type, int protocol) {
    bool bRet = false;

    if (mSocket == -1) {
        if ((mSocket = ::socket(AF_INET, type, protocol)) != -1)
            bRet = true;
        else
            bRet = false;
    } else {
        errno = EBUSY; // Device or resource busy
        bRet = false;
    }

    return bRet;
}

/**
 * @brief Associates a local address with the socket.
 *
 * @param port  The port identifying the socket application. Zero (0) allows Linux to choose an ephemeral port.
 * @param addr  The network address, a dotted number such as "192.168.80.187". NULL binds with any interface.
 *
 * @returns true if the function is successful; otherwise false, and a specific
 *          error code can be retrieved by calling Socket::GetLastError().
 */
bool Socket4::bind(unsigned short port, const char *addr) {
    struct sockaddr_in socketaddr;
    bool bRet = false;

    // fill sockaddr_in structure
    bRet = fillSockaddr(port, addr, socketaddr);

    // bind
    if (bRet) {
        if (::bind(mSocket, (struct sockaddr *) & socketaddr, sizeof (socketaddr)) != -1)
            bRet = true;
        else
            bRet = false;
    }

    return bRet;
}

/**
 * @brief Establishes a connection to a peer socket.
 *
 * @param addr  The network address of the socket to which this object is connected, like "192.168.80.187".
 * @param port  The port identifying the socket application.
 * 
 * @returns true if the function is successful; otherwise false, and a specific
 *          error code can be retrieved by calling Socket::GetLastError().
 */
bool Socket4::connect(const char *addr, unsigned short port) {
    struct sockaddr_in socketaddr;
    bool bRet = false;

    // fill sockaddr_in structure
    bRet = fillSockaddr(port, addr, socketaddr);

    // connect
    if (bRet) {
        if (::connect(mSocket, (struct sockaddr *) & socketaddr, sizeof (socketaddr)) != -1)
            bRet = true;
        else
            bRet = false;
    }

    return bRet;
}

/**
 * @brief Gets the local name and port for a socket.
 *
 * @param addr Dotted number IP address, like "192.168.80.187".
 * @param port Port number.
 * 
 * @returns true if the function is successful; otherwise false, and a specific
 *          error code can be retrieved by calling Socket::GetLastError().
 */
bool Socket4::getSockname(std::string &addr, unsigned short &port) {
    struct sockaddr_in socketaddr;
    socklen_t len = sizeof (socketaddr);
    bool bRet = false;

    ::bzero(&socketaddr, sizeof (socketaddr));

    if (::getsockname(mSocket, (struct sockaddr *) & socketaddr, &len) != -1)
        bRet = getAddr(socketaddr, addr, port);
    else
        bRet = false;

    return bRet;
}

/**
 * @brief Gets the name and port of the peer socket to which the socket is connected.
 *
 * @param addr  Dotted number IP address, like "192.168.80.187".
 * @param port  Port number.
 * 
 * @returns true if the function is successful; otherwise false, and a specific
 *          error code can be retrieved by calling Socket::GetLastError().
 */
bool Socket4::getPeername(std::string &addr, unsigned short &port) {
    struct sockaddr_in socketaddr;
    socklen_t len = sizeof (socketaddr);
    bool bRet = false;

    ::bzero(&socketaddr, sizeof (socketaddr));

    if (::getpeername(mSocket, (struct sockaddr *) & socketaddr, &len) != -1)
        bRet = getAddr(socketaddr, addr, port);
    else
        bRet = false;

    return bRet;
}

/**
 * @brief Associates "n" local addresses with the socket.
 *
 * @param vetAddrs  The network addresses. A vector filled with a dotted number such as "192.168.80.187".
 * @param port      The port identifying the socket application.
 * @param flags     The  flags parameter can be either SCTP_BINDX_ADD_ADDR or SCTP_BINDX_REM_ADDR.
 * 
 * @returns true if the function is successful; otherwise false, and a specific
 *          error code can be retrieved by calling Socket::GetLastError().
 */
bool Socket4::sctp_bindx(const VectorString &vetAddrs, unsigned short port, int flags) {
    sockaddr_in *addrs = NULL;
    int size = vetAddrs.size();
    bool bRet = true;

    addrs = new sockaddr_in[size];

    // fill address structures
    for (int i = 0; i < size && bRet; i++)
        bRet = fillSockaddr(port, vetAddrs[i].c_str(), addrs[i]);

    if (bRet) {
        if (::sctp_bindx(mSocket, (sockaddr *) addrs, size, flags) != -1)
            bRet = true;
        else
            bRet = false;
    }

    // delete array of address structures
    delete [] addrs;

    return bRet;
}

/**
 * @brief Gets all locally bound addresses on the socket.
 *
 * @param vetAddrs  Vector filled with local addresses.
 * @param port      Local bound port.
 * @param assocId   Association ID.
 * 
 * @returns true if the function is successful; otherwise false, and a specific
 *          error code can be retrieved by calling Socket::GetLastError().
 */
bool Socket4::sctp_getladdrs(VectorString &vetAddrs, unsigned short &port, unsigned int assocId) {
    struct sockaddr *addrs = NULL;
    int count;
    bool bRet = false;

    if ((count = ::sctp_getladdrs(mSocket, (sctp_assoc_t) assocId, &addrs)) > 0)
        bRet = getAddrs((struct sockaddr_in *) addrs, count, vetAddrs, port);
    else
        bRet = false;

    // free memory allocated by sctp_getladdrs() call
    if (addrs != NULL)
        ::sctp_freeladdrs(addrs);


    return bRet;
}

/**
 * @brief Gets all remote connected addresses on the socket.
 *
 * @param vetAddrs  Vector filled with remote addresses.
 * @param port      Remote connected port.
 * @param assocId   association ID.
 * 
 * @returns true if the function is successful; otherwise false, and a specific
 *          error code can be retrieved by calling Socket::GetLastError().
 */
bool Socket4::sctp_getpaddrs(VectorString &vetAddrs, unsigned short &port, unsigned int assocId) {
    struct sockaddr *addrs = NULL;
    int count;
    bool bRet = false;

    if ((count = ::sctp_getpaddrs(mSocket, (sctp_assoc_t) assocId, &addrs)) > 0)
        bRet = getAddrs((struct sockaddr_in *) addrs, count, vetAddrs, port);
    else
        bRet = false;

    // free memory allocated by sctp_getpaddrs() call
    if (addrs != NULL)
        ::sctp_freepaddrs(addrs);

    return bRet;
}

/**
 * @brief Sends a message from a socket while using the advanced features of SCTP.
 * 
 * @param msg         Message to be sent.
 * @param len         Message length.
 * @param stream      Identifies the stream number.
 * @param addr        Destination address.
 * @param port        Destination port.
 * @param ppid        Holds the pay-load protocol identifier that will be passed with the data chunk 
 * @param flags       Composed of a bitwise OR of the following values: MSG_UNORDERED, MSG_ADDR_OVER, MSG_ABORT, MSG_EOF.
 * @param timetolive  Specifies  the time duration in milliseconds, where 0 represents an infinite lifetime.
 * @param context     An value that is passed back to the upper layer along with the 
 *                    undelivered message if an error occurs on the send of the message.
 * 
 * @returns The number of characters sent, or -1 if an error ccurred and a specific 
 *          error code can be retrieved by calling Socket::GetLastError().
 */
int Socket4::sctp_sendmsg(const void *msg, unsigned int len, unsigned short stream,
        const char *addr, unsigned short port, unsigned int ppid,
        unsigned int flags, unsigned int timetolive, unsigned int context) {
    int ret;
    struct sockaddr_in socketaddr;

    if (fillSockaddr(port, addr, socketaddr)) {
        ret = ::sctp_sendmsg(mSocket, msg, (size_t) len, (struct sockaddr *) & socketaddr, (socklen_t)sizeof (socketaddr),
                (uint32_t) ppid, (uint32_t) flags, (uint16_t) stream, (uint32_t) timetolive,
                (uint32_t) context);
    } else {
        errno = EINVAL; // Invalid argument
        ret = -1;
    }

    return ret;
}

/**
 * @brief Receives a message from a socket while using the advanced features of SCTP.
 *
 * @param msg        Buffer to receive message.
 * @param len        Message length.
 * @param sinfo      A sctp_sndrcvinfo structure to be filled upon receipt of the message.
 * @param msg_flags  A value that is filled with any message flags like MSG_NOTIFICATION or MSG_EOR.
 * @param addr       Remote address.
 * @param port       Remote port.
 * 
 * @returns The number of characters received, or -1 if an error occurred and a specific 
 *          error code can be retrieved by calling Socket::GetLastError();
 */
int Socket4::sctp_recvmsg(void *msg, unsigned int len, struct sctp_sndrcvinfo &sinfo, int &msg_flags,
        std::string &addr, unsigned short &port) {
    int ret;
    struct sockaddr_in socketaddr;
    socklen_t fromlen = sizeof (socketaddr);

    ret = ::sctp_recvmsg(mSocket, msg, (size_t) len, (struct sockaddr *) & socketaddr, &fromlen,
            &sinfo, &msg_flags);

    if (ret > 0) {
        if (!getAddr(socketaddr, addr, port)) {
            errno = EADDRNOTAVAIL; // Cannot assign requested address
            ret = -1; // invalid address
        }
    }

    return ret;
}

/**
 * @brief Fills sockaddr_in structure with port and address.
 *
 * @param port        Port number.
 * @param addr        Address.
 * @param socketaddr  sockaddr_in structure.
 *
 * @returns true if sockaddr_in structure was filled, otherwise false.
 */
bool Socket4::fillSockaddr(unsigned short port, const char *addr, struct sockaddr_in &socketaddr) {
    bool bRet = false;
    int r;

    ::bzero(&socketaddr, sizeof (socketaddr));

    if (addr == NULL) {
        socketaddr.sin_addr.s_addr = ::htonl(INADDR_ANY);
        socketaddr.sin_family = AF_INET;
        socketaddr.sin_port = ::htons(port);
        bRet = true;
    } else {
        if ((r = ::inet_pton(AF_INET, addr, &socketaddr.sin_addr)) > 0) {
            socketaddr.sin_family = AF_INET;
            socketaddr.sin_port = ::htons(port);
            bRet = true;
        } else if (r == 0) {
            errno = EINVAL; // invalid character string representing a valid network address
            bRet = false;
        } else {
            bRet = false;
        }
    }

    return bRet;
}

/**
 * @brief Gets address and port number from sockaddr_in structure.
 *
 * @param socketaddr  sockaddr_in structure.
 * @param addr        Address.
 * @param port        Port Number.
 *
 * @return true if values were gotten, otherwise false.
 */
bool Socket4::getAddr(const struct sockaddr_in &socketaddr, std::string &addr, unsigned short &port) {
    char dst[INET_ADDRSTRLEN];
    bool bRet = false;

    if (::inet_ntop(AF_INET, &socketaddr.sin_addr, dst, sizeof (dst)) != NULL) {
        addr = dst;
        port = ::ntohs(socketaddr.sin_port);
        bRet = true;
    } else
        bRet = false;

    return bRet;
}

/**
 * @brief Gets addresses and port number from an array of sockaddr_in structures.
 *
 * @param socketaddr  Array of sockaddr_in structures.
 * @param size        Size of the array.
 * @param vetAddrs    Adresses.
 * @param port        Port number.
 *
 * @return true if values were gotten, otherwise false.
 */
bool Socket4::getAddrs(const struct sockaddr_in socketaddr[], int size, VectorString &vetAddrs,
        unsigned short &port) {
    char dst[INET_ADDRSTRLEN];
    bool bRet = true;

    // allocate vector positions
    vetAddrs.reserve(size);

    // get remote port
    port = ::ntohs(socketaddr[0].sin_port);

    // get remote host names
    for (int i = 0; i < size && bRet; i++) {
        if (::inet_ntop(AF_INET, &socketaddr[i].sin_addr, dst, sizeof (dst)) != NULL)
            vetAddrs.push_back(dst);
        else
            bRet = false; // invalid address
    }

    if (!bRet)
        vetAddrs.clear();

    return bRet;
}

