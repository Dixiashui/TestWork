#ifndef _COMMON_BUFFER_H
#define _COMMON_BUFFER_H

/**
 * @file CommonBuffer.h
 * @brief Defines the CommonBuffer class.
 */

/**
 * @brief This class is used to get bytes inside a buffer.
 *
 * @author Alexandre Maia Godoi <alexandre.maia@siemens.com>
 */
class CommonBuffer
{
public:
    CommonBuffer(char *buffer, unsigned int size, bool destruct=true);
    CommonBuffer(unsigned int size);
    ~CommonBuffer();

    char* getBytes(unsigned int idx, unsigned int size);
    char getChar(unsigned int idx);
    unsigned char getUChar(unsigned int idx);
    short getShort(unsigned int idx);
    unsigned short getUShort(unsigned int idx);
    int getInt(unsigned int idx);

    void putBytes(unsigned int idx, const char *buffer, unsigned int size);
    void putChar(unsigned int idx, char value);
    void putShort(unsigned int idx, short value);
    void putInt(unsigned int idx, int value);

    char * getBuffer();
    unsigned int getBufferSize();

    void detach();

private:
    char *_buffer;       ///< Data buffer.
    unsigned int _size;  ///< Data buffer size.
    bool _destruct;      ///< Flag to control the Buffer::mBuffer destruction.
};
    
#endif // ifndef _BUFFER_H

