//#include "stacks/sctp_common/Socket.h"
#include "Socket.h"

/**
 * @brief Socket constructor.
 */
Socket::Socket() {
    mSocket = -1;
}

/**
 * @brief Socket destructor.
 */
Socket::~Socket() {
    close();
}

/**
 * @brief Gets the mSocket member variable.
 *
 * @returns Socket::mSocket member variable.
 */
int Socket::getSocket() {
    return mSocket;
}

/**
 * @brief Closes the socket.
 */
void Socket::close() {
    if (mSocket != -1) {
        ::close(mSocket);
        mSocket = -1;
    }
}

/**
 * @brief Accepts a connection on the socket.
 *
 * @param socket A reference identifying a new Socket that is available for connection.
 *
 * @returns true if the function is successful; otherwise false, and a
 *          specific error code can be retrieved by calling Socket::GetLastError().
 */
bool Socket::accept(Socket &sock) {
    int s;
    bool bRet = false;

    if ((s = ::accept(mSocket, NULL, NULL)) != -1) {
        // attach socket s
        bRet = sock.attach(s);

        if (!bRet) // cannot attach socket s
            ::close(s);
    } else
        bRet = false;

    return bRet;
}

/**
 * @brief Attaches a socket(file descriptor) to a Socket object.
 *
 * @param socket Contains a socket(file descriptor).
 *
 * @returns true if the function is successful; otherwise false, and a
 *          specific error code can be retrieved by calling Socket::GetLastError().
 */
bool Socket::attach(int sock) {
    bool bRet = false;

    if (mSocket == -1) {
        mSocket = sock;
        bRet = true;
    } else {
        errno = EPERM; // operation not permitted
        //errno = EBUSY;        // Device or resource busy
        //errno = ETOOMANYREFS; // Too many references: cannot splice

        bRet = false;
    }

    return bRet;
}

/**
 * @brief Detaches the socket(file descriptor) in the mSocket data member from the Socket object.
 *
 * @returns Socket(file descriptor) detached.
 */
int Socket::detach() {
    int s = mSocket;

    mSocket = -1;

    return s;
}

/**
 * @brief Establishes a Socket to listen for incoming connection requests.
 *
 * @param backlog Defines  the maximum length the queue of pending connections may grow to.
 *
 * @returns true if the function is successful; otherwise false, and a
 *          specific error code can be retrieved by calling Socket::GetLastError().
 */
bool Socket::listen(int backlog) {
    bool bRet = false;

    if (::listen(mSocket, backlog) != -1)
        bRet = true;
    else
        bRet = false;

    return bRet;
}

/**
 * @brief  Retrieves a socket option.
 *
 * @param level    Specifies the code in the system that interprets the option:
 *                 the general socket code (SOL_SOCKET.) or some protocol-specific
 *                 code (e.g., IPv4, IPv6, TCP, or SCTP).
 * @param optname  The socket option for which the optval is to be retrieved.
 * @param optval   It is a pointer to a variable from which the new value of the option is stored.
 * @param optlen   optval size.
 * 
 * @returns true if the function is successful; otherwise false, and a
 *          specific error code can be retrieved by calling Socket::GetLastError().
 */
bool Socket::getSockopt(int level, int optname, void *optval, socklen_t *optlen) {
    bool bRet = false;

    if (::getsockopt(mSocket, level, optname, optval, optlen) != -1)
        bRet = true;
    else
        bRet = false;

    return bRet;
}

/**
 * @brief Sets a socket option.
 *
 * @param level    Specifies the code in the system that interprets the option:
 *                 the general socket code (SOL_SOCKET.) or some protocol-specific
 *                 code (e.g., IPv4, IPv6, TCP, or SCTP).
 * @param optname  The socket option for which the optval is to be set.
 * @param optval   It is a pointer to a variable from which the new value of the option is fetched.
 * @param optlen   optval size.
 * 
 * @returns true if the function is successful; otherwise false, and a
 *          specific error code can be retrieved by calling Socket::GetLastError().
 */
bool Socket::setSockopt(int level, int optname, const void *optval, socklen_t optlen) {
    bool bRet = false;

    if (::setsockopt(mSocket, level, optname, optval, optlen) != -1)
        bRet = true;
    else
        bRet = false;

    return bRet;
}

/**
 * @brief Enables or disables the non block mode.
 *
 * @param enable  true enables non block mode, false disables non block mode.
 *
 * @returns true if the function is successful; otherwise false, and a
 *          specific error code can be retrieved by calling Socket::GetLastError().
 */
