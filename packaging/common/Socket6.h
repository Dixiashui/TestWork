#ifndef _Socket6_h
#define _Socket6_h

/**
 * @file Socket6.h
 * @brief Defines the Socket6 class.
 */


#include "Socket.h"
#include <net/if.h>
#include <ifaddrs.h>

/**
 * @brief Socket6 class is a wrapper for Linux socket IPv6.
 *
 * This class is derived from Socket class and inplements specific
 * IPv4 services.
 *
 * @author Alexandre Maia Godoi <alexandre.maia@siemens.com>
 */
class Socket6 : public Socket
{
public:
    Socket6();
    virtual ~Socket6();

    virtual bool create(int type, int protocol=0);
    virtual bool bind(unsigned short port, const char *addr=NULL);
    virtual bool connect(const char *addr, unsigned short port);
    bool connect(const char *addr, unsigned short port, int timeout);
    virtual bool sctp_connectx(const VectorString &vetAddrs, unsigned short port);


    virtual bool getSockname(std::string &addr, unsigned short &port);
    virtual bool getPeername(std::string &addr, unsigned short &port);

    virtual bool sctp_bindx(const VectorString &vetAddrs, unsigned short port, int flags);

    virtual bool sctp_getladdrs(VectorString &vetAddrs, unsigned short &port, unsigned int assocId=0);
    virtual bool sctp_getpaddrs(VectorString &vetAddrs, unsigned short &port, unsigned int assocId=0);

    virtual int sctp_sendmsg(const void *msg, unsigned int len, unsigned short stream, 
                             const char *addr, unsigned short port, unsigned int ppid=0,
                             unsigned int flags=0, unsigned int timetolive=0, unsigned int context=0);
    virtual int sctp_recvmsg(void *msg, unsigned int len, struct sctp_sndrcvinfo &sinfo, int &msg_flags,
                             std::string &addr, unsigned short &port);
    static bool addrs_cmp(const std::string &addr1, const std::string &addr2);
    
    int udp_send_to(void * msg, int size, const char *addr, unsigned short port);
    int udp_receive_from(void *buf, int size, std::string &addr, unsigned short &port);
	
private:
    bool fillSockaddr(unsigned short port, const char *addr, struct sockaddr_in6 &socketaddr);
    bool fillScopeId(const char *addr, struct sockaddr_in6 &socketaddr);

    bool getAddr(const struct sockaddr_in6 &socketaddr, std::string &addr, unsigned short &port);
    bool getAddrs(const struct sockaddr_in6 socketaddr[], int size, VectorString &vetAddrs, unsigned short &port);
};

#endif // ifndef _Socket6_h

