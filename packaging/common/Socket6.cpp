#include "Socket6.h"
#include <iostream>
#include <stdio.h>

/**
 * @brief Socket6 constructor
 */
Socket6::Socket6()
{
}

/**
 * @brief Socket6 destructor
 */
Socket6::~Socket6()
{
    close();
}

/**
 * @brief Creates the IPV6 socket and attach it.
 *
 * @param type     Specifies the communication semantics.  Currently defined types are (more than this values are allowed):
 *                 SOCK_STREAM, SOCK_DGRAM, SOCK_SEQPACKET.
 * @param protocol Specifies a particular protocol to be used with the socket. Usually zero (0) is used to select the
 *                 default protocol. IPPROTO_SCTP is used for SCTP protocol.
 *
 * @returns true if the function is successful; otherwise false, and a specific
 *          error code can be retrieved by calling Socket::GetLastError().
 */
bool Socket6::create(int type, int protocol)
{
    bool bRet = false;

    if (mSocket == -1)
    {
        if ( (mSocket = ::socket(AF_INET6, type, protocol)) != -1 )
            bRet = true;
        else
            bRet = false;
    }
    else
    {
        errno = EBUSY;        // Device or resource busy
        bRet = false;
    }
    
    if(bRet)
    {  
        bRet = this->setReuseAddr();
    }

    return bRet;
}

/**
 * @brief Associates a local address with the socket.
 *
 * @param port  The port identifying the socket application. Zero (0) allows Linux to choose an ephemeral port.
 * @param addr  The network address in any allowed IPv6 address format. NULL binds with any interface.
 *
 * @returns true if the function is successful; otherwise false, and a specific
 *          error code can be retrieved by calling Socket::GetLastError().
 */
bool Socket6::bind(unsigned short port, const char *addr)
{
    struct sockaddr_in6 socketaddr;
    bool bRet = false;

    // fill sockaddr_in6 structure
    bRet = fillSockaddr(port, addr, socketaddr);

    // bind
    if (bRet)
    {
        if ( ::bind(mSocket, (struct sockaddr *)&socketaddr, sizeof(socketaddr)) != -1 )
            bRet = true;
        else
            bRet = false;
    }

    return bRet;
}

/**
 * @brief Establishes a connection to a peer socket.
 *
 * @param addr  The network address of the socket to which this object is connected in any allowed IPv6 address format..
 * @param port  The port identifying the socket application.
 *
 * @returns true if the function is successful; otherwise false, and a specific
 *          error code can be retrieved by calling Socket::GetLastError().
 */
bool Socket6::connect(const char *addr, unsigned short port)
{
    struct sockaddr_in6 socketaddr;
    bool bRet = false;

    // fill sockaddr_in6 structure
    bRet = fillSockaddr(port, addr, socketaddr);

    // connect
    if (bRet)
    {
        if (::connect(mSocket, (struct sockaddr *)&socketaddr, sizeof(socketaddr)) != -1)
            bRet = true;
        else
            bRet = false;
    }

    return bRet;
}

bool Socket6::connect(const char* addr, unsigned short port, int timeout)
{
    struct sockaddr_in6 socketaddr;
    if (!fillSockaddr(port, addr, socketaddr))
        return false;

    if (!this->setNonBlockMode())
        return false;
    
    bool bRet = false;

    if (::connect(mSocket, (struct sockaddr *) &socketaddr, sizeof (socketaddr)) == 0)
    {
        return this->setNonBlockMode(false);
    }
    
    if (errno == EINPROGRESS)
    {
        timeval tm;
        tm.tv_sec = timeout / 1000;
        tm.tv_usec = (timeout % 1000) * 1000;
        fd_set set;
        FD_ZERO(&set);
        FD_SET(mSocket, &set);
        while (tm.tv_sec > 0 || tm.tv_usec > 0)
        {
            //actually select function is always return right away when the
            //the other side is not ready for connection, 'cause a RST message
            //will always be received in this case, so this select function
            //will not wait until tm expired to return. It returns almost right 
            //away, and then the value of tm will be modified to the timeout left
            if (select(mSocket + 1, NULL, &set, NULL, &tm) > 0)
            {
                socklen_t len;
                int error;
                if (!this->getSockopt(SOL_SOCKET, SO_ERROR, &error, (socklen_t *) & len))
                {
                    continue;
                }
                if (error == 0)
                {
                    bRet = this->setNonBlockMode(false);
                    break;
                }
            }
        }
    }
    
    return bRet;    
}

