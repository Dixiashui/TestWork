#include "common/Socket.h"
#include "common/ServerSocket.h"
#include <sstream>
#include <string>

/**
 * @brief ServerSocket constructor.
 *
 * @param sockServer Socket server. Can be Socket4 or Socket6.
 * @param port       The local TCP port.
 * @param addr       The local address the server will bind to.
 *
 * Creates a TCP server socket with the specified port and local 
 * IP address to bind to. 
 */
ServerSocket::ServerSocket(Socket &sockServer, unsigned short port, const char *addr) throw (std::string) : mServer(sockServer) {
    std::ostringstream os;
    std::string s = (addr == NULL) ? "0.0.0.0" : addr;

    os << "Cannot create server socket. Address: " << s << "  Port: " << port;

    if (!mServer.create(SOCK_STREAM)) {
        throw os.str();
    }

    if (!mServer.setNonBlockMode()) {
        throw os.str();
    }

    if (!mServer.setReuseAddr()) {
        throw os.str();
    }

    if (!mServer.bind(port, addr)) {
        throw os.str();
    }

    if (!mServer.listen()) {
        throw os.str();
    }
}

/**
 * @brief ServerSocket destructor.
 */
ServerSocket::~ServerSocket() {
    mServer.close();
}

/**
 * @brief Listens for a connection to be made to this socket and accepts it.
 *
 * Listens for a connection to be made to this socket and accepts it. 
 * The method blocks until timeout is elapsed or a socket arrives. 
 * A new Socket4 is created.
 *
 * @param sockClient Accepted socket just created. Can be Socket4 or Socket6, 
 *                   it is Socket Server dependent.
 * @param timeout    Timeout to be reached.
 *
 * @returns true if the function is successful; otherwise false.
 */
bool ServerSocket::accept(Socket &sockClient, int timeout) {
    struct pollfd pfd; // poll structure
    bool bRet = false;

    // set poll events
    pfd.fd = mServer.getSocket();
    pfd.events = POLLIN | POLLPRI;
    pfd.revents = 0;

    int r = ::poll(&pfd, 1, timeout);
    if (r == -1) {
        // poll error
        bRet = false;
    } else if (r == 0) {
        // poll timeout
        bRet = false;
    } else {
        // accept new client socket
        bRet = mServer.accept(sockClient);
    }

    return bRet;
}