bool Socket::setNonBlockMode(bool enable) {
    bool bRet = false;
    int val;

    val = ::fcntl(mSocket, F_GETFL, 0);

    if (val >= 0) {
        if (enable)
            bRet = ::fcntl(mSocket, F_SETFL, val | O_NONBLOCK) != -1 ? true : false;
        else
            bRet = ::fcntl(mSocket, F_SETFL, val & ~O_NONBLOCK) != -1 ? true : false;
    }

    return bRet;
}

/**
 * @brief Enables or disables the TCP's Nagle algorithm.
 *
 * @param enable true disables TCP's Nagle algorithm, false enables TCP's Nagle algorithm.
 * 
 * @returns true if the function is successful; otherwise false, and a
 *          specific error code can be retrieved by calling Socket::GetLastError().
 */
bool Socket::setTcpNoDelay(bool enable) {
    bool bRet = false;
    int optval;

    optval = enable ? 1 : 0;

    if (::setsockopt(mSocket, IPPROTO_TCP, TCP_NODELAY, (const void *) & optval, sizeof (optval)) != -1)
        bRet = true;
    else
        bRet = false;

    return bRet;
}

/**
 * @brief Allows or disallows local address reuse.
 *
 * SO_REUSEADDR allows your server to bind to an address which is in a 
 * TIME_WAIT state. It does not allow more than one server to bind to the 
 * same address. It was mentioned that use of this flag can create a security 
 * risk because another server can bind to a the same port, by binding to a 
 * specific address as opposed to INADDR_ANY. The SO_REUSEPORT flag allows 
 * multiple processes to bind to the same address provided all of them use 
 * the SO_REUSEPORT option.
 * From Richard Stevens (rstevens@noao.edu):
 *
 * @param enable true allows address reuse, false disallows address reuse.
 * 
 * @returns true if the function is successful; otherwise false, and a
 *          specific error code can be retrieved by calling Socket::GetLastError().
 */
bool Socket::setReuseAddr(bool enable) {
    bool bRet = false;
    int optval;

    optval = enable ? 1 : 0;

    if (::setsockopt(mSocket, SOL_SOCKET, SO_REUSEADDR, (const void *) & optval, sizeof (optval)) != -1)
        bRet = true;
    else
        bRet = false;

    return bRet;
}

/**
 * @brief Allows or disallows local address reuse.
 
 * SO_REUSEADDR allows your server to bind to an address which is in a 
 * TIME_WAIT state. It does not allow more than one server to bind to the 
 * same address. It was mentioned that use of this flag can create a security 
 * risk because another server can bind to a the same port, by binding to a 
 * specific address as opposed to INADDR_ANY. The SO_REUSEPORT flag allows 
 * multiple processes to bind to the same address provided all of them use 
 * the SO_REUSEPORT option.
 * From Richard Stevens (rstevens@noao.edu):
 *
 * @param enable true allows address reuse, false disallows address reuse.
 * 
 * @returns true if the function is successful; otherwise false, and a
 *          specific error code can be retrieved by calling Socket::GetLastError().
 */
bool Socket::setReusePort(bool enable) {
    const int SO_REUSEPORT = 15; // it is commented in /usr/include/asm/socket.h ... then it is defined here
    bool bRet = false;
    int optval;

    optval = enable ? 1 : 0;

    if (::setsockopt(mSocket, SOL_SOCKET, SO_REUSEPORT, (const void *) & optval, sizeof (optval)) != -1)
        bRet = true;
    else
        bRet = false;

    return bRet;
}

/**
 * @brief Performs one of various miscellaneous operations.
 *
 * @param cmd The operation to be performed.
 * 
 * @returns The command result or 0, or -1 if an error occurred and a specific 
 *          error code can be retrieved by calling Socket::GetLastError(). 
 */
int Socket::fcntl(int cmd) {
    int ret;

    ret = ::fcntl(mSocket, cmd);

    return ret;
}

/**
 * @brief Performs one of various miscellaneous operations.
 *
 * @param cmd  The operation to be performed.
 * @param arg  Argument parameter.
 * 
 * @returns The command result or 0, or -1 if an error occurred and a specific 
 *          error code can be retrieved by calling Socket::GetLastError(). 
 */
int Socket::fcntl(int cmd, long arg) {
    int ret;

    ret = ::fcntl(mSocket, cmd, arg);

    return ret;
}

/**
 * @brief Performs one of various miscellaneous operations.
 *
 * @param cmd   The operation to be performed.
 * @param lock  Argument parameter.
 *
 * @returns The command result or 0, or -1 if an error occurred and a specific 
 *          error code can be retrieved by calling Socket::GetLastError(). 
 */
