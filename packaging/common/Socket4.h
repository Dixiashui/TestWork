#ifndef _Socket4_h
#define _Socket4_h

/**
 * @file Socket4.h
 * @brief Defines the Socket4 class.
 */

#include "Socket.h"

/**
 * @brief Socket4 class is a wrapper for Linux socket IPv4.
 *
 * This class is derived from Socket class and inplements specific 
 * IPv4 services.
 *
 * @author Alexandre Maia Godoi <alexandre.maia@siemens.com>
 */
class Socket4 : public Socket {
public:
    Socket4();
    virtual ~Socket4();

    virtual bool create(int type, int protocol = 0);
    virtual bool bind(unsigned short port, const char *addr = NULL);
    virtual bool connect(const char *addr, unsigned short port);

    virtual bool getSockname(std::string &addr, unsigned short &port);
    virtual bool getPeername(std::string &addr, unsigned short &port);

    virtual bool sctp_bindx(const VectorString &vetAddrs, unsigned short port, int flags);
    virtual bool sctp_getladdrs(VectorString &vetAddrs, unsigned short &port, unsigned int assocId = 0);
    virtual bool sctp_getpaddrs(VectorString &vetAddrs, unsigned short &port, unsigned int assocId = 0);

    virtual int sctp_sendmsg(const void *msg, unsigned int len, unsigned short stream,
            const char *addr, unsigned short port, unsigned int ppid = 0,
            unsigned int flags = 0, unsigned int timetolive = 0, unsigned int context = 0);
    virtual int sctp_recvmsg(void *msg, unsigned int len, struct sctp_sndrcvinfo &sinfo, int &msg_flags,
            std::string &addr, unsigned short &port);

private:
    bool fillSockaddr(unsigned short port, const char *addr, struct sockaddr_in &socketaddr);

    bool getAddr(const struct sockaddr_in &socketaddr, std::string &addr, unsigned short &port);
    bool getAddrs(const struct sockaddr_in socketaddr[], int size, VectorString &vetAddrs, unsigned short &port);
};

#endif // ifndef _Socket4_h

