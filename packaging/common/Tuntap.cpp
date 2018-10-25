#include <iostream>
#include <string.h>
#include <stdlib.h>
#include <sstream>
#include <stdio.h>
#include <net/if.h>
#include <net/if_arp.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
//#include <linux/if_tun.h>
#include "if_tun.h"
#include "Tuntap.h"


using std::ostringstream;


/**
 * @brief Constructor for a tuntap device object.
 *
 * @param name Name for the device
 */
Tuntap::Tuntap(const char *name)
{

    mSocket = init_tundev(name, mName, mAddr);
}

/**
 * @brief Destructor for the tuntap device object.
 *
 * Closes the tuntap device.
 */
Tuntap::~Tuntap()
{
    if(mSocket >= 0)
        close(mSocket);
}
 
/**
 * @brief Gets the TUNTAP error as string
 *
 * @param error  Error code.
 */
string Tuntap::getError(int error)
{
    string s;

    switch (error)
    {
        case TUNTAP_CANT_OPEN_DEV:
            s = "cannot open TUNTAP device. Try run it as root!";
            break;

        case TUNTAP_CANT_SET_OPTIONS:
            s = "cannot set TUNTAP options";
            break;

        case TUNTAP_CANT_SET_TYPE:
            s = "cannot set TUNTAP options";
            break;

        case TUNTAP_BAD_NAME:
            s = "invalid TUNTAP name";
            break;

        case TUNTAP_CLOSED:
            s = "TUNTAP is closed";
            break;

        case TUNTAP_IFCONFIG_FAILURE:
            s = "ifconfig failure";
            break;

        default:
            s = "";
    }

    return s;
}

/**
 * @brief Initializes a tuntap device.
 *
 * @param name [Input] name for the device.
 *
 * @param dev_name [Output] name for the device.
 *
 * @param addr [Output] address of the device.
 *
 * @returns A negative value on error (error list in Tuntap.h). The socket if the init was ok.
 */
int Tuntap::init_tundev(const char *name, string & dev_name, string & addr)
{   
    // sanity checks
    if(strlen(name) >= IFNAMSIZ || name == NULL || *name == '\0')
        return TUNTAP_BAD_NAME;
    
    // opens up the tuntap device
    int fd;
    if((fd = open("/dev/net/tun", O_RDWR)) < 0)
    {
        close(fd);
        return TUNTAP_CANT_OPEN_DEV;
    }
 
    // sets the tuntap options
    struct ifreq ifr;
    memset(&ifr, 0, sizeof(ifr));
    ifr.ifr_flags = IFF_TUN;// | IFF_NO_PI; // TODO: remove IFF_NO_PI if the packets sent to this interface get an information header
    strncpy(ifr.ifr_name, name, IFNAMSIZ);

    if(ioctl(fd, TUNSETIFF, (void *)&ifr) < 0)
    {
        close(fd);
        return TUNTAP_CANT_SET_OPTIONS;
    }

    // sets the tuntap type
    if(ioctl(fd, TUNSETLINK, 19 /*ARPHRD_ATM*/) < 0)
    {
        close(fd);
        return TUNTAP_CANT_SET_TYPE;
    }
    
    // builds the interface address
    dev_name = ifr.ifr_name;
    
    unsigned int hash = 0, hash2 = 0;
    for(unsigned int i = 0; i < dev_name.size(); i++)
        hash += ((dev_name[i] * 78488051) >> 8) ^ 78488051;

    hash2 = (hash ^ 0x90) >> 1;

    ostringstream addr_buf;
    addr_buf << "78.48." << hash2 % 256 << "." << hash % 256;
    addr = addr_buf.str();
    
    // brings up the tun interface with the specified IP (the output must be ignored)
    dev_name = ifr.ifr_name;
    ostringstream cmd;
    cmd << "/sbin/ifconfig " << dev_name << " up " << addr << " pointopoint 43.43.43.43 " << " 2> /dev/null";
    if(system(cmd.str().c_str()) != 0)
    {
        close(fd);
        return TUNTAP_IFCONFIG_FAILURE;
    }
    
    return fd;
}