bool Socket6::sctp_connectx(const VectorString &vetAddrs, unsigned short port)
{
    sockaddr_in6 *addrs = NULL;
    int size  = vetAddrs.size();
    bool bRet = true;

    addrs = new sockaddr_in6[size];

    // fill address structures
    for (int i=0; i<size && bRet; i++)
        bRet = fillSockaddr(port, vetAddrs[i].c_str(), addrs[i]);

    // connect
    if (bRet)
    {
        if (::sctp_connectx(mSocket, (sockaddr *)addrs, size, NULL) != -1)
            bRet = true;
        else
            bRet = false;
    }
    
    delete[] addrs;

    return bRet;
}

/**
 * @brief Gets the local name and port for a socket.
 *
 * @param addr Any allowed IPv6 address format.
 * @param port Port number.
 *
 * @returns true if the function is successful; otherwise false, and a specific
 *          error code can be retrieved by calling Socket::GetLastError().
 */
bool Socket6::getSockname(std::string &addr, unsigned short &port)
{
    struct sockaddr_in6 socketaddr;
    socklen_t len = sizeof(socketaddr);
    bool bRet = false;

    ::bzero(&socketaddr, sizeof(socketaddr));

    if (::getsockname(mSocket, (struct sockaddr *)&socketaddr, &len) != -1)
        bRet = getAddr(socketaddr, addr, port);
    else
        bRet = false;

    return bRet;
}

/**
 * @brief Gets the name and port of the peer socket to which the socket is connected.
 *
 * @param addr  Any allowed IPv6 address format.
 * @param port  Port number.
 *
 * @returns true if the function is successful; otherwise false, and a specific
 *          error code can be retrieved by calling Socket::GetLastError().
 */
bool Socket6::getPeername(std::string &addr, unsigned short &port)
{
    struct sockaddr_in6 socketaddr;
    socklen_t len = sizeof(socketaddr);
    bool bRet = false;

    ::bzero(&socketaddr, sizeof(socketaddr));

    if (::getpeername(mSocket, (struct sockaddr *)&socketaddr, &len) != -1)
        bRet = getAddr(socketaddr, addr, port);
    else
        bRet = false;

    return bRet;
}

/**
 * @brief Associates "n" local addresses with the socket.
 *
 * @param vetAddrs  The network addresses. A vector filled with any allowed IPv6 address format.
 * @param port      The port identifying the socket application.
 * @param flags     The  flags parameter can be either SCTP_BINDX_ADD_ADDR or SCTP_BINDX_REM_ADDR.
 *
 * @returns true if the function is successful; otherwise false, and a specific
 *          error code can be retrieved by calling Socket::GetLastError().
 */