int Socket::fcntl(int cmd, struct flock &lock) {
    int ret;

    ret = ::fcntl(mSocket, cmd, &lock);

    return ret;
}

/**
 * @brief Sends data to a connected socket.
 *
 * @param buf Buffer to be sent.
 * @param len Buffer length.
 * @param flags The flags parameter is the bitwise OR of zero or more of the following flags:
 *        MSG_OOB, MSG_EOR, MSG_DONTROUTE, MSG_DONTWAIT, MSG_NOSIGNAL, MSG_CONFIRM.
 *
 * @returns The number of characters sent, or -1 if an error occurred and 
 *          a specific error code can be retrieved by calling Socket::GetLastError().
 */
int Socket::send(const void *buf, unsigned int len, int flags) {
    ssize_t ret;

    ret = ::send(mSocket, buf, (size_t) len, flags);

    return (int) ret;
}

/**
 * @brief Receives data from the socket.
 *
 * @param buf    Buffer to receive data.
 * @param len    Buffer length.
 * @param flags  The flags parameter is the bitwise OR of zero or more of the following flags: 
 *               MSG_OOB, MSG_PEEK, MSG_WAITALL, MSG_TRUNC, MSG_ERRQUEUE.
 *
 * @returns The number of bytes received, or -1 if an error occurred and a 
 *          specific error code can be retrieved by calling Socket::GetLastError().
 *          The return value will be 0 when the peer has performed an orderly shutdown.
 */
int Socket::recv(void *buf, unsigned int len, int flags) {
    ssize_t ret;

    ret = ::recv(mSocket, buf, (size_t) len, flags);

    return (int) ret;
}

/**
 * @bried Sends "all" data to a connected socket.
 *
 * @param buf      Buffer to be sent.
 * @param len      Buffer length.
 * @param timeout  Timeout in milliseconds for each write() call.
 *
 * @returns The number of bytes sent, -2 if timeout occured, -1 if an error occurred and a 
 *          specific error code can be retrieved by calling Socket::GetLastError().
 */
int Socket::writen(const void *buf, unsigned int len, int timeout) {
    struct pollfd pfd; // structure to poll()
    unsigned int wrtd = 0; // total data written
    unsigned int wnow = 0; // partial data written
    ssize_t ret = len;

    // set poll events
    pfd.fd = mSocket;
    pfd.events = POLLOUT;
    pfd.revents = 0;

    // try to write all data requested
    while ((wrtd < len) && (ret > 0)) {
        int r = ::poll(&pfd, 1, timeout);
        if (r > 0) // poll OK
        {
            wnow = ::write(mSocket, (char *) buf + wrtd, len - wrtd);
            if (wnow > 0) {
                wrtd += wnow; // update number of bytes sent
                ret = wrtd; // data written OK
            } else {
                ret = wnow; // some error ocurred (-1: error;  0: no data sent)
            }
        } else if (r == 0) // poll timeout
        {
            ret = -2;
        } else // poll error
        {
            ret = r;
        }
    }

    return (int) ret;
}

/**
 * @brief Receives "all" data from the socket.
 *
 * @param buf      Buffer to receive data.
 * @param len      Buffer length.
 * @param timeout  Timeout in milliseconds for each read() call.
 *
 * @returns The number of bytes read, -2 if timeout occured, -1 if an error occurred 
 *          and a specific error code can be retrieved by calling Socket::GetLastError(). 
 *          The return value will be 0 when the peer has performed an orderly shutdown.
 */
int Socket::readn(void *buf, unsigned int len, int timeout) {
    struct pollfd pfd; // structure to poll()
    unsigned int recvd = 0; // total data read
    unsigned int rnow = 0; // partial data read
    ssize_t ret = len;

    // set poll events
    pfd.fd = mSocket;
    pfd.events = POLLIN | POLLPRI;
    pfd.revents = 0;

    // try to read all data requested
    while ((recvd < len) && (ret > 0)) {
        int r = ::poll(&pfd, 1, timeout);
        if (r > 0) // poll OK
        {
            rnow = ::read(mSocket, (char *) buf + recvd, len - recvd);
            if (rnow > 0) {
                recvd += rnow;
                ret = recvd; // data read OK
            } else {
                ret = rnow; // some error ocurred (-1: error;  0: connection closed by peer)
            }
        } else if (r == 0) // poll timeout
        {
            ret = -2;
        } else if (errno == EINTR) // interruped
        {
            ret = 1;
        } else // poll error
        {
            ret = r;
        }
    }

    return (int) ret;
}

