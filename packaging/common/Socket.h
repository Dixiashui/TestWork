#ifndef _Socket_h
#define _Socket_h

/**
 * @file Socket.h
 * @brief Defines the Socket class.
 */

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/poll.h>
#include <arpa/inet.h>
#include <netinet/sctp.h>
#include <netinet/tcp.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <string>
#include <vector>
#include <fcntl.h>
#include <sstream>
#include "common/Types.h"

/**
 * @brief Socket class is a wrapper for Linux socket.
 * 
 *
 * This class is used as a base class and cannot be instanced. This class
 * provides common services for Socket6 and Socket4 derived classes. All pure 
 * virtual methods are address dependent and must be implemented in derived classes.
 *
 * @author Alexandre Maia Godoi <alexandre.maia@siemens.com>
 */
class Socket {
public:
    Socket();
    virtual ~Socket();

    /**
     * @brief Creates the socket and attach it.
     *
     * @param type     Specifies the communication semantics. 
     * @param protocol Specifies a particular protocol to be used with the socket. Usually zero (0) is used to select the
     *                 default protocol.
     *
     * @returns true if the function is successful; otherwise false, and a specific
     *          error code can be retrieved by calling Socket::GetLastError().
     */
    virtual bool create(int type, int protocol = 0) = 0;

    /**
     * @brief Associates a local address with the socket.
     *
     * @param port  The port identifying the socket application.
     * @param addr  The network address, a dotted number such as "192.168.80.187". NULL binds with any interface.
     *
     * @returns true if the function is successful; otherwise false, and a specific
     *          error code can be retrieved by calling Socket::GetLastError().
     */
    virtual bool bind(unsigned short port, const char *addr = NULL) = 0;

    /**
     * @brief Establishes a connection to a peer socket.
     *
     * @param addr  The network address of the socket to which this object is connected, like "192.168.80.187".
     * @param port  The port identifying the socket application.
     *
     * @returns true if the function is successful; otherwise false, and a specific
     *          error code can be retrieved by calling Socket::GetLastError().
     */
    virtual bool connect(const char *addr, unsigned short port) = 0;

    /**
     * @brief Gets the local name and port for a socket.
     *
     * @param addr Dotted number IP address, like "192.168.80.187".
     * @param port port number.
     *
     * @returns true if the function is successful; otherwise false, and a specific
     *          error code can be retrieved by calling Socket::GetLastError().
     */
    virtual bool getSockname(std::string &addr, unsigned short &port) = 0;

    /**
     * @brief Gets the name and port of the peer socket to which the socket is connected.
     *
     * @param addr  Dotted number IP address, like "192.168.80.187".
     * @param port  Port number.
     *
     * @returns true if the function is successful; otherwise false, and a specific
     *          error code can be retrieved by calling Socket::GetLastError().
     */
    virtual bool getPeername(std::string &addr, unsigned short &port) = 0;

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
    virtual bool sctp_bindx(const VectorString &vetAddrs, unsigned short port, int flags) = 0;

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
    virtual bool sctp_getladdrs(VectorString &vetAddrs, unsigned short &port, unsigned int assocId = 0) = 0;

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
    virtual bool sctp_getpaddrs(VectorString &vetAddrs, unsigned short &port, unsigned int assocId = 0) = 0;

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
    virtual int sctp_sendmsg(const void *msg, unsigned int len, unsigned short stream,
            const char *addr, unsigned short port, unsigned int ppid = 0,
            unsigned int flags = 0, unsigned int timetolive = 0, unsigned int context = 0) = 0;

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
    virtual int sctp_recvmsg(void *msg, unsigned int len, struct sctp_sndrcvinfo &sinfo, int &msg_flags,
            std::string &addr, unsigned short &port) = 0;

    int getSocket();

    bool accept(Socket &sock);
    void close();

    bool attach(int sock);
    int detach();

    bool listen(int backlog = 5);

    bool getSockopt(int level, int optname, void *optval, socklen_t *optlen);
    bool setSockopt(int level, int optname, const void *optval, socklen_t optlen);

    bool setNonBlockMode(bool enable = true);
    bool setTcpNoDelay(bool enable = true);

    bool setReuseAddr(bool enable = true);
    bool setReusePort(bool enable = true);

    int fcntl(int cmd);
    int fcntl(int cmd, long arg);
    int fcntl(int cmd, struct flock &lock);

    int send(const void *buf, unsigned int len, int flags = 0);
    int recv(void *buf, unsigned int len, int flags = 0);

    int writen(const void *buf, unsigned int len, int timeout = -1);
    int readn(void *buf, unsigned int len, int timeout = -1);
    int read(void *buf, unsigned int len, int timeout = -1);

    bool sctp_opt_info(int opt, void *arg, unsigned int &size, unsigned int assocId = 0);

    bool setSctpNoDelay(bool enable = true);

    int sctp_sendmsg(const void *msg, unsigned int len, unsigned short stream, unsigned int ppid = 0,
            unsigned int flags = 0, unsigned int timetolive = 0, unsigned int context = 0);
    int sctp_recvmsg(void *msg, unsigned int len, struct sctp_sndrcvinfo &sinfo, int &msg_flags);

    static int getLastError();
    static std::string getLastErrorMsg();

protected:
    int mSocket; ///< Handle to Linux socket.
};

#endif // ifndef _Socket_h

