#ifndef SBSCTPIP_H_
#define SBSCTPIP_H_


#include <memory>
#include "emuProtocolLayers/Sctp/SctpUpperSend.h"
#include "emuProtocolLayers/Sctp/SctpUpperReceive.h"
#include "emuProtocolLayers/Sctp/SctpClient.h"
#include "emuProtocolLayers/Sctp/SctpServer.h"
#include "common/JsiTwConnection.h"
#include "common/Util.h"
#include "common/TwLog.h"

// error codes
#define SB_SCTP_IP_OK                             0
#define SB_SCTP_IP_NOT_CONFIGURED                 1
#define SB_SCTP_IP_CANNOT_SEND_DATA               2
#define SB_SCTP_IP_ALREADY_CONFIGURED             3
#define SB_SCTP_IP_CANNOT_GET_CONFIG_VALUES       4
#define SB_SCTP_IP_CANNOT_CONFIGURE               5
#define SB_SCTP_IP_SCTP_CONNECTION_FAIL           7
#define SB_SCTP_IP_CANNOT_SET_OUTPUT_STREAM       8
#define SB_SCTP_IP_CANNOT_EXECUTE_COMMAND         9
#define SB_SCTP_IP_RE_CONNECT_TIMEOUT             10

// error codes as strings
#define STR_SB_SCTP_IP_OK                          ""
#define STR_SB_SCTP_IP_NOT_CONFIGURED              "(SbSctpIp) Protocol Stack is not configured"
#define STR_SB_SCTP_IP_CANNOT_SEND_DATA            "(SbSctpIp) Cannot send data to remote host"
#define STR_SB_SCTP_IP_ALREADY_CONFIGURED          "(SbSctpIp) Protocol Stack is already configured"
#define STR_SB_SCTP_IP_CANNOT_GET_CONFIG_VALUES    "(SbSctpIp) Cannot get configuration values"
#define STR_SB_SCTP_IP_CANNOT_CONFIGURE            "(SbSctpIp) Cannot configure protocol stack"
#define STR_SB_SCTP_IP_INVALID_ERROR_CODE          "(SbSctpIp) Invalid error code"
#define STR_SB_SCTP_IP_SCTP_CONNECTION_FAIL        "(SbSctpIp) Cannot connect SCTP protocol"
#define STR_SB_SCTP_IP_CANNOT_SET_OUTPUT_STREAM    "(SbSctpIp) Invalid output stream value"
#define STR_SB_SCTP_IP_CANNOT_EXECUTE_COMMAND      "(SbSctpIp) Cannot execute command from Testwork"
#define STR_SB_SCTP_IP_RE_CONNECT_TIMEOUT          "(SbSctpIp) re-connect timeout"

// generic commands
#define COMMAND_SET_OUTPUT_STREAM         1
#define COMMAND_SET_SCTP_LOAD_CONTROL       2

#define SCTP_SERVER_DEFAULT_TIMEOUT      8000
#define DEFAULT_SCTP_OUT_STREAM          1

#define SCTP_TEMP_BUF_LEN 256
#define DATAINDEXT_FLAG_IP  1
#define DATAINDEXT_FLAG_PORT 2
#define DATAINDEXT_DISCONNECT 3
// SbSctpIp Configuration
struct ConfigSbSctpIp
{
    std::string localAddr;
    VectorString optLocalAddrs;
    unsigned short localPort;
    std::string remoteAddr;
    VectorString optRemoteAddrs;
    unsigned short remotePort;
    unsigned short outStreams;
    unsigned short inStreams;
    unsigned int timeout;
    bool isServer;
    unsigned int protocolId;
    bool ipCheck;
};

/**
 * @brief SbSctpIp Testwork Stack builder for the stack Sctp/Ip.
 *
 * This class creates and configures all protocol layers of this stack.
 * It also has a tcp socket connection (JsiTwConnection) to the Testwork Java part.
 *
 * @author Franco Sinhori <franco.sinhori.ext\@siemens.com>
*/
class SbSctpIp : public JsiTwConnection, public SctpUpperReceive
{
public:
    SbSctpIp(int id, char *port);
    virtual ~SbSctpIp();

    virtual int send (char *buffer, int size);
    virtual std::string getErrorMsg(int error); 
    virtual int configReq();
    virtual int cleanUp();
    virtual int execCommand(const char *buf);
    void setSctpUpperSend(SctpUpperSend * p) {mpSctpUpperSend = p;}
    void receive(char *buffer, int size, unsigned int stream);
    void receiveEx(char *bufferEx, int size);
    int send(char *buf, int size, char *bufext, int len);
    bool getIpPortFromExtCmd(char *bufext, int len, std::string &ip, unsigned short &port);

private:
    SctpUpperSend * mpSctpUpperSend;    ///< Pointer to send data to the Sctp layer.
    SctpCnx * mpSctpConn;               ///< Pointer to the Sctp protocol layer application
    ConfigSbSctpIp mConfigSbSctpIp;     ///< Object to hold all necessary parameters to configure the stack.
    ConfigSctpLayer mConfigSctpLayer;   ///< Object to hold all necessary parameters to configure SCTP layer.
    TwLog mLog;                         ///< Object used for logging.
    unsigned short mOutStream;          ///< Stream used to send data.
	
    int config();
    int getConfigValues(ConfigSbSctpIp &configSbSctpIp);
    void parseCommand(const char *buffer, int &command, int &msgSize, char *&msg);
    int setOutputStream(int stream);
    int setSctpLoadControl(int c);
};


#endif /*SBSCTPIP_H_*/
