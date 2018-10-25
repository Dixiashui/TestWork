#ifndef TUNTAP_H_INCLUDED
#define TUNTAP_H_INCLUDED

#include <string>
using std::string;


/// errors which can be returned by getDeviceSocket on socket creation
#define TUNTAP_CANT_OPEN_DEV     -100  /// Error given when the device cannot be opened
#define TUNTAP_CANT_SET_OPTIONS  -101  /// Error given when the device options cant be set
#define TUNTAP_CANT_SET_TYPE     -102  /// Error given when the device type cant be set
#define TUNTAP_BAD_NAME          -103  /// Error given when the device name exceeds 15 bytes
#define TUNTAP_CLOSED            -104  /// Error given when the socket was closed
#define TUNTAP_IFCONFIG_FAILURE  -105  /// Error given when the system call to ifconfig fails


/**
 * @brief This class implements a Tuntap wrapper.
 * 
 * This wrapper will create a tuntap tunnel with a given interface name.
 *
 * @author Daniel Cavalcanti Jeronymo <daniel.jeronymo.ext@siemens.com>
 */
class Tuntap
{
private:
    string mName;       ///< Name of the tuntap device
    string mAddr;       ///< Address (in IPV4 format) of the device
    int mSocket;        ///< Socket (positive value) or error id (negative value)
    
public:
    Tuntap(const char *name);
    ~Tuntap();
    
    string getDeviceName();
    string getDeviceAddress();
    int getDeviceSocket();
    string getError(int error);
    
    int read(void *buffer, unsigned int size);
    int write(void *buffer, unsigned int size);

private:
    int init_tundev(const char *name, string & dev_name, string & addr);
};

/**
 * @brief Returns the device name.
 *
 * @returns A string with the device name.
 */
inline string Tuntap::getDeviceName()
{
    return mName;
}

/**
 * @brief Returns the device address in IPV4 format.
 *
 * @returns A string with the device address in IPV4 format.
 */
inline string Tuntap::getDeviceAddress()
{
    return mAddr;
}

/**
 * @brief Returns the device socket.
 *
 * @returns A negative value on error (error list in Tuntap.h). The socket if the init was ok.
 */
inline int Tuntap::getDeviceSocket()
{
    return mSocket;
}

/**
 * @brief Reads data from the device's socket.
 *
 * @param buffer Output buffer to receive the read data.
 *
 * @param size Size of the output buffer.
 */
inline int Tuntap::read(void *buffer, unsigned int size)
{
    return ::read(mSocket, buffer, size);
}

/**
 * @brief Writes data to the device's socket.
 *
 * @param buffer Input buffer to send the data.
 *
 * @param size Size of the data to be sent.
 */
inline int Tuntap::write(void *buffer, unsigned int size)
{
    return ::write(mSocket, buffer, size);
}

#endif

