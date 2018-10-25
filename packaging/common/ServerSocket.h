#ifndef _SERVERSOCKET_H
#define _SERVERSOCKET_H

#include <string>

/**
 * @file ServerSocket.h
 * @brief Defines the ServerSocket class.
 */

class Socket;

/**
 * @brief ServerSocket class is used to accept incoming client sockets.
 *
 * @author Alexandre Maia Godoi <alexandre.maia@siemens.com>
 */
class ServerSocket {
public:
    ServerSocket(Socket &sockServer, unsigned short port, const char *addr = NULL) throw (std::string);
    ~ServerSocket();

    bool accept(Socket &sockClient, int timeout);

private:
    Socket &mServer; ///< Server socket.
};

#endif // ifndef _SERVERSOCKET_H

