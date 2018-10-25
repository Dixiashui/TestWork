#ifndef SctpUpperSend_H
#define SctpUpperSend_H

/**
@brief SctpUpperReceive defines a send method to receive data from the Upper protocol layer.

@author Franco Sinhori <franco.sinhori.ext\@siemens.com>
*/
class SctpUpperSend
{
public:
    SctpUpperSend() {};
    virtual ~SctpUpperSend() {};
    virtual bool send(const void *msg, unsigned int len, unsigned short stream,
                    bool ordered) = 0;
    virtual bool send(const void *msg, unsigned int len, unsigned short stream, bool ordered,  char *ip, unsigned short &port){ return false; };
};

#endif  // SctpUpperSend

