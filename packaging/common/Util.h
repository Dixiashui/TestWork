#ifndef _UTIL_H
#define _UTIL_H

/**
 * @file Util.h
 * @brief Defines the Util class.
 */

#include <stdio.h>
#include <string.h>
#include <sstream>
#include <iomanip>
#include <bitset>
#include <time.h>
#include <math.h>
#include <vector>

#include <assert.h>
#include "commonDefinitions.h"

#define DEFAULT_SHELL_OUTPUT_BUFFER_LENGTH      1024

#define RELEASE_POINTER(p)       \
if(p != NULL)                    \
{                                \
    delete p;                    \
    p = NULL;                    \
}

#define RELEASE_ARRAY_POINTER(p)       \
if(p != NULL)                   \
{                               \
    delete[] p;                   \
    p = NULL;                   \
}


typedef std::vector<std::string> StringVector;
/**
 * @brief This class provides some basic services.
 *
 * @author Alexandre Maia Godoi <alexandre.maia@siemens.com>
 */
class Util
{
public:
    static std::string toString(unsigned int n);
    static std::string toString(int n);
    static std::string toString(unsigned short n);
    static std::string toString(short n);
    static std::string toString(unsigned long n);
    static std::string toString(long n);
    static std::string toString(unsigned char n);
    static std::string toString(char n);
    static std::string toString(bool b);
    static std::string toString(double n);
    
    static std::string toHexString(unsigned int n, bool fill=false);
    static std::string toHexString(int n, bool fill=false);
    static std::string toHexString(unsigned short n, bool fill=false);
    static std::string toHexString(short n, bool fill=false);
    static std::string toHexString(unsigned long n, bool fill=false);
    static std::string toHexString(long n, bool fill=false);
    static std::string toHexString(unsigned char n, bool fill=false);
    static std::string toHexString(char n, bool fill=false);
    static std::string toHexString(const unsigned char *buffer, unsigned int len);
    
    static std::string toOctString(unsigned int n);
    static std::string toOctString(int n);
    static std::string toOctString(unsigned short n);
    static std::string toOctString(short n);
    static std::string toOctString(unsigned long n);
    static std::string toOctString(long n);
    static std::string toOctString(unsigned char n);
    static std::string toOctString(char n);
    
    static std::string toBinString(unsigned int n);
    static std::string toBinString(int n);
    static std::string toBinString(unsigned short n);
    static std::string toBinString(short n);
    static std::string toBinString(unsigned long n);
    static std::string toBinString(long n);
    static std::string toBinString(unsigned char n);
    static std::string toBinString(char n);
    static std::string toBinString(const unsigned char *buffer, unsigned int len);

    static void ltrim(std::string &s, const std::string &trimset=" \t\r\n");
    static void rtrim(std::string &s, const std::string &trimset=" \t\r\n");
    static void trim(std::string &s, const std::string &trimset=" \t\r\n") ;
    
    static void decreaseWithModulus(int& v, int m, int offset = 1);
    static void increaseWithModulus(int& v, int m, int offset = 1) 
    {
        assert(v>=0 && m>=0 && offset>=0);
        v = (v+offset) % m ;
    }
    
    static bool stringContains(std::string s, std::string dest);
    static bool splitStringToVector(std::string s, char splitter, StringVector &vector);
    
    static std::string processShellCommand(const char * cmd, int length = DEFAULT_SHELL_OUTPUT_BUFFER_LENGTH);
    
    static unsigned int getBitSizeFromByteSize(unsigned int byteSize) {return byteSize * BitSizeOfByte;}

    static int nocaseCmp(const std::string &s1, const std::string &s2);

    static void milliSleep(long timeout);
    static void absTimeout(long timeout, timespec &tv);
    static bool absTimeEqual(struct timeval &src, struct timeval &dst, int accuracy);

    static int get_file_length(char* file_path);

    static inline int readIntFromBuf(byte** readptr)
    {
        int ret = *(int *)(*readptr);
        (*readptr) += sizeof(int);
        return ret;
    }
    static inline std::string readStrFromBuf(byte** readptr, int num)
    {
        if(num == 0)
            return "";
        char outptr[num + 1];
        memcpy(outptr, (char *)(*readptr), num);
        outptr[num] = '\0';
        (*readptr) += num;
        return outptr;
    }
    static inline std::string readStrParamFromBuf(byte** readptr)
    {
        int len = readIntFromBuf(readptr);
        return readStrFromBuf(readptr, len);
    }
    /**
     * @brief Calculates CRC of a given frame.
     *
     * @param message         Frame. CRC is calculated on that frame.
     * @param messageSize     Frame size in BYTES.
     * @param polynomial      Polynomial used to divide the frame.
     * @param polynomialSize  Polynomial size in BITS. For instance, the polynomial x3 + x + 1 is 4 bits size.
     *
     * @returns The CRC for the given frame and polynominal.
     */
    template <typename T>
    static T calcCrc(const char *message, int messageSize, T polynomial, int polynomialSize)
    {
        const int BYTE_TOP_BIT             = 0x80;
        const T REMAINDER_TOP_BIT          = static_cast<T>(pow(2, polynomialSize - 1));
        const T REMAINDER_SIGNIFICANT_BITS = static_cast<T>(pow(2, polynomialSize - 1) - 1);

        T remainder = 0;

        // perform division, a byte at a time.
        for (int byte = 0; byte < messageSize; ++byte)
        {
            // perform division, a bit at a time.
            for (int bit = 7, shift = BYTE_TOP_BIT; bit >= 0; --bit, shift >>= 1)
            {
                remainder <<= 1;
                remainder |= static_cast<T>((message[byte] & shift) >> (bit));

                // try to divide the current data bit.
                if (remainder & REMAINDER_TOP_BIT)
                {
                    remainder ^= polynomial;
                }
            }
        }

        // add polynomialSize to the end of the message
        for (int i = 1; i < polynomialSize; i++)
        {
            remainder <<= 1;

            if (remainder & REMAINDER_TOP_BIT)
            {
                remainder ^= polynomial;
            }
        }

        // The final remainder is the CRC result.
        return (remainder & REMAINDER_SIGNIFICANT_BITS);
    }

private:
    static void confHexStream(std::ostringstream &s, bool fill, int size);
    static void confOctStream(std::ostringstream &s);

    /**
     * @brief Util constructor. Cannot be instanced.
     */
    Util();

    /**
     * @brief Util destructor. Cannot be deleted.
     */
    ~Util();
};

#endif

