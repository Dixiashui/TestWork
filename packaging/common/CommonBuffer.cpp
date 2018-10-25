#include <string.h>
#include "common/CommonBuffer.h"
#include "common/CommonException.h"

/**
 * @brief CommonBuffer constructor
 *
 * Just point to the buffer.
 */
CommonBuffer::CommonBuffer(char *buffer, unsigned int size, bool destruct) : _buffer(buffer), _size(size), _destruct(destruct)
{
}

/**
 * @brief CommonBuffer constructor
 *
 * Create a new buffer.
 */
CommonBuffer::CommonBuffer(unsigned int size) : _buffer(new char[size]), _size(size), _destruct(true)
{
}

/**
 * @brief CommonBuffer destructor
 */
CommonBuffer::~CommonBuffer()
{
    if (_destruct) {
        delete [] _buffer;
    }
}

/**
 * @brief Gets buffer from within the buffer.
 *
 * @param idx  Index inside the buffer.
 *
 * @param size Size of the sub-buffer.
 *
 * @returns A pointer to where the sub-buffer begins or null.
 */
char* CommonBuffer::getBytes(unsigned int idx, unsigned int size)
{
    if ( (idx + sizeof(char))  > _size  || (idx + size * sizeof(char)) > _size ) {
        throw CommonException("(CommonBuffer) Out of range!");
    }

    return (char*)(_buffer+idx);
}

/**
 * @brief Gets a char (1 byte) from the buffer.
 *
 * @param idx  Index inside the buffer.
 *
 * @returns A char value.
 */
char CommonBuffer::getChar(unsigned int idx)
{
    if ( (idx + sizeof(char))  > _size ) {
        throw CommonException("(CommonBuffer) Out of range!");
    }

    return *(char*)(_buffer+idx);
}

/**
 * @brief Gets a char (1 byte) from the buffer.
 *
 * @param idx  Index inside the buffer.
 *
 * @returns A char value.
 */
unsigned char CommonBuffer::getUChar(unsigned int idx)
{
    if ( (idx + sizeof(char))  > _size ) {
        throw CommonException("(CommonBuffer) Out of range!");
    }

    return *(unsigned char*)(_buffer+idx);
}

/**
 * @brief Gets a short (2 bytes) from the buffer.
 *
 * @param idx  Index inside the buffer.
 *
 * @returns A short value.
 */
short CommonBuffer::getShort(unsigned int idx)
{
    if ( (idx + sizeof(short))  > _size ) {
        throw CommonException("(CommonBuffer) Out of range!");
    }

    return *(short*)(_buffer+idx);
}

/**
 * @brief Gets a short (2 bytes) from the buffer.
 *
 * @param idx  Index inside the buffer.
 *
 * @returns A short value.
 */
unsigned short CommonBuffer::getUShort(unsigned int idx)
{
    if ( (idx + sizeof(short))  > _size ) {
        throw CommonException("(CommonBuffer) Out of range!");
    }

    return *(unsigned short*)(_buffer+idx);
}

/**
 * @brief Gets a int (4 bytes) from the buffer.
 *
 * @param idx  Index inside the buffer.
 *
 * @returns An int value.
 */
int CommonBuffer::getInt(unsigned int idx)
{
    if ( (idx + sizeof(int))  > _size ) {
        throw CommonException("(CommonBuffer) Out of range!"); 
    }

    return *(int*)(_buffer+idx);
}

/**
 * @brief Inserts a 2nd buffer (N bytes) into the buffer.
 *
 * @param idx     Index inside the buffer.
 * @param buffer  The buffer to be inserted.
 * @param size    Amount of bytes from buffer to be inserted.
 */
void CommonBuffer::putBytes(unsigned int idx, const char *buffer, unsigned int size)
{
    if ( (idx + sizeof(char))  > _size || (idx + size * sizeof(char)) > _size ) {
        throw CommonException("(CommonBuffer) Out of range!");
    }
    
    memcpy(_buffer + idx, (void *)buffer, sizeof(char) * size);
}

/**
 * @brief Inserts a char (1 byte) into the buffer.
 *
 * @param idx    Index inside the buffer.
 * @param value  A value to be inserted.
 */
void CommonBuffer::putChar(unsigned int idx, char value)
{
    if ( (idx + sizeof(char))  > _size ) {
        throw CommonException("(CommonBuffer) Out of range!");
    }

    memcpy(_buffer + idx, (void *)&value, sizeof(char));
}

/**
 * @brief Inserts a short (2 bytes) into the buffer.
 *
 * @param idx    Index inside the buffer.
 * @param value  A value to be inserted.
 */
void CommonBuffer::putShort(unsigned int idx, short value)
{
    if ( (idx + sizeof(short))  > _size ) {
        throw CommonException("(CommonBuffer) Out of range!");
    }

    memcpy(_buffer + idx, (void *)&value, sizeof(short));
}

/**
 * @brief Inserts a int (4 bytes) into the buffer.
 *
 * @param idx    Index inside the buffer.
 * @param value  A value to be inserted.
 */
void CommonBuffer::putInt(unsigned int idx, int value)
{
    if ( (idx + sizeof(int))  > _size ) {
        throw CommonException("(CommonBuffer) Out of range!");
    }

    memcpy(_buffer + idx, (void *)&value, sizeof(int));
}
    
/**
 */
void CommonBuffer::detach()
{
    // detach buffer to avoid memory release in destruction call.
    _buffer = NULL;
    _size   = 0;
}

/**
 * @brief Gets the buffer pointer.
 *
 * @returns The buffer pointer.
 */
char * CommonBuffer::getBuffer()
{
    return _buffer;
}

/**
 * @brief Gets the buffer size.
 *
 * @returns The buffer size.
 */
unsigned int CommonBuffer::getBufferSize()
{
    return _size;
}

