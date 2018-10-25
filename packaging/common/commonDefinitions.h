#ifndef COMMONDEFINITIONS_H_
#define COMMONDEFINITIONS_H_

#include <tiff.h>
#include <limits.h>
#if 0
#ifndef uint8
typedef unsigned char uint8;
#endif
#ifndef uint16
typedef unsigned short int uint16;
#endif
#ifndef uint32
typedef unsigned int uint32;
#endif

#endif

#ifndef byte
typedef uint8 byte;
#endif
#ifndef word
typedef uint16 word;
#endif
#ifndef dword
typedef uint32 dword;
#endif

#ifndef BitSizeOfByte
#define BitSizeOfByte (CHAR_BIT)
#endif

#endif /*COMMONDEFINITIONS_H_*/
