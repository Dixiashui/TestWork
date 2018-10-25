#ifndef SctpUpperReceive_H
#define SctpUpperReceive_H

/**
@brief SctpUpperReceive defines a receive method to deliver data to the Upper protocol layer.

@author Franco Sinhori <franco.sinhori.ext\@siemens.com>
*/
class SctpUpperReceive
{
public:
    SctpUpperReceive() {};
    virtual ~SctpUpperReceive() {};
    virtual void receive(char *buffer, int size, unsigned int stream) = 0;
    virtual void receiveEx(char *bufferEx, int size){};
};

#endif  // SctpUpperReceive_H