bool Socket6::sctp_bindx(const VectorString &vetAddrs, unsigned short port, int flags)
{
    sockaddr_in6 *addrs = NULL;
    int size  = vetAddrs.size();
    bool bRet = true;

    addrs = new sockaddr_in6[size];

    // fill address structures
    for (int i=0; i<size && bRet; i++)
        bRet = fillSockaddr(port, vetAddrs[i].c_str(), addrs[i]);

    if (bRet)
    {
        if (::sctp_bindx(mSocket, (sockaddr *)addrs, size, flags) != -1)
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
bool Socket6::sctp_getladdrs(VectorString &vetAddrs, unsigned short &port, unsigned int assocId)
{
    struct sockaddr *addrs = NULL;
    int count;
    bool bRet = false;

    if ( (count = ::sctp_getladdrs(mSocket, (sctp_assoc_t)assocId, &addrs)) > 0 )
        bRet = getAddrs((struct sockaddr_in6 *)addrs, count, vetAddrs, port);
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
bool Socket6::sctp_getpaddrs(VectorString &vetAddrs, unsigned short &port, unsigned int assocId)
{
    struct sockaddr *addrs = NULL;
    int count;
    bool bRet = false;

    if ( (count = ::sctp_getpaddrs(mSocket, (sctp_assoc_t)assocId, &addrs)) > 0 )
        bRet = getAddrs((struct sockaddr_in6 *)addrs, count, vetAddrs, port);
    else
        bRet = false;

    // free memory allocated by sctp_getpaddrs() call
    if (addrs != NULL)
        ::sctp_freepaddrs(addrs);

    return bRet;
}

/**
 * @brief Compares to IPv6 addresses.
 *
 * @param addr1  Address to be compared.
 * @param addr2  Address to be compared.
 *
 * @returns true if addresses are equals, otherwise false.
 */
bool Socket6::addrs_cmp(const std::string &addr1, const std::string &addr2)
{
    struct sockaddr_in6 addr_st1, addr_st2;
    
    bool bRet = false;

    ::bzero(&addr_st1, sizeof(addr_st1));
    ::bzero(&addr_st2, sizeof(addr_st2));

    if ( ::inet_pton(AF_INET6, addr1.c_str(), &addr_st1.sin6_addr) > 0 && ::inet_pton(AF_INET6, addr2.c_str(), &addr_st2.sin6_addr) > 0)
    {
        if(!memcmp((void*)&addr_st1.sin6_addr, (void*)&addr_st2.sin6_addr, sizeof(addr_st1.sin6_addr)))
            bRet = true;
        else
            bRet = false;
    }
    else
    {
        bRet = false;
    }

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
int Socket6::sctp_sendmsg(const void *msg, unsigned int len, unsigned short stream,
                          const char *addr, unsigned short port, unsigned int ppid,
                          unsigned int flags, unsigned int timetolive, unsigned int context)
{
    int ret;
    struct sockaddr_in6 socketaddr;

    if(addr != NULL)
    {
        if (fillSockaddr(port, addr, socketaddr)) // change this to fill scope_id related to the local ip address.
        {
            ret = ::sctp_sendmsg(mSocket, msg, (size_t)len, (struct sockaddr *)&socketaddr, (socklen_t)sizeof(socketaddr),
                    (uint32_t)ppid, (uint32_t)flags, (uint16_t)stream, (uint32_t)timetolive,
                    (uint32_t)context);
        }
        else
        {
            errno = EINVAL; // Invalid argument
            ret = -1;
        }
    }
    else
    {
        ret = ::sctp_sendmsg(mSocket, msg, (size_t)len, (struct sockaddr *)NULL, 0,
                (uint32_t)ppid, (uint32_t)flags, (uint16_t)stream, (uint32_t)timetolive,
                (uint32_t)context);        
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
int Socket6::sctp_recvmsg(void *msg, unsigned int len, struct sctp_sndrcvinfo &sinfo, int &msg_flags,
                          std::string &addr, unsigned short &port)
{
    int ret;
    struct sockaddr_in6 socketaddr;
    socklen_t fromlen = sizeof(socketaddr);

    ret = ::sctp_recvmsg(mSocket, msg, (size_t)len, (struct sockaddr *)&socketaddr, &fromlen,
                         &sinfo, &msg_flags);

    if (ret > 0)
    {
        if (!getAddr(socketaddr, addr, port))
        {
            errno = EADDRNOTAVAIL; // Cannot assign requested address
            ret = -1;              // invalid address
        }
    }

    return ret;
}

/**
 * @brief Fills sockaddr_in6 structure with port and address.
 *
 * @param port        Port number.
 * @param addr        Address.
 * @param socketaddr  sockaddr_in6 structure.
 *
 * @returns true if sockaddr_in6 structure was filled, otherwise false.
 */
bool Socket6::fillSockaddr(unsigned short port, const char *addr, struct sockaddr_in6 &socketaddr)
{
    bool bRet = false;
    int r;

    ::bzero(&socketaddr, sizeof(socketaddr));

    if (addr == NULL)
    {
        socketaddr.sin6_addr  = in6addr_any;
        socketaddr.sin6_family = AF_INET6;
        socketaddr.sin6_port = ::htons(port);
        bRet = true;
    }
    else
    {
        if ( (r = ::inet_pton(AF_INET6, addr, &socketaddr.sin6_addr)) > 0 )
        {
            socketaddr.sin6_family = AF_INET6;
            socketaddr.sin6_port = ::htons(port);
	       //socketaddr.sin6_scope_id = if_nametoindex("eth0");
	       //bool r = fillScopeId(addr,socketaddr);
	       fillScopeId(addr,socketaddr);
           bRet = true;
        }
        else if (r == 0)
        {
            errno = EINVAL; // invalid character string representing a valid network address
            bRet = false;
        }
        else
        {
            bRet = false;
        }
    }

    return bRet;
}

/**
 */
bool Socket6::fillScopeId(const char *addr, struct sockaddr_in6 &socketaddr)
{
    bool bRet = false;
    struct ifaddrs *l, *c;
    char* name = NULL;
	
    if (getifaddrs(&l) == -1)
	   return false;
	
    for (c = l; c != NULL; c = c->ifa_next) 
    {
	   //if (c->ifa_addr == NULL || c->ifa_addr->sa_family != AF_LINK)
	   if (c->ifa_addr == NULL || c->ifa_addr->sa_family != AF_INET6)
		  continue;
    
	   std::string addrToCmp;
	   unsigned short num;
	   getAddr(*((sockaddr_in6*)c->ifa_addr),addrToCmp,num);
	   //if(addrToCmp == (std::string)addr)
           if(!memcmp((void*)&socketaddr.sin6_addr, (void*)&((sockaddr_in6*)c->ifa_addr)->sin6_addr, sizeof(socketaddr.sin6_addr)))
           {
		  name = strdup(c->ifa_name);
		  socketaddr.sin6_scope_id = if_nametoindex(name);
		  bRet = true;
		  break;
	   }
    }		

    return bRet;    
}

/**
 * @brief Gets address and port number from sockaddr_in6 structure.
 *
 * @param socketaddr  sockaddr_in6 structure.
 * @param addr        Address.
 * @param port        Port Number.
 *
 * @return true if values were gotten, otherwise false.
 */
bool Socket6::getAddr(const struct sockaddr_in6 &socketaddr, std::string &addr, unsigned short &port)
{
    char dst[INET6_ADDRSTRLEN];
    bool bRet = false;

    if (::inet_ntop(AF_INET6, &socketaddr.sin6_addr, dst, sizeof(dst)) != NULL)
    {
        addr = dst;
        port = ::ntohs(socketaddr.sin6_port);
        bRet = true;
    }
    else
        bRet = false;

    return bRet;
}

/**
 * @brief Gets addresses and port number from an array of sockaddr_ini structures.
 *
 * @param socketaddr  Array of sockaddr_ini structures.
 * @param size        Size of the array.
 * @param vetAddrs    Adresses.
 * @param port        Port number.
 *
 * @return true if values were gotten, otherwise false.
 */
bool Socket6::getAddrs(const struct sockaddr_in6 socketaddr[], int size, VectorString &vetAddrs,
                       unsigned short &port)
{
    char dst[INET6_ADDRSTRLEN];
    bool bRet = true;

    // allocate vector positions
    vetAddrs.reserve(size);

    // get remote port
    port = ::ntohs(socketaddr[0].sin6_port);

    // get remote host names
    for (int i=0; i<size && bRet; i++)
    {
        if (::inet_ntop(AF_INET6, &socketaddr[i].sin6_addr, dst, sizeof(dst)) != NULL)
            vetAddrs.push_back(dst);
        else
            bRet = false; // invalid address
    }

    if (!bRet)
        vetAddrs.clear();

    return bRet;
}

/**
 * @brief Sends data to a remote address.
 * 
 * @param msg Buffer with data to send.
 * @param size Buffer size.
 * @param addr Remote address.
 * @param port Remote port.
 *
 * @returns 0 if successed. 
 */
int Socket6::udp_send_to(void *msg, int size, const char *addr, unsigned short port)
{
	struct sockaddr_in6 their_addr; 
	fillSockaddr(port, addr, their_addr);
	
	return sendto(mSocket, msg, size, 0, (struct sockaddr*) &their_addr, sizeof(struct sockaddr_in6));
}

/**
 * @brief Receives data from a remote address.
 * 
 * @param msg Buffer to store received data.
 * @param size Buffer size.
 * @param addr Remote address.
 * @param port Remote port.
 *
 * @returns 0 if successed. 
 */
int Socket6::udp_receive_from(void *buf, int size, std::string &addr, unsigned short &port)
{
	int ret;
	int addr_len = sizeof(struct sockaddr_in6); 
	struct sockaddr_in6 their_addr; 
	
	ret = recvfrom(mSocket, buf, size, 0, (struct sockaddr *) &their_addr, (socklen_t *) &addr_len);  
	
	if(ret>0)
	{
		getAddr(their_addr, addr, port);
	}
	
	return ret;
} 
		