int Socket::read(void *buf, unsigned int len, int timeout) {
    struct pollfd pfd; // structure to poll()
    unsigned int recvd = 0; // total data read
    unsigned int rnow = 0; // partial data read
    int ret = 0;

    // set poll events
    pfd.fd = mSocket;
    pfd.events = POLLIN | POLLPRI;
    pfd.revents = 0;

    // try to read all data requested
    int r = ::poll(&pfd, 1, timeout);
    if (r > 0) // poll OK
    {
        ret = ::read(mSocket, (char *) buf, len);
    }else if (r == 0) // poll timeout
    {
        ret = -2;
    }else // poll error
    {
        ret = r;
    }

    return ret;
}

/**
 * @brief Gets SCTP level options on a socket.
 *
 * @param opt      Specifes the  SCTP socket option to get.
 * @param arg      Buffer, normally a structure.
 * @param size     Contains the size of the buffer.
 * @param assocId  Specifies the association ID.
 *
 * @returns true if the function is successful; otherwise false, and a
 *          specific error code can be retrieved by calling Socket::GetLastError().
 */
bool Socket::sctp_opt_info(int opt, void *arg, unsigned int &size, unsigned int assocId) {
    bool bRet = false;

    if (::sctp_opt_info(mSocket, (sctp_assoc_t) assocId, opt, arg, (socklen_t *) & size) != -1)
        bRet = true;
    else
        bRet = false;

    return bRet;
}

/**
 * @brief Enables or Disables the SCTP's Nagle algorithm.
 *
 * @param enable true disables SCTP's Nagle algorithm. false enables SCTP's Nagle algorithm
 *
 * @returns true if the function is successful; otherwise false, and a
 *          specific error code can be retrieved by calling Socket::GetLastError().
 */
bool Socket::setSctpNoDelay(bool enable) {
    bool bRet = false;
    int optval;

    optval = enable ? 1 : 0;

    if (::setsockopt(mSocket, IPPROTO_SCTP, SCTP_NODELAY, (const void *) & optval, sizeof (optval)) != -1)
        bRet = true;
    else
        bRet = false;

    return bRet;
}

/**
 * @brief Sends a message from a socket while using the advanced features of SCTP.
 *
 * @param msg          Message to be sent.
 * @param len          Message length.
 * @param stream       Identifies the stream number.
 * @param ppid         Holds the pay-load protocol identifier that will be passed with the data chunk
 * @param flags        Composed of a bitwise OR of the following values: MSG_UNORDERED, MSG_ADDR_OVER, MSG_ABORT M,SG_EOF.
 * @param timetolive   Specifies  the time duration in milliseconds, where 0 represents an infinite lifetime.
 * @param context      An value that is passed back to the upper layer along with the undelivered message
 *                     if an error occurs on the send of the message.
 * 
 * @returns The number of characters sent, or -1 if an error occurred and 
 *          a specific error code can be retrieved by calling Socket::GetLastError().
 */
int Socket::sctp_sendmsg(const void *msg, unsigned int len, unsigned short stream, unsigned int ppid,
        unsigned int flags, unsigned int timetolive, unsigned int context) {
    int ret;

    ret = ::sctp_sendmsg(mSocket, msg, (size_t) len, (struct sockaddr *) NULL, (socklen_t) 0,
            (uint32_t) ppid, (uint32_t) flags, (uint16_t) stream, (uint32_t) timetolive,
            (uint32_t) context);

    return ret;
}

/**
 * @brief Receives a message from a socket while using the advanced features of SCTP.
 *
 * @param msg        Buffer to receive message.
 * @param len        Message length.
 * @param sinfo      A sctp_sndrcvinfo structure to be filled upon receipt of the message.
 * @param msg_flags  A value that is filled with any message flags like MSG_NOTIFICATION or MSG_EOR.
 * 
 * @returns the number of characters received, or -1 if an error occurred and 
 *          a specific error code can be retrieved by calling Socket::GetLastError().
 */
int Socket::sctp_recvmsg(void *msg, unsigned int len, struct sctp_sndrcvinfo &sinfo, int &msg_flags) {
    int ret;

    ret = ::sctp_recvmsg(mSocket, msg, (size_t) len, (struct sockaddr *) NULL, 0, &sinfo, &msg_flags);

    return ret;
}

/**
 * @brief Gets the error status for the last operation that failed.
 *
 * @returns Value indicating the error code (errno) for the last sockets API routine performed by this thread.
 */
int Socket::getLastError() {
    return errno;
}

/**
 * @brief Gets the error status for the last operation that failed as a string.
 *
 * @returns String indicating the error string for the last sockets API routine performed by this thread.
 */
std::string Socket::getLastErrorMsg() {
    std::string s;

    s = ::strerror(errno);

    return s;
}

